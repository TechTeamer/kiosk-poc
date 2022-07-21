module.exports.locales = {
  // this object must conform to the one found in the config
  supported: (document.body.parentNode.dataset.supportedLocales || '').split(/,/),
  default: document.body.parentElement.lang
}
module.exports.logger = console
