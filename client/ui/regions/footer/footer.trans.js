/** @param {Dictionary} dict */
module.exports = function (dict) {
  dict.define({
    copyright: (year) => ({
      en: 'Copyright ' + year + ' © All rights reserved.',
      hu: 'Copyright ' + year + ' © Minden jog fenntartva.'
    }),
    footer_terms: () => ({
      en: 'Terms & Conditions',
      hu: 'Felhasználási Feltételek'
    }),
    footer_ownership: () => ({
      en: 'Ownership Statement',
      hu: 'Tulajdonjogi Nyilatkozat'
    }),
    footer_privacy: () => ({
      en: 'Privacy Policy',
      hu: 'Adatvédelmi Nyilatkozat'
    }),
    footer_cookie: () => ({
      en: 'Cookies Policy',
      hu: 'Süti Szabályzat'
    }),
    footer_help: () => ({
      en: 'Help',
      hu: 'Segítség'
    })
  })
}
