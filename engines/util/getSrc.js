const glob = require('glob')
const minimatch = require('minimatch')

/**
 * Returns a list of file paths according to the pattern(s) given.
 * Filters duplicates with an option to exclude paths.
 *
 * @param {String|String[]} paths
 * @param {String|Object} [options] options passed to `glob` lib
 * @param {String[]} [exclude]
 * @return {Promise<String[]>}
 * */
async function getSrc (paths, options, exclude) {
  if (options === 'string') {
    options = { cwd: options }
  }

  const patterns = Array.isArray(paths) ? paths : [paths]
  const findings = []

  for (const pattern of patterns) {
    if (!glob.hasMagic(pattern)) {
      findings.push(paths)
    } else {
      const results = await new Promise((resolve, reject) => {
        glob(pattern, options, (err, matches) => {
          if (err) {
            reject(err)
          } else {
            resolve(matches)
          }
        })
      })
      findings.push(...results)
    }
  }

  const deduped = filterDuplicates(findings)
  const sources = excludePatterns(deduped, exclude)

  return sources
}

/**
 * @param {String[]} sources
 * @param {String[]} [exclude]
 * @return {String[]}
 * */
function excludePatterns (sources, exclude) {
  if (Array.isArray(exclude)) {
    exclude.forEach(function (pattern) {
      sources = sources.filter(function (src) {
        return !minimatch(src, pattern)
      })
    })
  }

  return sources
}

/**
 * @param {String[]} sources
 * @return {String[]}
 * */
function filterDuplicates (sources) {
  return sources.filter(function (src, i, array) {
    return array.indexOf(src) === i
  })
}

module.exports = getSrc
