const fs = require('fs')
const dot = require('dot-object')

let config
let localConfig

try {
  // config = require('getconfig')
  const origiConfig = require(`${__dirname}/config/dev.json`)

  const localConfigPath = `${__dirname}/config/local.json`

  if (fs.existsSync(localConfigPath)) {
    localConfig = require(localConfigPath)
  } else {
    localConfig = {}
  }

  config = Object.assign({}, origiConfig, localConfig);
} catch (e) {
  if (!(e instanceof SyntaxError)) {
    throw e
  }

  /* the SyntaxError provided by the library getconfig doesn't provide an error with fileName attribute,
       because of this I will parse the path from e.message and read the lines with the error in the json file.
   */
  const splittedMessage = e.message.split(':')
  const absPath = splittedMessage[0]
  let position
  try {
    position = splittedMessage[1].split(' ')[splittedMessage[1].split(' ').length - 1]
  } catch (err) {
    throw new Error(`a new error was thrown during the parsing of the following error message: ${err.message}`)
  }
  const start = parseInt(position, 10) - 20
  const end = parseInt(position, 10) + 20
  let jsonStringError
  try {
    jsonStringError = fs.readFileSync(absPath, 'utf8').substring(start, end)
  } catch (err) {
    throw new Error(`new error was thrown during the reading of the file with the following path: ${absPath}` +
      `the path was parsed from the following error string: ${err.message}`)
  }
  // eslint-disable-next-line no-console
  console.error(`${e.name} in json file located at: ${absPath} at position: ${position}\n` +
    'The portion of the file that have an error is the following:\n\n ' +
    `${jsonStringError}\n`)

  process.exit(2)
}

if (process.env.NODE_ENV === 'dev' || process.env.NODE_ENV === 'travisci') {
  let devDomain = process.env.DEV_DOMAIN
  if (!devDomain) {
    devDomain = fs.readFileSync('/etc/hostname').toString().trim()
  }

  const separator = devDomain.endsWith('facekomdev.net') ? '-' : '.'

  if (!config.hosts.css) {
    config.hosts.css = 'css' + separator + devDomain
  }
  if (!config.hosts.oss) {
    config.hosts.oss = 'oss' + separator + devDomain
  }
}

if (process.env.NODE_ENV === 'travisci') {
  config.queue.url = 'amqp://localhost:5672'
  config.queue.options = { rejectUnauthorized: false }
}

// Usage: config.get('foo.bar.baz') returns baz if config.foo && config.foo.bar && config.foo.bar.baz exist. Otherwise null
config.get = (objectPath, defaultValue = null) => {
  const path = objectPath.split('.')
  let current = config

  if (objectPath === '') {
    return current
  }

  while (path.length > 0) {
    if (!current[path[0]] && current[path[0]] !== false) {
      return defaultValue
    }
    current = current[path.shift()]
  }

  return current
}

config.has = (objectPath) => {
  const path = objectPath.split('.')
  let current = config

  if (objectPath === '') {
    return true
  }

  while (path.length > 0) {
    if (typeof current !== 'object' || current === null) {
      return false
    }
    if (!Object.prototype.hasOwnProperty.call(current, path[0])) {
      return false
    }
    current = current[path.shift()]
  }

  return true
}

/*
  to enable cloud config server add this to config  (for docs see: https://github.com/victorherraiz/cloud-config-client)
  "springCloudConfigServer": {
    "endpoint": "http://localhost:8888",
    "auth": { user: "username", pass: "password" },
    "name": "css",
    "profiles": ["cloud"]
  }
*/
if (config.springCloudConfigServer) {
  const promiseRetry = require('promise-retry')

  const client = require('cloud-config-client')
  config.loaded = promiseRetry({ maxTimeout: 2000 }, (retry, number) => {
    if (number > 1) {
      const logger = require('./server/logger') // cyclic reference logger<->config
      logger.warn('Try loading spring cloud config retry: ' + number)
    }

    return client.load(config.springCloudConfigServer).catch(retry)
  }).then(cloudconfig => {
    cloudconfig.forEach((key, value) => {
      key = key.replace('[', '.').replace(']', '')
      if (value) {
        dot.str(key, value, config)
      } else {
        dot.remove(key, config)
      }
    })

    config.loadedSpringCloudConfig = cloudconfig
    return config
  }).catch((err) => {
    const logger = require('./server/logger')
    logger.error('CANNOT LOAD SPRING CLOUD SERVER CONFIG', err)
    throw err
  })
} else {
  config.loaded = Promise.resolve()
}

config.getSecureConfig = () => {
  const conf = JSON.parse(JSON.stringify(config))
  delete conf.getconfig

  if (config.get('security.sensitiveConfigFields')) {
    for (const [key] of Object.entries(config.get('security.sensitiveConfigFields'))) {
      if (key.includes('.*.')) {
        const keyParts = key.split('.*.') || []
        const configValue = dot.pick(keyParts[0], conf)

        if (configValue) {
          for (const [i] of Object.entries(configValue)) {
            const fullKey = keyParts[0] + `.${i}.` + keyParts[1]
            dot.set(fullKey, 'SECRET', conf)
          }
        }
      } else {
        const configValue = dot.pick(key, conf)
        if (configValue) {
          dot.set(key, 'SECRET', conf)
        }
      }
    }
  }

  return conf
}

module.exports = config
