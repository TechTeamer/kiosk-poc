const path = require('path')
const getSrc = require('../util/getSrc')
const fs = require('fs-extra')

/**
 * @typedef {Object} BundleOptions
 *
 * @property {string} src             a glob pattern for files that will be processed
 * @property {boolean} [read = true]  whether or not to read the file contents
 * @property {boolean} [write = true] whether or not to write the output
 * @property {string} [bundleRoot]    this segment of the input path will be ignored when creating output file paths
 * @property {string} [importRoot]    this will be the root for imports/requires
 * @property {string} [outputRoot]    this is the output dir for compiled files
 * @property {function} [pathFilter]  a function to filter src files
 * @property {string} [ext]           the extension of the output file
 * @property {function} compile       this function will be called to process each file
 * */

/**
 * @typedef {Object} BundleContext
 *
 * @property {string} contents  the contents of the processed file
 * @property {string} filePath  the path of the processed file
 * */

/**
 * @typedef {Object} CompilerResults
 * @property {String} output
 * @property {String} sourceMap
 * @property {Object} stats
 * @property {Number} stats.size
 * @property {Number} stats.minSize
 * */

/**
 * This is a bundler tool that reads files from a glob pattern and executes a compiler function for each.
 * The compiler is called with two arguments: the options passed to the bundler and a context object that contains
 * the current file path, the file contents, a resolve and a reject function.
 *
 * @param {BundleOptions} options
 * @return {Promise}
 * */
async function bundle (options) {
  const src = options.src
  const bundleRoot = options.bundleRoot
  const outputRoot = options.outputRoot
  const pathFilter = options.pathFilter
  const ext = options.ext
  const read = Object.prototype.hasOwnProperty.call(options, 'read') ? options.read : true
  const write = Object.prototype.hasOwnProperty.call(options, 'write') ? options.write : true
  /** @type {function(BundleOptions, BundleContext):Promise<CompilerResults>} */
  const compile = options.compile

  // this creates a full path for the css from an input file path
  function createOutputFileName (filePath, fileExt) {
    if (!outputRoot) {
      return ''
    }

    const fileName = path.basename(filePath, path.extname(filePath))
    const fileDir = path.dirname(filePath)
    const namespace = path.relative(bundleRoot, fileDir)
    const outputFileName = fileName + fileExt
    return path.join(outputRoot, namespace, outputFileName)
  }

  const filePaths = await getSrc([src])
  let paths = filePaths

  if (typeof pathFilter === 'function') {
    paths = filePaths.filter(pathFilter)
  }

  // loop
  for (const filePath of paths) {
    const outputPath = createOutputFileName(filePath, ext)
    const sourceMapPath = createOutputFileName(filePath, ext + '.map')
    const startTime = Date.now()
    let contents

    if (read) {
      // read
      const buffer = await fs.readFile(filePath)
      contents = buffer.toString()
    } else {
      contents = null
    }

    try {
      // process
      const compilerResults = await compile(options, {
        contents,
        filePath
      })
      if (!compilerResults) {
        throw new Error(`Error compiling: ${filePath}`)
      }
      const { output, sourceMap, stats } = compilerResults || {}
      const { size = 0, minSize = 0 } = stats

      // write
      if (write && outputPath) {
        await fs.mkdirp(path.dirname(outputPath))
        await fs.writeFile(outputPath, output)
      }
      if (write && sourceMap) {
        await fs.mkdirp(path.dirname(sourceMapPath))
        await fs.writeFile(sourceMapPath, sourceMap)
      }

      // success
      const duration = ((Date.now() - startTime) / 1000)
      let sizeReport = '(size unknown)'

      if (size) {
        if (minSize) {
          const save = (size - minSize) / size * 100 >> 0
          sizeReport = `${(minSize / 1024).toFixed(2)}KB ${save}% minified`
        } else {
          sizeReport = `${(size / 1024).toFixed(2)}KB`
        }
      }
      const statReport = `${sizeReport} | ${duration}s`
      if (write && outputPath) {
        console.log('rendered', outputPath, statReport)
      } else {
        console.log('processed', outputPath, statReport)
      }
    } catch (err) {
      // error
      console.error('Error compiling file', filePath)
      console.trace(err)
    }
  }
}

module.exports = bundle
