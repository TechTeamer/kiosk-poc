const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('electronAPI', {
    callPrinter: () => ipcRenderer.invoke('call-printer'),
    callFingerprintReader: () => ipcRenderer.invoke('call-fingerprint-reader'),
    callScale: () => ipcRenderer.invoke('call-scale'),
    onScaleUpdate: (callback) => ipcRenderer.on('update', callback),
    changePage: (page) => ipcRenderer.invoke('change-page', page)
})
