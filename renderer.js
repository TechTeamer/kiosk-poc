const scaleButton = document.getElementById('scale-btn')
const fingerPrintButton = document.getElementById('fingerprint-btn')
const printerButton = document.getElementById('printer-btn')

const scaleContainer = document.getElementById('scaleContainer')
const scaleStatus = scaleContainer.querySelector('#status')
const scaleWeight = scaleContainer.querySelector('#weight')
const scaleSystem = scaleContainer.querySelector('.system')
const scaleFinalWeight = scaleContainer.querySelector('#final_weight')
const scaleMessages = scaleContainer.querySelector('#messages')

function handleScaleMessages(event, msg) {
    scaleMessages.innerText = msg
    scaleStatus.innerText = event
    scaleMessages.classList.remove('hidden')
}

function handleScaleUpdates(data) {
    scaleMessages.classList.add('hidden')

    scaleWeight.innerText = data.value
    scaleSystem.innerText = data.system
}

scaleButton.addEventListener('click', () => {
    scaleContainer.classList.remove('hidden')

    window.electronAPI.callScale()
    window.electronAPI.onScaleUpdate((event, data) => {
        switch(data.message) {
            case 'weight-change':
                // scaleWeight.innerText = data.data.value
                handleScaleUpdates(data.data)
                break
            case 'overweight-change':
                handleScaleMessages('online', 'Túlsúly!')
                break
            case 'end':
                handleScaleMessages('offline', 'Nincs aktív mérleg')
                break
            case 'error':
                handleScaleMessages('offline', 'Nincs aktív mérleg')
                break
            default:
                break
        }
    })
});

fingerPrintButton.addEventListener('click', () => {
    window.electronAPI.callFingerprintReader()
});

printerButton.addEventListener('click', () => {
    window.electronAPI.callPrinter()
});
