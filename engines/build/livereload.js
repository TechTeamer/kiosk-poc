// const config = require('../../config')
// const fs = require('fs')

let startPromise = null

function start () {
  if (!startPromise) {
    startPromise = new Promise((resolve) => {
      // const c = config.build.liveReloadSettings
      //
      // if (c && c.https && c.https.key && c.https.cert) {
      //   c.https.key = fs.readFileSync(c.https.key)
      //   c.https.cert = fs.readFileSync(c.https.cert)
      // }

      // eslint-disable-next-line n/no-unpublished-require
      const liveReloadServer = require('livereload').createServer(null,() => {
        resolve(liveReloadServer)
      })
    })
  }

  return startPromise
}

module.exports = start
