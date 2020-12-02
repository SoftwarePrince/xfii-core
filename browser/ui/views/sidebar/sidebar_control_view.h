/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_CONTROL_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_CONTROL_VIEW_H_

#include "ui/views/view.h"

class BraveBrowser;
class SidebarButtonView;
class SidebarItemsContainerView;

namespace views {
class BoxLayout;
}  // namespace views

// This view includes all sidebar buttons such as sidebar item buttons, add and
// settings button.
class SidebarControlView : public views::View {
 public:
  explicit SidebarControlView(BraveBrowser* browser);
  ~SidebarControlView() override;

  SidebarControlView(const SidebarControlView&) = delete;
  SidebarControlView& operator=(const SidebarControlView&) = delete;

 private:
  void AddChildViews();

  void OnButtonPressed(views::View* view);
  // Add button is disabled when all builtin items are enabled and current tab
  // is NTP.
  void UpdateAddButtonEnabledState();

  BraveBrowser* browser_ = nullptr;
  SidebarItemsContainerView* sidebar_items_view_ = nullptr;
  SidebarButtonView* sidebar_item_add_view_ = nullptr;
  views::BoxLayout* box_layout_ = nullptr;
};

#endif // BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_CONTROL_VIEW_H_
