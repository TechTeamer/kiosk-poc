/** @param {Dictionary} dict */
module.exports = function (dict) {
  dict.use(require('../../../../customization/ui/regions/header/header.trans'))
  dict.use(require('../../../../customization/ui/regions/footer/footer.trans'))

  dict.use(require('../../../../customization/ui/layouts/default/default.trans.custom'))

  dict.define({
    page_title: () => ({
      en: 'Main page',
      hu: 'Főoldal'
    }),
    image_text: () => ({
      en: 'Advertising graphics and text',
      hu: 'Reklám grafika és szöveg'
    }),
    menu_primary_text: () => ({
      en: 'Please choose from the following processes:',
      hu: 'Kérjük, hogy válasszon az alábbi folyamatok közül:'
    }),
    menu_secondary_text: () => ({
      en: 'Hardware/device use cases:',
      hu: 'Hardver/eszköz felhasználási esetek:'
    }),
    menu_item_real_time_identification: () => ({
      en: 'Real time identification',
      hu: 'Valós idejű azonosítás'
    }),
    menu_item_non_real_time_identification: () => ({
      en: 'Non real time identification',
      hu: 'Nem valós idejű azonosítás'
    }),
    menu_item_face_photo: () => ({
      en: 'Face photo',
      hu: 'Arckép felvétel'
    }),
    menu_item_post_demo: () => ({
      en: 'Post demo',
      hu: 'Posta demo'
    }),
    menu_item_pos_payment_demo: () => ({
      en: 'POS payment demo',
      hu: 'POS fizetés demo'
    }),
    menu_item_fingerprint_reading: () => ({
      en: 'Fingerprint reading',
      hu: 'Ujjlenyomat olvasás'
    }),
    menu_item_document_scanning: () => ({
      en: 'Document scanning',
      hu: 'Okmány beolvasás'
    }),
    menu_item_face_recognition_module: () => ({
      en: 'Face recognition module',
      hu: 'Arcazonosítás modul'
    }),
    menu_item_liveness_check: () => ({
      en: 'Liveness check',
      hu: 'Élőség ellenőrzés'
    }),
    menu_item_weighing: () => ({
      en: 'Weighing',
      hu: 'Mérleg beolvasás'
    }),
    menu_item_a4_printing: () => ({
      en: 'A4 printing',
      hu: 'A4-es nyomtatás'
    }),
    menu_item_label_printing: () => ({
      en: 'Label printing',
      hu: 'Címke nyomtatás'
    })
  })
}
