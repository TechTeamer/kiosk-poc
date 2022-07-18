const build = require('../../engines/build/build')
const compileStylesheet = require('./style.compiler')

const param = {
  bundle: [
    { // pages
      src: 'client/ui/pages/**/*.style.styl',
      bundleRoot: 'client/ui/pages/',
      importRoot: 'client/ui/',
      outputRoot: 'web/css',
      ext: '.css',
      compile: compileStylesheet
    },
    { // layouts
      src: 'client/ui/layouts/**/*.layout.styl',
      bundleRoot: 'client/ui/layouts/',
      importRoot: 'client/ui/',
      outputRoot: 'web/css',
      ext: '.css',
      compile: compileStylesheet
    },
    { // custom pages
      src: 'customization/ui/pages/**/*.style.styl',
      bundleRoot: 'customization/ui/pages/',
      importRoot: 'customization/ui/',
      outputRoot: 'web/css',
      ext: '.css',
      compile: compileStylesheet
    },
    { // custom layouts
      src: 'customization/ui/layouts/**/*.layout.styl',
      bundleRoot: 'customization/ui/layouts/',
      importRoot: 'customization/ui/',
      outputRoot: 'web/css',
      ext: '.css',
      compile: compileStylesheet
    }
  ]
}

if (process.argv.includes('--watch')) {
  param.watch = [
    'client/**/*.styl',
    'customization/**/*.styl'
  ]
  param.liveReload = '*.css'
}

module.exports = build('style', param)
