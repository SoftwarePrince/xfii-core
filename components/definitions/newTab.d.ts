// Copyright (c) 2019 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

declare namespace NewTab {

  export type BrandedWallpaperLogo = {
    image: string
    companyName: string
    alt: string
    destinationUrl: string
  }

  export interface BrandedWallpaper {
    isSponsored: boolean
    wallpaperImageUrl: string
    creativeInstanceId: string
    wallpaperId: string
    logo: BrandedWallpaperLogo
  }
  export interface ApplicationState {
    newTabData: State | undefined
    gridSitesData: GridSitesState | undefined
  }

  export interface Image {
    name: string
    source: string
    author: string
    link: string
    originalUrl: string
    license: string
  }

  export interface Site {
    id: string
    url: string
    title: string
    favicon: string
    letter: string
    pinnedIndex: number | undefined
    defaultSRTopSite: boolean | undefined
  }

  export interface Stats {
    adsBlockedStat: number
    javascriptBlockedStat: number
    bandwidthSavedStat: number
    fingerprintingBlockedStat: number
  }

  export interface Bookmark {
    dateAdded: number
    id: string
    index: number
    parentId: string
    title: string
    url: string
  }

  export type StackWidget =  'binance' | 'together' | 'gemini' | 'bitcoinDotCom' | 'cryptoDotCom' | ''

  export interface GridSitesState {
    removedSites: Site[]
    gridSites: Site[]
    shouldShowSiteRemovedNotification: boolean
  }

  export interface PageState {
    showEmptyPage: boolean
  }


  export interface PersistentState {
    showEmptyPage: boolean
  }

  export interface EphemeralState {
    initialDataLoaded: boolean
    textDirection: string
    isIncognito: boolean
    useAlternativePrivateSearchEngine: boolean
    torCircuitEstablished: boolean,
    torInitProgress: string,
    isTor: boolean
    isQwant: boolean
    backgroundImage?: Image
    gridLayoutSize?: 'small'
    showGridSiteRemovedNotification?: boolean
    showBackgroundImage: boolean
    showStats: boolean
    showClock: boolean
    clockFormat: string
    showTopSites: boolean
    customLinksEnabled: boolean
    isBrandedWallpaperNotificationDismissed: boolean
    stats: Stats,
    brandedWallpaperData?: BrandedWallpaper
  }

 
  export interface BinanceWidgetState {
    userTLD: BinanceTLD
    initialFiat: string
    initialAmount: string
    initialAsset: string
    userTLDAutoSet: boolean
    accountBalances: Record<string, string>
    authInProgress: boolean
    assetBTCValues: Record<string, string>
    assetUSDValues: Record<string, string>
    assetBTCVolumes: Record<string, string>
    userAuthed: boolean
    btcBalanceValue: string
    hideBalance: boolean
    btcPrice: string
    btcVolume: string
    binanceClientUrl: string
    assetDepositInfo: Record<string, any>
    assetDepoitQRCodeSrcs: Record<string, string>
    convertAssets: Record<string, Record<string, string>[]>
    accountBTCValue: string
    accountBTCUSDValue: string
    disconnectInProgress: boolean
    authInvalid: boolean
    selectedView: string
    depositInfoSaved: boolean
  }

  export interface GeminiWidgetState {
    geminiClientUrl: string
    userAuthed: boolean
    authInProgress: boolean
    tickerPrices: Record<string, string>
    selectedView: string
    assetAddresses: Record<string, string>
    assetAddressQRCodes: Record<string, string>
    hideBalance: boolean
    accountBalances: Record<string, string>
    disconnectInProgress: boolean
    authInvalid: boolean
  }

  export interface CryptoDotComWidgetState {
    optInTotal: boolean
    optInBTCPrice: boolean
    optInMarkets: boolean
    tickerPrices: Record<string, any>
    losersGainers: Record<string, any>
    supportedPairs: Record<string, any>
    charts: Record<string, any>
  }

  export type BinanceTLD = 'us' | 'com'

  export enum PromotionTypes {
    UGP = 0,
    ADS = 1
  }

  export interface PromotionResponse {
    result: number
    promotions: Promotion[]
  }

  export interface Promotion {
    type: PromotionTypes
    promotionId: string
  }

  export interface DefaultSuperReferralTopSite {
    pinnedIndex: number
    url: string
    title: string
    favicon: string
  }

  // In-memory state is a superset of PersistentState
  export type State = PersistentState & EphemeralState
}
