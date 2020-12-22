// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

// Utils
import { debounce } from '../../common/debounce'

export const keyName = 'new-tab-data'

export const defaultState: NewTab.State = {
  initialDataLoaded: false,
  textDirection: window.loadTimeData.getString('textdirection'),
  showBackgroundImage: false,
  showStats: false,
  showClock: false,
  clockFormat: '',
  showTopSites: false,
  customLinksEnabled: false,
  isBrandedWallpaperNotificationDismissed: true,
  showEmptyPage: false,
  isIncognito: chrome.extension.inIncognitoContext,
  useAlternativePrivateSearchEngine: false,
  torCircuitEstablished: false,
  torInitProgress: '',
  isTor: false,
  isQwant: false,
  stats: {
    adsBlockedStat: 0,
    javascriptBlockedStat: 0,
    bandwidthSavedStat: 0,
    fingerprintingBlockedStat: 0
  },


}

if (chrome.extension.inIncognitoContext) {
  defaultState.isTor = window.loadTimeData.getBoolean('isTor')
  defaultState.isQwant = window.loadTimeData.getBoolean('isQwant')
}

// For users upgrading to the new list based widget stack state,
// a list in the current format will need to be generated based on their
// previous configuration.


export const migrateStackWidgetSettings = (state: NewTab.State) => {
  // Migrating to the new stack widget data format
  return state
}

// Ensure any new stack widgets introduced are put behind
// the others, and not re-added unecessarily if removed
// at one point.
export const addNewStackWidget = (state: NewTab.State) => {
  return state
}

// Replaces any stack widgets that were improperly removed
// as a result of https://github.com/brave/brave-browser/issues/10067
export const replaceStackWidgets = (state: NewTab.State) => {

  return state
}

const cleanData = (state: NewTab.State) => {
  // We need to disable linter as we defined in d.ts that this values are number,
  // but we need this check to covert from old version to a new one
  /* tslint:disable */

  return state
}

export const load = (): NewTab.State => {
  const data: string | null = window.localStorage.getItem(keyName)
  let state = defaultState
  let storedState

  if (data) {
    try {
      storedState = JSON.parse(data)
      // add defaults for non-peristant data
      state = {
        ...state,
        ...storedState
      }
    } catch (e) {
      console.error('[NewTabData] Could not parse local storage data: ', e)
    }
  }
  return cleanData(state)
}

export const debouncedSave = debounce<NewTab.State>((data: NewTab.State) => {

}, 50)
