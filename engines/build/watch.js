const chokidar = require('chokidar')

/**
 * This is a thin wrapper that is only capable of watching file changes.
 * It executes a function for every file change event.
 * The callback must return a promise, and until that promise is resolved, the watcher doesn't run it again.
 *
 * @param {string|string[]} src
 * @param {function} onChange
 * */
function watch (src, onChange) {
  let blocked = false
  chokidar.watch(src).on('change', function () {
    if (!blocked) {
      blocked = true
      onChange.apply(this, arguments).then(function () {
        blocked = false
      })
    }
  })
}

module.exports = watch
