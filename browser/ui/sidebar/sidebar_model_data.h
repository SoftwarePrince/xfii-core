/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_SIDEBAR_SIDEBAR_MODEL_DATA_H_
#define BRAVE_BROWSER_UI_SIDEBAR_SIDEBAR_MODEL_DATA_H_

#include <memory>

#include "brave/components/sidebar/sidebar_item.h"

class Profile;

namespace content {
class WebContents;
}  // namespace content

namespace sidebar {

// SidebarModelData represents sidebar item's runtime state.
class SidebarModelData {
 public:
  SidebarModelData(const SidebarItem& sidebar_item, Profile* profile);
  virtual ~SidebarModelData();

  SidebarModelData(const SidebarModelData&) = delete;
  SidebarModelData& operator=(const SidebarModelData&) = delete;

  content::WebContents* GetWebContents();
  bool IsBuiltInType() const;
  GURL GetURL() const;
  bool OpenInPanel() const;

  void set_loaded(bool loaded) { loaded_ = loaded; }
  bool loaded() const { return loaded_; }

 private:
  Profile* profile_ = nullptr;
  SidebarItem sidebar_item_;
  bool loaded_ = false;
  // Each sidebar item uses its own WebContents.
  std::unique_ptr<content::WebContents> contents_;
};

}  // namespace sidebar

#endif  // BRAVE_BROWSER_UI_SIDEBAR_SIDEBAR_MODEL_DATA_H_
