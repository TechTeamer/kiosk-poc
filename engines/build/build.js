const watch = require('./watch')
const path = require('path')
const bundle = require('./bundle')
const program = require('commander')
  .storeOptionsAsProperties()
  .option('--watch', 'Watch', false)
  .option('--no-exit', 'Prevent auto exit', false)
  .parse(process.argv)

const autoExit = program.exit !== false
const runWatcher = program.watch
const pageArgs = program.args || []
let pathFilter = null

console.info('build', runWatcher ? 'watch' : 'no-watch', autoExit ? 'auto exit' : 'no-exit', pageArgs.length ? pageArgs.join(', ') : 'all pages')

if (pageArgs.length) {
  const allowedPaths = pageArgs.map((pageName) => {
    const includePaths = [
      path.join(process.cwd(), 'client/ui/pages', pageName),
      path.join(process.cwd(), 'client/ui/layouts', pageName),
      path.join(process.cwd(), 'customization/ui/layouts', pageName),
      path.join(process.cwd(), 'customization/ui/pages', pageName)
    ]
    return (srcPath) => includePaths.includes(srcPath) || includePaths.includes(path.dirname(srcPath))
  })
  pathFilter = (filePath) => {
    const srcPath = path.join(process.cwd(), path.relative(process.cwd(), filePath)) // convert to absolute even if it's relative path
    return allowedPaths.some(check => check(srcPath))
  }
}

/**
 * This function defines a builder that can (optionally) watch files and bundle them when they change.
 *
 * @param buildName
 * @param {Object} options
 * @param {string|string[]} [options.watch] file paths or glob patterns to watch
 * @param {string} [options.liveReload] file paths or glob patterns to watch
 * @param {Array<BundleOptions>} options.bundle the options to pass to `bundle()`
 * @return {Promise}
 * */
function build (buildName, options) {
  const watchPaths = options.watch
  const bundleOptions = options.bundle
  const liveReload = options.liveReload
  let liveReloadServer

  function bundler () {
    return Promise.resolve().then(() => {
      if (Array.isArray(bundleOptions)) {
        return Promise.all(bundleOptions.map((currOptions) => {
          currOptions.pathFilter = pathFilter
          return bundle(currOptions)
        }))
      } else {
        bundleOptions.pathFilter = pathFilter
        return bundle(bundleOptions)
      }
    }).then(() => {
      console.log('done', buildName)
      if (liveReloadServer) {
        liveReloadServer.refresh(buildName === 'style' ? 'all.css' : 'all.js')
      }
    })
  }

  return Promise.resolve().then(() => {
    return bundler()
  }).then(() => {
    if (liveReload) {
      require('./livereload')().then(lrs => {
        liveReloadServer = lrs
      })
    }
  }).then(() => {
    if (runWatcher) {
      if (Array.isArray(watchPaths)) {
        watchPaths.forEach(function (wp) {
          console.log('watching', wp)
        })

        watch(watchPaths, function onChange () {
          return bundler()
        })
      }
    } else if (autoExit) {
      process.exit(0)
    }
  })
}

module.exports = build
