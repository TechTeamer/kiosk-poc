process.argv.push('--no-exit') // prevent individual builds to exit too soon

Promise.all([
  require('../style/style.task')
]).then(() => {
  console.log('done', 'Build')
  if (!process.argv.includes('--watch')) {
    process.exit()
  }
})
