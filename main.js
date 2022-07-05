const {app, BrowserWindow, ipcMain, dialog} = require('electron')
const path = require('path')
const {DymoServices, createImageWithText} = require('node-dymo-printer')
const ScaleService = require('./service/scaleService')
let mainWindow = null

function createWindow () {
    mainWindow = new BrowserWindow({
        kiosk: false,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    })

    mainWindow.loadFile('index.html')
}

app.whenReady().then(() => {
    ipcMain.handle('call-printer', handleCallPrinter)

    ipcMain.handle('call-fingerprint-reader', handleCallFingerprintReader)

    ipcMain.handle('call-scale', handleCallScale)

    createWindow()

    app.on('activate', function () {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow()
        }
    })
})

app.on('window-all-closed', function () {
    if (process.platform !== 'darwin') {
        app.quit()
    }
})

async function handleCallPrinter() {
    // Create landscape image with the dimensions of the label and with the text "Hello World!".
    const {imageWidth, imageHeight} = DymoServices.DYMO_LABELS['89mm x 36mm'];
    const image = await createImageWithText(imageWidth, imageHeight, 50, 128, 'Hello World!');

    // For debugging purposes, write the image to disk.
    image.write(__dirname + '/image1.png');

    const ds = new DymoServices()
    console.log(await ds.listPrinters())

    // Print it, just one label.
    await ds.print(image, 1);
    console.log('Successfully printed');
}

function handleCallFingerprintReader() {
    dialog.showMessageBox({
        message: 'Ujjlenyomat olvasó'
    })
}

function handleCallScale() {
    if(!mainWindow) {
        return
    }

    const result = {
        message: '',
        data: null
    }

    ScaleService.removeAllListeners()

    ScaleService.on('error', (data) => {
        result.message = 'error'
        result.data = data
        mainWindow.webContents.send('update', result)
    })

    ScaleService.on('weight-change', (data) => {
        result.message = 'weight-change'
        result.data = data

        console.log('weight-change', result)
        mainWindow.webContents.send('update', result)
    })

    ScaleService.on('overweight-change', (data) => {
        console.log('overweight-change', data)
        result.message = 'overweight-change'
        result.data = data
        mainWindow.webContents.send('update', result)
    })

    ScaleService.on('weight', (data) => {
        console.log('weight', data)
        result.message = 'error'
        result.data = data
        mainWindow.webContents.send('update', result)
    })

    ScaleService.on('end', () => {
        console.log('end')
        result.message = 'end'
        result.data = null
        mainWindow.webContents.send('update', result)
    })

    ScaleService.init()
}
