// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'

// Components
import Stats from './stats'
import TopSitesGrid from './gridSites'
import FooterInfo from './footerInfo'
import SiteRemovalNotification from './notification'
import {
  ClockWidget as Clock
} from '../../components/default'
import * as Page from '../../components/default/page'
import BrandedWallpaperLogo from '../../components/default/brandedWallpaper/logo'
import { brandedWallpaperLogoClicked } from '../../api/brandedWallpaper'

// Helpers
import VisibilityTimer from '../../helpers/visibilityTimer'


// Types
import { getLocale } from '../../../common/locale'
import { NewTabActions } from '../../constants/new_tab_types'

// NTP features
import Settings, { TabType as SettingsTabType } from './settings'

interface Props {
  newTabData: NewTab.State
  gridSitesData: NewTab.GridSitesState
  actions: NewTabActions
  saveShowBackgroundImage: (value: boolean) => void
  saveShowStats: (value: boolean) => void
  saveSetAllStackWidgets: (value: boolean) => void
  //saveShowRewards: (value: boolean) => void
}

interface State {
  onlyAnonWallet: boolean
  showSettingsMenu: boolean
  backgroundHasLoaded: boolean
  activeSettingsTab: SettingsTabType | null
}

function GetBackgroundImageSrc(props: Props) {
  if (!props.newTabData.showBackgroundImage &&
    (!props.newTabData.brandedWallpaperData || props.newTabData.brandedWallpaperData.isSponsored)) {
    return undefined
  }
  if (props.newTabData.brandedWallpaperData) {
    const wallpaperData = props.newTabData.brandedWallpaperData
    if (wallpaperData && wallpaperData.wallpaperImageUrl) {
      return wallpaperData.wallpaperImageUrl
    }
  }
  if (props.newTabData.backgroundImage && props.newTabData.backgroundImage.source) {
    return props.newTabData.backgroundImage.source
  }
  return undefined
}

function GetIsShowingBrandedWallpaper(props: Props) {
  const { newTabData } = props
  return (newTabData.brandedWallpaperData &&
    newTabData.brandedWallpaperData.isSponsored) ? true : false
}

function GetShouldShowBrandedWallpaperNotification(props: Props) {
  return GetIsShowingBrandedWallpaper(props) &&
    !props.newTabData.isBrandedWallpaperNotificationDismissed
}

class NewTabPage extends React.Component<Props, State> {
  state: State = {
    onlyAnonWallet: false,
    showSettingsMenu: false,
    backgroundHasLoaded: false,
    activeSettingsTab: null
  }
  imageSource?: string = undefined
  timerIdForBrandedWallpaperNotification?: number = undefined
  onVisiblityTimerExpired = () => {
    this.dismissBrandedWallpaperNotification(false)
  }
  visibilityTimer = new VisibilityTimer(this.onVisiblityTimerExpired, 4000)

  componentDidMount() {
    // if a notification is open at component mounting time, close it
    this.props.actions.showTilesRemovedNotice(false)
    this.imageSource = GetBackgroundImageSrc(this.props)
    this.trackCachedImage()
    if (GetShouldShowBrandedWallpaperNotification(this.props)) {
      this.trackBrandedWallpaperNotificationAutoDismiss()
    }
    this.checkShouldOpenSettings()
  }

  componentDidUpdate(prevProps: Props) {
    const oldImageSource = GetBackgroundImageSrc(prevProps)
    const newImageSource = GetBackgroundImageSrc(this.props)
    this.imageSource = newImageSource
    if (newImageSource && oldImageSource !== newImageSource) {
      this.trackCachedImage()
    }
    if (oldImageSource &&
      !newImageSource) {
      // reset loaded state
      this.setState({ backgroundHasLoaded: false })
    }
    if (!GetShouldShowBrandedWallpaperNotification(prevProps) &&
      GetShouldShowBrandedWallpaperNotification(this.props)) {
      this.trackBrandedWallpaperNotificationAutoDismiss()
    }

    if (GetShouldShowBrandedWallpaperNotification(prevProps) &&
      !GetShouldShowBrandedWallpaperNotification(this.props)) {
      this.stopWaitingForBrandedWallpaperNotificationAutoDismiss()
    }
  }

