/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/sidebar/sidebar_model_data.h"

#include "chrome/browser/profiles/profile.h"
#include "content/public/browser/web_contents.h"

namespace sidebar {

SidebarModelData::SidebarModelData(const SidebarItem& sidebar_item,
                                   Profile* profile)
    : profile_(profile),
      sidebar_item_(sidebar_item) {}

SidebarModelData::~SidebarModelData() = default;

content::WebContents* SidebarModelData::GetWebContents() {
  if (!contents_) {
    content::WebContents::CreateParams params(profile_);
    contents_ = content::WebContents::Create(params);
  }

  return contents_.get();
}

bool SidebarModelData::IsBuiltInType() const {
  return sidebar_item_.type == SidebarItem::Type::kTypeBuiltIn;
}

GURL SidebarModelData::GetURL() const {
  return sidebar_item_.url;
}

bool SidebarModelData::OpenInPanel() const {
  return sidebar_item_.open_in_panel;
}

}  // namespace sidebar
