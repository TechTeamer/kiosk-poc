const EventEmitter = require('events')
const hid = require('node-hid')

class ScaleService extends EventEmitter {
  constructor() {
    super()

    this.device = null
    this.scaleIsActive = false
    this.weight = {value: 0, unit: 'grams', overweight: false}
    // this.init()
  }

  init () {
    try {
      if (this.scaleIsActive) {
        return
      }

      // if (this.device) {
      //   this.device.removeAllListeners()
      //   this.device = null
      //
      //   // this.device.resume()
      //   console.log('SEGG', this.device)
      //   return
      // }

      // this.device = new hid.HID('/dev/hidraw3')
      this.device = new hid.HID(2338, 32771)

      if (!this.device) {
        this.emit('error', 'no-device')
        return
      }

      this.device.on('error', () => {
        this.scaleIsActive = false
        // this.device.close()
        this.emit('end')
      })

      this.device.on('data', (data) => {
        this.scaleIsActive = true

        let dataArray = data
        let change = false
        let overweight = false
        let value = 0
        let system = 'grams'
        const weight = {value: 0, unit: 'grams'}

        if (dataArray[1] === 2) {
          // no weight is on the scale
          weight.value = 0;
          overweight = false;
        }
        if (dataArray[2] === 11) {
          system = 'ounces';
        }
        if (dataArray[2] === 2) {
          system = 'grams';
        }
        if (dataArray[1] === 4 && system === 'ounces') {
          overweight = false;
          value = Math.round(((dataArray[4] + (dataArray[5] * 256)) * 0.1) * 10) / 10;
        }
        if (dataArray[1] === 4 && system === 'grams') {
          overweight = false;
          value = Math.round((dataArray[4] + dataArray[5] * 256) * 10) / 10;
        }
        if (dataArray[1] === 6) {
          // there's too much weight
          value = 0;
          overweight = true;
        }

        if (this.weight.value !== value) {
          this.weight.value = value;
          this.weight.system = system;
          this.emit('weight-change', {value: value, system: system});
          change = true;
        }
        if (this.weight.overweight !== overweight) {
          this.weight.overweight = overweight;
          this.emit('overweight-change', overweight);
          change = true;
        }

        // if (change === true) {
        //   this.emit('weight', weight);
        // }

        // console.log('DATA', data)
      })
    } catch (e) {
      this.emit('error', 'no-device')
    }
  }

  getStatus () {
    return this.scaleIsActive
  }
}

module.exports = new ScaleService()