  trackCachedImage() {
    if (this.state.backgroundHasLoaded) {
      this.setState({ backgroundHasLoaded: false })
    }
    if (this.imageSource) {
      const imgCache = new Image()
      imgCache.src = this.imageSource
      console.timeStamp('image start loading...')
      imgCache.onload = () => {
        console.timeStamp('image loaded')
        this.setState({
          backgroundHasLoaded: true
        })
      }
    }
  }

  trackBrandedWallpaperNotificationAutoDismiss() {
    // Wait until page has been visible for an uninterupted Y seconds and then
    // dismiss the notification.
    this.visibilityTimer.startTracking()
  }

  checkShouldOpenSettings() {
    const params = window.location.search
    const urlParams = new URLSearchParams(params)
    const openSettings = urlParams.get('openSettings')

    if (openSettings) {
      this.setState({ showSettingsMenu: true })
      // Remove settings param so menu doesn't persist on reload
      window.history.pushState(null, '', '/')
    }
  }

  stopWaitingForBrandedWallpaperNotificationAutoDismiss() {
    this.visibilityTimer.stopTracking()
  }

  toggleShowBackgroundImage = () => {
    this.props.saveShowBackgroundImage(
      !this.props.newTabData.showBackgroundImage
    )
  }

  toggleShowClock = () => {
    this.props.actions.clockWidgetUpdated(
      !this.props.newTabData.showClock,
      this.props.newTabData.clockFormat)
  }

  toggleClockFormat = () => {
    const currentFormat = this.props.newTabData.clockFormat
    let newFormat
    // cycle through the available options
    switch (currentFormat) {
      case '': newFormat = '12'; break
      case '12': newFormat = '24'; break
      case '24': newFormat = ''; break
      default: newFormat = ''; break
    }
    this.props.actions.clockWidgetUpdated(
      this.props.newTabData.showClock,
      newFormat)
  }

  toggleShowStats = () => {
    this.props.saveShowStats(
      !this.props.newTabData.showStats
    )
  }


  toggleShowTopSites = () => {
    const { showTopSites, customLinksEnabled } = this.props.newTabData
    this.props.actions.setMostVisitedSettings(!showTopSites, customLinksEnabled)
  }

