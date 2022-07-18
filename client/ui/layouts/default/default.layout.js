const pageSelectors = document.getElementsByClassName('page-selector')

for( let i = 0; i < pageSelectors.length; i++) {
    pageSelectors[i].addEventListener('click', () => {
        window.electronAPI.changePage(pageSelectors[i].dataset.page)
    })
}
