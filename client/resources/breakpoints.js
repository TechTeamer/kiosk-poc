const breakpoints = module.exports = {}
const deviceSizes = require('./device-sizes.json')

// widescreen portrait
breakpoints.aspectOverWidescreenPortrait = 'only screen and (min-aspect-ratio: 90000/16000)'
breakpoints.aspectUnderWidescreenPortrait = 'only screen and (max-aspect-ratio: 89999/16000)'
// widescreen landscape
breakpoints.aspectOverWidescreenLandscape = 'only screen and (min-aspect-ratio: 16000/90000)'
breakpoints.aspectUnderWidescreenLandscape = 'only screen and (max-aspect-ratio: 15999/90000)'
// square
breakpoints.aspectOverSquare = 'only screen and (min-aspect-ratio: 1/1)'
breakpoints.aspectUnderSquare = 'only screen and (max-aspect-ratio: 1/1)'
// letterbox portrait
breakpoints.aspectOverLetterboxPortrait = 'only screen and (min-aspect-ratio: 30000/40000)'
breakpoints.aspectUnderLetterboxPortrait = 'only screen and (max-aspect-ratio: 29999/40000)'
// letterbox landscape
breakpoints.aspectOverLetterboxLandscape = 'only screen and (min-aspect-ratio: 40000/30000)'
breakpoints.aspectUnderLetterboxLandscape = 'only screen and (max-aspect-ratio: 39999/30000)'

// phone
const phonePortraitSize = deviceSizes.phonePortraitSize
const phoneLandscapeSize = deviceSizes.phoneLandscapeSize
// under phone
breakpoints.narrowerThanPhonePortrait = `only screen and (max-width: ${phonePortraitSize}px)`
breakpoints.narrowerThanPhoneLandscape = `only screen and (max-width: ${phoneLandscapeSize}px)`
breakpoints.shorterThanPhonePortrait = `only screen and (max-height: ${phonePortraitSize}px)`
breakpoints.shorterThanPhoneLandscape = `only screen and (max-height: ${phoneLandscapeSize}px)`
// above phone
breakpoints.widerThanPhonePortrait = `only screen and (min-width: ${phonePortraitSize + 1}px)`
breakpoints.widerThanPhoneLandscape = `only screen and (min-width: ${phoneLandscapeSize + 1}px)`
breakpoints.tallerThanPhonePortrait = `only screen and (min-height: ${phonePortraitSize + 1}px)`
breakpoints.tallerThanPhoneLandscape = `only screen and (min-height: ${phoneLandscapeSize + 1})px`

// tablet
const tabletPortraitSize = deviceSizes.tabletPortraitSize
const tabletLandscapeSize = deviceSizes.tabletLandscapeSize
// under tablet
breakpoints.narrowerThanTabletPortrait = `only screen and (max-width: ${tabletPortraitSize}px)`
breakpoints.narrowerThanTabletLandscape = `only screen and (max-width: ${tabletLandscapeSize}px)`
breakpoints.shorterThanTabletPortrait = `only screen and (max-height: ${tabletPortraitSize}px)`
breakpoints.shorterThanTabletLandscape = `only screen and (max-height: ${tabletLandscapeSize}px)`
// above tablet
breakpoints.widerThanTabletPortrait = `only screen and (min-width: ${tabletPortraitSize + 1}px)`
breakpoints.widerThanTabletLandscape = `only screen and (min-width: ${tabletLandscapeSize + 1}px)`
breakpoints.tallerThanTabletPortrait = `only screen and (min-height: ${tabletPortraitSize + 1}px)`
breakpoints.tallerThanTabletLandscape = `only screen and (min-height: ${tabletLandscapeSize + 1}px)`

// small-desktop
const smallDesktopWidth = deviceSizes.smallDesktopWidth
const smallDesktopHeight = deviceSizes.smallDesktopHeight
// under small-desktop
breakpoints.narrowerThanSmallDesktop = `only screen and (max-width: ${smallDesktopWidth}px)`
breakpoints.shorterThanSmallDesktop = `only screen and (max-height: ${smallDesktopHeight}px)`
// above small-desktop
breakpoints.widerThanSmallDesktop = `only screen and (min-width: ${smallDesktopWidth + 1}px)`
breakpoints.tallerThanSmallDesktop = `only screen and (min-height: ${smallDesktopHeight + 1}px)`

// medium-desktop
const mediumDesktopWidth = deviceSizes.mediumDesktopWidth
const mediumDesktopHeight = deviceSizes.mediumDesktopHeight
// under medium-desktop
breakpoints.narrowerThanMediumDesktop = `only screen and (max-width: ${mediumDesktopWidth}px)`
breakpoints.shorterThanMediumDesktop = `only screen and (max-height: ${mediumDesktopHeight}px)`
// above medium-desktop
breakpoints.widerThanMediumDesktop = `only screen and (min-width: ${mediumDesktopWidth + 1}px)`
breakpoints.tallerThanMediumDesktop = `only screen and (min-height: ${mediumDesktopHeight + 1}px)`

// hd-desktop
const hdDesktopWidth = deviceSizes.hdDesktopWidth
const hdDesktopHeight = deviceSizes.hdDesktopHeight
// under hd-desktop
breakpoints.narrowerThanHdDesktop = `only screen and (max-width: ${hdDesktopWidth}px)`
breakpoints.shorterThanHdDesktop = `only screen and (max-height: ${hdDesktopHeight}px)`
// above hd-desktop
breakpoints.widerThanHdDesktop = `only screen and (min-width: ${hdDesktopWidth + 1}px)`
breakpoints.tallerThanHdDesktop = `only screen and (min-height: ${hdDesktopHeight + 1}px)`

// only
breakpoints.onlyPhone = `only screen and (max-width: ${phoneLandscapeSize}px)`
breakpoints.onlyTablet = `only screen and (min-width: ${tabletPortraitSize}px) and (max-width: ${smallDesktopWidth - 1}px)`
breakpoints.onlySmallDesktop = `only screen and (min-width: ${smallDesktopWidth}px) and (max-width: ${mediumDesktopWidth - 1}px)`
breakpoints.onlyMediumDesktop = `only screen and (min-width: ${mediumDesktopWidth}px) and (max-width: ${hdDesktopWidth - 1}px)`
breakpoints.onlyHdDesktop = breakpoints.widerThanHdDesktop
