/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/sidebar/sidebar_controller.h"

#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/sidebar/sidebar_model.h"
#include "brave/browser/ui/sidebar/sidebar_model_data.h"
#include "brave/browser/ui/sidebar/sidebar_service_factory.h"
#include "brave/components/sidebar/sidebar_service.h"
#include "chrome/browser/ui/browser_tabstrip.h"
#include "chrome/browser/ui/singleton_tabs.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"

namespace sidebar {

SidebarController::SidebarController(BraveBrowser* browser, Profile* profile)
    : browser_(browser),
      sidebar_model_(new SidebarModel(profile)) {}

SidebarController::~SidebarController() = default;

void SidebarController::ActivateItemAt(int index) {
  // -1 means there is no active item.
  DCHECK_GE(index, -1);
  if (index == -1) {
    sidebar_model_->SetActiveIndex(index);
    return;
  }

  auto* model_data = sidebar_model_->data()[index].get();
  if (model_data->OpenInPanel()) {
    sidebar_model_->SetActiveIndex(index);
    return;
  }

  // If an item targets in new tab, it should not be an active item.
  if (model_data->IsBuiltInType()) {
    // Should we also always open built-in type in new tab?
    ShowSingletonTab(browser_, model_data->GetURL());
  } else {
    chrome::AddTabAt(browser_, model_data->GetURL(), -1, true);
  }
}

void SidebarController::AddItemWithCurrentTab() {
  auto* active_contents = browser_->tab_strip_model()->GetActiveWebContents();
  if (!active_contents)
    return;

  const GURL url = active_contents->GetVisibleURL();
  const base::string16 title = active_contents->GetTitle();
  DVLOG(2) << __func__ << ": " << url << " , " << title;

  SidebarService* service =
      SidebarServiceFactory::GetForProfile(browser_->profile());
  service->AddItem(
      SidebarItem::Create(url, title, SidebarItem::Type::kTypeWeb, false));
}

}  // namespace sidebar
