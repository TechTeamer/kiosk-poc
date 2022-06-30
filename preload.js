const { contextBridge, ipcRenderer } = require('electron')

contextBridge.exposeInMainWorld('electronAPI', {
    callPrinter: () => ipcRenderer.invoke('call-printer'),
    callFingerprintReader: () => ipcRenderer.invoke('call-fingerprint-reader'),
    callScale: () => ipcRenderer.invoke('call-scale')
})
