const {app, BrowserWindow, ipcMain, dialog} = require('electron')
const config = require('./config')
const Dictionary = require('./engines/translator/Dictionary')
const path = require('path')
const {DymoServices, createImageWithText} = require('node-dymo-printer')
const ScaleService = require('./service/scaleService')
const twig = require('electron-twig')
let mainWindow = null

function createWindow () {
    const dictPath = `${__dirname}/client/ui/pages/main/main.trans.js`
    const t = Dictionary.load(dictPath, {})

    mainWindow = new BrowserWindow({
        kiosk: config.get('kiosk'),
        webPreferences: {
            preload: path.join(__dirname, 'preload.js')
        }
    })

    if (config.get('devMode')) {
        mainWindow.webContents.openDevTools();
    }

    mainWindow.loadURL(`file://${__dirname}/client/ui/pages/main/main.html.twig`);
    twig.view = {
        staticDir: __dirname + '/web',
        t
    }
}

app.whenReady().then(() => {
    ipcMain.handle('call-printer', handleCallPrinter)

    ipcMain.handle('call-fingerprint-reader', handleCallFingerprintReader)

    ipcMain.handle('call-scale', handleCallScale)

    ipcMain.handle('change-page', (event, page) => handlePageChange(page))

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

function handlePageChange(page) {
    switch(page) {
        case 'subpage1':
            mainWindow.loadURL(`file://${__dirname}/client/ui/pages/subpage1/subpage1.html.twig`)
            break
        default:
            mainWindow.loadURL(`file://${__dirname}/client/ui/pages/${page}/${page}.html.twig`)
            break
    }
}
