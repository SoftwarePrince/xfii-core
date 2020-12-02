/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/sidebar/sidebar_item_view.h"

#include "brave/grit/brave_theme_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/canvas.h"

SidebarItemView::~SidebarItemView() = default;

void SidebarItemView::OnPaintBorder(gfx::Canvas* canvas) {
  ImageButton::OnPaintBorder(canvas);

  // Draw item highlight
  if (active_) {
    auto& bundle = ui::ResourceBundle::GetSharedInstance();
    canvas->DrawImageInt(
        *bundle.GetImageSkiaNamed(IDR_SIDEBAR_ITEM_HIGHLIGHT), 0, 0);
  }
}

void SidebarItemView::SetActive(bool active) {
  if (active_ == active)
    return;

  active_ = active;
  SchedulePaint();
}
