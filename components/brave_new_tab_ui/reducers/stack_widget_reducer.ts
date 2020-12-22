/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { Reducer } from 'redux'
import { types } from '../constants/stack_widget_types'



const setForegroundStackWidget = (widget: NewTab.StackWidget, state: NewTab.State): NewTab.State => {
  return state
}

const handleWidgetPrefsChange = (state: NewTab.State, oldState: NewTab.State): NewTab.State => {

  return state
}

const stackWidgetReducer: Reducer<NewTab.State | undefined> = (state: NewTab.State, action) => {
  const payload = action.payload

  switch (action.type) {
    case types.SET_FOREGROUND_STACK_WIDGET:
      state = setForegroundStackWidget(payload.widget as NewTab.StackWidget, state)
      break

    default:
      break
  }

  return state
}

export {
  stackWidgetReducer,
  handleWidgetPrefsChange
}