  toggleCustomLinksEnabled = () => {
    const { showTopSites, customLinksEnabled } = this.props.newTabData
    this.props.actions.setMostVisitedSettings(showTopSites, !customLinksEnabled)
  }
  /*
    toggleShowRewards = () => {
      this.props.saveShowRewards(!this.props.newTabData.showRewards)
    }
  
  toggleShowTogether = () => {
    this.props.saveShowTogether(!this.props.newTabData.showTogether)
  }

  toggleShowBinance = () => {

  }

  toggleShowGemini = () => {

  }

  toggleShowBitcoinDotCom = () => {
    this.props.saveShowBitcoinDotCom(!this.props.newTabData.showBitcoinDotCom)
  }

  toggleShowCryptoDotCom = () => {
    this.props.saveShowCryptoDotCom(!this.props.newTabData.showCryptoDotCom)
  }

  onBinanceClientUrl = (clientUrl: string) => {
    this.props.actions.onBinanceClientUrl(clientUrl)
  }

  onGeminiClientUrl = (clientUrl: string) => {
    this.props.actions.onGeminiClientUrl(clientUrl)
  }

  onValidBinanceAuthCode = () => {
    this.props.actions.onValidBinanceAuthCode()
  }

  onValidGeminiAuthCode = () => {
    this.props.actions.onValidGeminiAuthCode()
  }

  setBinanceHideBalance = (hide: boolean) => {
    this.props.actions.setBinanceHideBalance(hide)
  }

  setGeminiHideBalance = (hide: boolean) => {
    this.props.actions.setGeminiHideBalance(hide)
  }

  disconnectBinance = () => {
    this.props.actions.disconnectBinance()
  }

  setBinanceDisconnectInProgress = () => {
    this.props.actions.setBinanceDisconnectInProgress(true)
  }

  cancelBinanceDisconnect = () => {
    this.props.actions.setBinanceDisconnectInProgress(false)
  }

  disconnectGemini = () => {
    this.props.actions.disconnectGemini()
  }

  setGeminiDisconnectInProgress = () => {
    this.props.actions.setGeminiDisconnectInProgress(true)
  }

  cancelGeminiDisconnect = () => {
    this.props.actions.setGeminiDisconnectInProgress(false)
  }

  connectBinance = () => {
    this.props.actions.connectToBinance()
  }

  connectGemini = () => {
    this.props.actions.connectToGemini()
  }

  buyCrypto = (coin: string, amount: string, fiat: string) => {
    const { userTLD } = this.props.newTabData.binanceState
    const refCode = userTLD === 'us' ? '35089877' : '39346846'
    const refParams = `ref=${refCode}&utm_source=brave`

    if (userTLD === 'us') {
      window.open(`https://www.binance.us/en/buy-sell-crypto?crypto=${coin}&amount=${amount}&${refParams}`, '_blank', 'noopener')
    } else {
      window.open(`https://www.binance.com/en/buy-sell-crypto?fiat=${fiat}&crypto=${coin}&amount=${amount}&${refParams}`, '_blank', 'noopener')
    }
  }

  onBuyBitcoinDotComCrypto = () => {
    this.props.actions.buyBitcoinDotComCrypto()
  }

  onInteractionBitcoinDotCom = () => {
    this.props.actions.interactionBitcoinDotCom()
  }

  onBinanceUserTLD = (userTLD: NewTab.BinanceTLD) => {
    this.props.actions.onBinanceUserTLD(userTLD)
  }

  setBalanceInfo = (info: Record<string, Record<string, string>>) => {
    this.props.actions.onAssetsBalanceInfo(info)
  }

  setAssetDepositInfo = (symbol: string, address: string, url: string) => {
    this.props.actions.onAssetDepositInfo(symbol, address, url)
  }

    startRewards = () => {
      chrome.braveRewards.saveAdsSetting('adsEnabled', 'true')
      chrome.braveRewards.setAutoContributeEnabled(true)
    }
  
  disableBrandedWallpaper = () => {
    this.props.saveBrandedWallpaperOptIn(false)
  }

  toggleShowBrandedWallpaper = () => {
    this.props.saveBrandedWallpaperOptIn(
      !this.props.newTabData.brandedWallpaperOptIn
    )
  }
  */
  dismissBrandedWallpaperNotification = (isUserAction: boolean) => {
    this.props.actions.dismissBrandedWallpaperNotification(isUserAction)
  }

  closeSettings = () => {
    this.setState({
      showSettingsMenu: false,
      activeSettingsTab: null
    })
  }

  openSettings = (activeTab?: SettingsTabType) => {
    this.props.actions.customizeClicked()
    this.setState({
      showSettingsMenu: !this.state.showSettingsMenu,
      activeSettingsTab: activeTab || null
    })
  }

  onClickLogo = () => {
    brandedWallpaperLogoClicked(this.props.newTabData.brandedWallpaperData)
  }



