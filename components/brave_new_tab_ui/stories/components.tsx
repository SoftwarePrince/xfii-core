// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// you can obtain one at http://mozilla.org/MPL/2.0/.

import * as React from 'react'
import { storiesOf } from '@storybook/react'
import { withKnobs, select, text } from '@storybook/addon-knobs/react'
import Theme from 'brave-ui/theme/brave-default'
import DarkTheme from 'brave-ui/theme/brave-dark'
import BraveCoreThemeProvider from '../../common/BraveCoreThemeProvider'

// Components
import Loading from '../components/loading'

function ThemeProvider ({ story }: any) {
  const themeName = select(
    'Theme',
    { ['Light']: 'Light', ['Dark']: 'Dark' },
    'Light'
  )
  console.log('theme', themeName)
  return (
    <BraveCoreThemeProvider
      dark={DarkTheme}
      light={Theme}
      initialThemeType={themeName as chrome.braveTheme.ThemeType}
    >
    {story}
    </BraveCoreThemeProvider>
  )
}

storiesOf('New Tab', module)
  .addDecorator(withKnobs)
  .addDecorator(story => <ThemeProvider story={story()} />)
  .add('Loading', () => (
    <div
      style={{ width: '500px', height: '500px', display: 'flex', alignItems: 'center', justifyContent: 'center' }}
    >
      <Loading />
    </div>
  ))

