const stylus = require('stylus')
const CleanCSS = require('clean-css')
const breakpoints = require('../../client/resources/breakpoints')
const deviceSizes = require('../../client/resources/device-sizes.json')
const colors = require('../../client/resources/colors')

// const config = require('../../config')
const poststylus = require('@techteamer/poststylus')
const autoprefixer = require('autoprefixer')

const cleanCss = new CleanCSS({
  returnPromise: true,
  sourceMap: true //config.build.generateSourceMaps
})

/**
 * This helper compiles a single stylus string into a css string.
 *
 * @param {BundleOptions} options
 * @param {BundleContext} context
 * @return Promise<CompilerResults>
 * */
async function compileStylus (options, context) {
  return new Promise((resolve, reject) => {
    const contents = context.contents
    const filePath = context.filePath

    const bundler = stylus(contents, {
      'include css': true
    })
    bundler.set('filename', filePath)
    bundler.set('paths', [filePath, options.importRoot, process.cwd()])
    // if (config.build.generateSourceMaps) {
    if (true) {
      bundler.set('sourcemap', { inline: true, comment: false })
    }
    bundler.import('client/ui/styles/global.styl')

    Object.keys(breakpoints).forEach(function (name) {
      bundler.define(name, breakpoints[name])
    })
    Object.keys(deviceSizes).forEach(function (name) {
      bundler.define(name, new stylus.Parser(deviceSizes[name] + 'px').peek().val)
    })
    Object.keys(colors).forEach(function (name) {
      // Ehh..: http://stackoverflow.com/questions/29682331/how-to-define-a-color-with-stylus
      bundler.define(name, new stylus.Parser(colors[name]).peek().val)
    })
    bundler.use(poststylus([
      autoprefixer()
    ]))

    bundler.render(function (err, css) {
      if (err) {
        reject(err)
      } else {
        // if (config.build.minify) {
        if (false) {
          cleanCss.minify(css)
            .then(output => {
              resolve({ output: output.styles, stats: { size: css.length, minSize: output.styles.length } })
            })
            .catch(reject)
        } else {
          resolve({ output: css, stats: { size: css.length } })
        }
      }
    })
  })
}

module.exports = compileStylus
