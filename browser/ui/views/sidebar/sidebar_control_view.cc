/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/sidebar/sidebar_control_view.h"

#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/sidebar/sidebar_controller.h"
#include "brave/browser/ui/views/sidebar/sidebar_button_view.h"
#include "brave/browser/ui/views/sidebar/sidebar_items_container_view.h"
#include "brave/grit/brave_theme_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/layout/box_layout.h"

SidebarControlView::SidebarControlView(BraveBrowser* browser)
    : browser_(browser) {
  constexpr int kBorderThickness = 1;
  // TODO(simonhong): Get color via theme properties.
  // We can handle dark/light theme color with theme properties.
  constexpr SkColor kBackgroundColor = SkColorSetRGB(0xF3, 0xF3, 0xF5);
  constexpr SkColor kBorderColor = SkColorSetRGB(0xD9, 0xDC, 0xDF);
  SetBackground(views::CreateSolidBackground(kBackgroundColor));
  SetBorder(
      views::CreateSolidSidedBorder(0, 0, 0, kBorderThickness, kBorderColor));
  box_layout_ = SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));
  AddChildViews();
  UpdateAddButtonEnabledState();
}

SidebarControlView::~SidebarControlView() = default;

void SidebarControlView::AddChildViews() {
  sidebar_items_view_ =
      AddChildView(std::make_unique<SidebarItemsContainerView>(browser_));
  sidebar_item_add_view_ = AddChildView(std::make_unique<SidebarButtonView>());
  auto& bundle = ui::ResourceBundle::GetSharedInstance();
  sidebar_item_add_view_->SetImage(
      views::Button::STATE_NORMAL,
      bundle.GetImageSkiaNamed(IDR_SIDEBAR_ITEM_ADD_FOCUSED));
  sidebar_item_add_view_->SetCallback(
      base::BindRepeating(
          &SidebarControlView::OnButtonPressed,
          base::Unretained(this),
          sidebar_item_add_view_));
  // This spacer view occupies the all empty space between add button and
  // settings button.
  auto* spacer = AddChildView(std::make_unique<views::View>());
  box_layout_->SetFlexForView(spacer, 1);
}

void SidebarControlView::OnButtonPressed(views::View* view) {
  if (view == sidebar_item_add_view_) {
    // TODO(simonhong): Call via bubble dialog menu.
    browser_->sidebar_controller()->AddItemWithCurrentTab();
  }
}

void SidebarControlView::UpdateAddButtonEnabledState() {
  // Calling sidebar_item_add_view->SetEnabled() based on current items and
  // active tab. And this method should be called whenever item is added/removed
  // and active tab is changed.
}