  setForegroundStackWidget = (widget: NewTab.StackWidget) => {
    this.props.actions.setForegroundStackWidget(widget)
  }
/*
  setInitialAmount = (amount: string) => {
    this.props.actions.setInitialAmount(amount)
  }

  setInitialFiat = (fiat: string) => {
    this.props.actions.setInitialFiat(fiat)
  }

  setInitialAsset = (asset: string) => {
    this.props.actions.setInitialAsset(asset)
  }

  setUserTLDAutoSet = () => {
    this.props.actions.setUserTLDAutoSet()
  }

  onBraveTodayInteracting = (isInteracting: boolean) => {
    if (isInteracting && !this.hasInitBraveToday) {
      this.hasInitBraveToday = true
      this.props.actions.today.interactionBegin()
    }
  }
  
    learnMoreRewards = () => {
      window.open('https://brave.com/brave-rewards/', '_blank', 'noopener')
    }
  
  learnMoreBinance = () => [
    window.open('https://brave.com/binance/', '_blank', 'noopener')
  ]

  setAssetDepositQRCodeSrc = (asset: string, src: string) => {
    this.props.actions.onDepositQRForAsset(asset, src)
  }

  setGeminiAssetDepositQRCodeSrc = (asset: string, src: string) => {
    this.props.actions.onGeminiDepositQRForAsset(asset, src)
  }
  setConvertableAssets = (asset: string, assets: string[]) => {
    this.props.actions.onConvertableAssets(asset, assets)
  }

  setBinanceSelectedView = (view: string) => {
    this.props.actions.setBinanceSelectedView(view)
  }

  setGeminiSelectedView = (view: string) => {
    this.props.actions.setGeminiSelectedView(view)
  }

  setGeminiAuthInvalid = () => {
    this.props.actions.setGeminiAuthInvalid(true)
    this.props.actions.disconnectGemini()
  }

  binanceUpdateActions = () => {
    this.fetchBalance()
    this.getConvertAssets()
  }

  binanceRefreshActions = () => {
    this.fetchBalance()
    this.setDepositInfo()
    this.getConvertAssets()
  }

  geminiUpdateActions = () => {
    this.fetchGeminiTickerPrices()
    this.fetchGeminiBalances()
    this.fetchGeminiDepositInfo()
  }

  fetchGeminiTickerPrices = () => {
    geminiData.currencies.map((asset: string) => {
      chrome.gemini.getTickerPrice(`${asset}usd`, (price: string) => {
        this.props.actions.setGeminiTickerPrice(asset, price)
      })
    })
  }

  onCryptoDotComMarketsRequested = async (assets: string[]) => {
    const [tickerPrices, losersGainers] = await Promise.all([
      fetchCryptoDotComTickerPrices(assets),
      fetchCryptoDotComLosersGainers()
    ])
    this.props.actions.cryptoDotComMarketDataUpdate(tickerPrices, losersGainers)
  }

  onCryptoDotComAssetData = async (assets: string[]) => {
    const [charts, pairs] = await Promise.all([
      fetchCryptoDotComCharts(assets),
      fetchCryptoDotComSupportedPairs()
    ])
    this.props.actions.setCryptoDotComAssetData(charts, pairs)
  }

  cryptoDotComUpdateActions = async () => {
    const { supportedPairs, tickerPrices: prices } = this.props.newTabData.cryptoDotComState
    const assets = Object.keys(prices)
    const supportedPairsSet = Object.keys(supportedPairs).length

    const [tickerPrices, losersGainers, charts] = await Promise.all([
      fetchCryptoDotComTickerPrices(assets),
      fetchCryptoDotComLosersGainers(),
      fetchCryptoDotComCharts(assets)
    ])

    // These are rarely updated, so we only need to fetch them
    // in the refresh interval if they aren't set yet (perhaps due to no connection)
    if (!supportedPairsSet) {
      const pairs = await fetchCryptoDotComSupportedPairs()
      this.props.actions.setCryptoDotComSupportedPairs(pairs)
    }

    this.props.actions.onCryptoDotComRefreshData(tickerPrices, losersGainers, charts)
  }

  onBtcPriceOptIn = async () => {
    this.props.actions.onBtcPriceOptIn()
    this.props.actions.onCryptoDotComInteraction()
    await this.onCryptoDotComMarketsRequested(['BTC'])
  }

  onCryptoDotComBuyCrypto = () => {
    this.props.actions.onCryptoDotComBuyCrypto()
  }

  onCryptoDotComInteraction = () => {
    this.props.actions.onCryptoDotComInteraction()
  }

  onCryptoDotComOptInMarkets = (show: boolean) => {
    this.props.actions.onCryptoDotComOptInMarkets(show)
  }

  fetchGeminiBalances = () => {
    chrome.gemini.getAccountBalances((balances: Record<string, string>, authInvalid: boolean) => {
      if (authInvalid) {
        chrome.gemini.refreshAccessToken((success: boolean) => {
          if (!success) {
            this.setGeminiAuthInvalid()
          }
        })
        return
      }

      this.props.actions.setGeminiAccountBalances(balances)
    })
  }

  fetchGeminiDepositInfo = () => {
    geminiData.currencies.map((asset: string) => {
      chrome.gemini.getDepositInfo(`${asset.toLowerCase()}`, (address: string) => {
        if (!address) {
          return
        }

        this.props.actions.setGeminiAssetAddress(asset, address)
        void generateQRData(address, asset, this.setGeminiAssetDepositQRCodeSrc)
      })
    })
  }

  getCurrencyList = () => {
    const { accountBalances, userTLD } = this.props.newTabData.binanceState
    const { usCurrencies, comCurrencies } = currencyData
    const baseList = userTLD === 'us' ? usCurrencies : comCurrencies

    if (!accountBalances) {
      return baseList
    }

    const accounts = Object.keys(accountBalances)
    const nonHoldingList = baseList.filter((symbol: string) => {
      return !accounts.includes(symbol)
    })

    return accounts.concat(nonHoldingList)
  }

  getConvertAssets = () => {
    chrome.binance.getConvertAssets((assets: any) => {
      for (let asset in assets) {
        if (assets[asset]) {
          this.setConvertableAssets(asset, assets[asset])
        }
      }
    })
  }

  fetchBalance = () => {
    const { depositInfoSaved } = this.props.newTabData.binanceState

    chrome.binance.getAccountBalances((balances: Record<string, Record<string, string>>, success: boolean) => {
      const hasBalances = Object.keys(balances).length

      if (!hasBalances) {
        return
      } else if (!success) {
        this.setAuthInvalid()
        return
      }

      this.setBalanceInfo(balances)

      if (!depositInfoSaved) {
        this.setDepositInfo()
      }
    })
  }

  setDepositInfo = () => {
    chrome.binance.getCoinNetworks((networks: Record<string, string>) => {
      const currencies = this.getCurrencyList()
      for (let ticker in networks) {
        if (currencies.includes(ticker)) {
          chrome.binance.getDepositInfo(ticker, networks[ticker], async (address: string, tag: string) => {
            this.setAssetDepositInfo(ticker, address, tag)
          })
        }
      }
      if (Object.keys(networks).length) {
        this.props.actions.setDepositInfoSaved()
      }
    })
  }

  setAuthInvalid = () => {
    this.props.actions.setAuthInvalid(true)
    this.props.actions.disconnectBinance()
  }

  dismissAuthInvalid = () => {
    this.props.actions.setAuthInvalid(false)
  }

  dismissGeminiAuthInvalid = () => {
    this.props.actions.setGeminiAuthInvalid(false)
  }

  getCryptoContent() {
    return null
  }

  allWidgetsHidden = () => {
    return null
  }

  toggleAllCards = (show: boolean) => {
    return null
  }

  renderCryptoContent() {
return null
  }
*/
  renderTogetherWidget(showContent: boolean, position: number) {

  }

