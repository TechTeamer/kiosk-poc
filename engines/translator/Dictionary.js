const localeConfig = require('./config')
const defaultLang = localeConfig.locales.default
const supportedLocales = localeConfig.locales.supported
// const logger = localeConfig.logger
const logger = console
const dicts = new Map()

/**
 * A Dictionary contains translation entries and can provide translations for the set locale.
 * A translation entry is a function that returns an key-value pair of locale<->translation.
 *
 * @class Dictionary
 * @example
 *
 * An example translation module:
 * // @param {Dictionary} dict
 * module.exports = function (dict) {
 *   dict.use(require('../../../resources/translations'))
 *
 *   dict.define({
 *     sayHi (name) {
 *       return {
 *         en: `Hello ${name}`,
 *         hu: `Szia $\{name}`
 *       }
 *     }
 *   })
 * }
 *
 * Usage in the browser:
 * const Dictionary = require('../../../../engines/translator/Dictionary')
 * const dict = Dictionary.create(require('./login.trans'))
 * console.log(dict.t('sign_in'))
 *
 * Usage in a twig template:
 * {{ t.t('sign_in') }}
 * Note that `t` is a global variable.
 *
 * */
class Dictionary {
  /**
   * Constant identification key for undefined intermediate parameter for translation saving into database
   * @returns {string}
   * @public
   */
  get UNDEFINED_PARAM () {
    return '__undefined__'
  }

  /**
   * @param {String} dictionaryPath
   * @param dictionaryOverrides
   * @return {Dictionary}
   * */
  static load (dictionaryPath, dictionaryOverrides) {
    let dictionary
    const absPath = require.resolve(dictionaryPath)

    if (!dicts.has(absPath) || dictionaryOverrides) {
      let dictModule
      try {
        dictModule = require(dictionaryPath)
      } catch (e) {
        logger.error('Unable to import dictionary module %s', dictionaryPath)
        throw new Error('Unable to import dictionary module')
      }
      dictionary = Dictionary.create(dictModule, defaultLang, dictionaryOverrides)
      dicts.set(absPath, dictionary)
    } else {
      dictionary = dicts.get(absPath)
    }

    return dictionary
  }

  /**
   * @param {function(Dictionary)} transModule
   * @param {String} [locale] default locale
   * @param {Object} [dictionaryOverrides={}]
   * @param {String} [namespace='']
   * @param {String} [parentNamespace='']
   * @return {Dictionary}
   * */
  static create (transModule, locale = defaultLang, dictionaryOverrides, namespace = '', parentNamespace = '') {
    const dictionary = new Dictionary(supportedLocales, dictionaryOverrides, namespace, parentNamespace)
    dictionary.locale = locale
    dictionary.use(transModule)
    return dictionary
  }

  constructor (locales = [], overrides = {}, namespace = '', parentNamespace = '') {
    this.locales = locales
    this.locale = defaultLang
    this.translations = new Map()
    this.overrides = overrides || {}
    this.namespace = namespace
    this.nsPath = (parentNamespace ? parentNamespace + '.' : '') + namespace
    this.namespaces = new Map()
    this.children = []
  }

  setLocale (locale) {
    this.locale = locale
    this.children.forEach(dict => dict.setLocale(locale))
  }

  /**
   * Import translations from a dictionary module.
   * A dictionary module is a kind of mixin you can use import translations to a dictionary.
   *
   * @example
   * // @param {Dictionary} dict
   * module.exports = function (dict) {
   *   dict.define({
   *     sayHi (name) {
   *       return {
   *         en: `Hello ${name}`,
   *         hu: `Szia $\{name}`
   *       }
   *     }
   *   })
   * }
   *
   * @param {function(Dictionary)} transModule
   * */
  use (transModule) {
    if (typeof transModule === 'function') {
      transModule(this)
    }
  }

  /**
   * @param {String} namespace
   * @return {Dictionary}
   */
  ns (namespace) {
    if (this.namespaces.has(namespace)) {
      return this.namespaces.get(namespace)
    } else {
      logger.warn('Dictionary namespace not found %s, falling back to base dictionary', namespace)
      return this
    }
  }

  /**
   * Define translations in this dictionary
   *
   * @example
   * dictionary.define({
   *   sayHi (name) {
   *     return {
   *       en: `Hello ${name}`,
   *       hu: `Szia $\{name}`
   *     }
   *   }
   * })
   *
   * @param {Object} translations
   * */
  define (translations) {
    Object.keys(translations).forEach(key => {
      const translateFn = translations[key]
      this.translations.set(key, translateFn)
    })
  }

  tAll (key, ...args) {
    let joinedArgs
    /**
     * Removes the undefined argument items from the last positions, but does not remove the intermediate undefined items
     * @param {string[]} items
     * @param {boolean} [stringifyUndefined]
     * @returns {string[]}
     */
    const truncateArgs = (items, stringifyUndefined) => {
      const result = []
      let hasValue = false

      items.slice().reverse().forEach((item) => {
        if (typeof item !== 'undefined') {
          hasValue = true
        }
        if (hasValue) {
          if (typeof item === 'undefined' && stringifyUndefined) {
            // using the constant undefined key at concatenated string key
            result.unshift(this.UNDEFINED_PARAM)
          } else {
            result.unshift(item)
          }
        }
      })
      return result
    }

    if (args.length) {
      joinedArgs = truncateArgs(args, true).join('.')
    }

    const overrideArgs = joinedArgs ? `.${joinedArgs}` : ''
    const overrideKey = (this.nsPath ? this.nsPath + '.' : '') + key + overrideArgs
    const overrideTrans = this.overrides ? this.overrides[overrideKey] : null
    if (overrideTrans) {
      return overrideTrans
    }

    if (this.translations.has(key)) {
      const translateFn = this.translations.get(key)
      return typeof translateFn === 'function' ? translateFn(...truncateArgs(args)) : null
    }
  }

  /**
   * Translate a translation key for the set locale with these arguments.
   * If the wanted locale is missing from the translation entry,
   * it attempts to provide a value by trying the other locales from the precedence config.
   * Ultimately it returns a key if nothing was found.
   *
   * @param {String} key
   * @param {Array<*>} [args]
   * @return {String}
   * */
  t (key, ...args) {
    const translations = this.tAll(key, ...args)
    const locale = this.locale

    if (translations) {
      if (Object.prototype.hasOwnProperty.call(translations, locale)) {
        return translations[locale]
      } else {
        const fallbackLocale = this.locales.find(l => Object.prototype.hasOwnProperty.call(translations, l))
        if (Object.prototype.hasOwnProperty.call(translations, fallbackLocale)) {
          return translations[fallbackLocale]
        }
      }
    }

    if (args && args.length) {
      logger.warn('Missing translation "%s" in "%s" locale. arguments: %s', key, locale, JSON.stringify(args))
    } else {
      logger.warn('Missing translation "%s" in "%s" locale', key, locale)
    }

    return key
  }

  /**
   * @param {String|function(Dictionary)} [namespace]
   * @param {function(Dictionary)} transModule
   * @return {Dictionary}
   * */
  create (namespace, transModule) {
    if (!transModule) {
      transModule = namespace
      namespace = null
    }
    const dictionary = Dictionary.create(transModule, this.locale, this.overrides, namespace, this.namespace)
    if (namespace) {
      this.namespaces.set(namespace, dictionary)
    }
    this.children.push(dictionary)
    return dictionary
  }
}

module.exports = Dictionary
