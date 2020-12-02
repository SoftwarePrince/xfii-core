/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_ITEM_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_ITEM_VIEW_H_

#include "brave/browser/ui/views/sidebar/sidebar_button_view.h"

class SidebarItemView : public SidebarButtonView {
 public:
  using SidebarButtonView::SidebarButtonView;
  ~SidebarItemView() override;

  SidebarItemView(const SidebarItemView&) = delete;
  SidebarItemView operator=(const SidebarItemView&) = delete;

  void SetActive(bool active);

  // views::ImageButton overrides:
  void OnPaintBorder(gfx::Canvas* canvas) override;

 private:
  bool active_ = false;
};

#endif  // BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_ITEM_VIEW_H_
