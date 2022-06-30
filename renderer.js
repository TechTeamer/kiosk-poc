const scaleButton = document.getElementById('scale-btn')
const fingerPrintButton = document.getElementById('fingerprint-btn')
const printerButton = document.getElementById('printer-btn')

scaleButton.addEventListener('click', () => {
    window.electronAPI.callScale()
});

fingerPrintButton.addEventListener('click', () => {
    window.electronAPI.callFingerprintReader()
});

printerButton.addEventListener('click', () => {
    window.electronAPI.callPrinter()
});
