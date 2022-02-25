const hid = require('node-hid')

console.log(hid.devices())

const device = new hid.HID('/dev/hidraw3')

device.on('data', (data) => {
  console.log('DATA', data)
})

CY3Z9t6Hw9Pz