  renderBinanceWidget(showContent: boolean, position: number) {
   
  }

  renderGeminiWidget(showContent: boolean, position: number) {
  
  }

  renderBitcoinDotComWidget(showContent: boolean, position: number) {
 
  }

  renderCryptoDotComWidget(showContent: boolean, position: number) {
  
  }

  render() {
    const { newTabData, gridSitesData, actions } = this.props
    const { showSettingsMenu } = this.state

    if (!newTabData) {
      return null
    }

    const hasImage = this.imageSource !== undefined
    const isShowingBrandedWallpaper = newTabData.brandedWallpaperData ? true : false
    const showTopSites = !!this.props.gridSitesData.gridSites.length && newTabData.showTopSites

    return (
      <Page.App
        dataIsReady={newTabData.initialDataLoaded}
        hasImage={hasImage}
        imageSrc={this.imageSource}
        imageHasLoaded={this.state.backgroundHasLoaded}
      >
        <Page.Page
          hasImage={hasImage}
          imageSrc={this.imageSource}
          imageHasLoaded={this.state.backgroundHasLoaded}
          showClock={newTabData.showClock}
          showStats={newTabData.showStats}
          showTopSites={showTopSites}
          showBrandedWallpaper={isShowingBrandedWallpaper}
        >
          {newTabData.showStats &&
            <Page.GridItemStats>
              <Stats
                paddingType={'right'}
                widgetTitle={getLocale('statsTitle')}
                textDirection={newTabData.textDirection}
                stats={newTabData.stats}
                hideWidget={this.toggleShowStats}
                menuPosition={'right'}
              />
            </Page.GridItemStats>
          }
          {newTabData.showClock &&
            <Page.GridItemClock>
              <Clock
                paddingType={'right'}
                widgetTitle={getLocale('clockTitle')}
                textDirection={newTabData.textDirection}
                hideWidget={this.toggleShowClock}
                menuPosition={'left'}
                toggleClickFormat={this.toggleClockFormat}
                clockFormat={newTabData.clockFormat}
              />
            </Page.GridItemClock>
          }
          {
            showTopSites
              ? (
                <Page.GridItemTopSites>
                  <TopSitesGrid
                    actions={actions}
                    paddingType={'right'}
                    customLinksEnabled={newTabData.customLinksEnabled}
                    widgetTitle={getLocale('topSitesTitle')}
                    gridSites={gridSitesData.gridSites}
                    menuPosition={'right'}
                    hideWidget={this.toggleShowTopSites}
                    textDirection={newTabData.textDirection}
                  />
                </Page.GridItemTopSites>
              ) : null
          }
          {
            gridSitesData.shouldShowSiteRemovedNotification
              ? (
                <Page.GridItemNotification>
                  <SiteRemovalNotification actions={actions} />
                </Page.GridItemNotification>
              ) : null
          }
          <Page.Footer>
            <Page.FooterContent>
              {isShowingBrandedWallpaper && newTabData.brandedWallpaperData &&
                newTabData.brandedWallpaperData.logo &&
                <Page.GridItemBrandedLogo>
                  <BrandedWallpaperLogo
                    menuPosition={'right'}
                    paddingType={'default'}
                    textDirection={newTabData.textDirection}
                    onClickLogo={this.onClickLogo}
                    data={newTabData.brandedWallpaperData.logo}
                  />
                </Page.GridItemBrandedLogo>}
              <FooterInfo
                textDirection={newTabData.textDirection}
                onClickSettings={this.openSettings}
                backgroundImageInfo={newTabData.backgroundImage}
                showPhotoInfo={!isShowingBrandedWallpaper && newTabData.showBackgroundImage}
              />
            </Page.FooterContent>
          </Page.Footer>
        
        </Page.Page>
  
        <Settings
          actions={actions}
          textDirection={newTabData.textDirection}
          showSettingsMenu={showSettingsMenu}
          onClose={this.closeSettings}
          setActiveTab={this.state.activeSettingsTab || undefined}
          toggleShowBackgroundImage={this.toggleShowBackgroundImage}
          toggleShowClock={this.toggleShowClock}
          toggleShowStats={this.toggleShowStats}
          toggleShowTopSites={this.toggleShowTopSites}
          toggleCustomLinksEnabled={this.toggleCustomLinksEnabled}
          showBackgroundImage={newTabData.showBackgroundImage}
          showClock={newTabData.showClock}
          clockFormat={newTabData.clockFormat}
          showStats={newTabData.showStats}
          showTopSites={newTabData.showTopSites}
          customLinksEnabled={newTabData.customLinksEnabled}


        />
      </Page.App>
    )
  }
}

export default NewTabPage
