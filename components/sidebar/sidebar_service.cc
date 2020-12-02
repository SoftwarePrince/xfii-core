/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/sidebar/sidebar_service.h"

#include "base/feature_list.h"
#include "base/values.h"
#include "brave/components/sidebar/features.h"
#include "brave/components/sidebar/pref_names.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"

namespace sidebar {

namespace {

std::vector<SidebarItem> GetDefaultSidebarItems() {
  std::vector<SidebarItem> items;
  items.push_back(SidebarItem::Create(GURL("https://together.brave.com/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      true));
  items.push_back(SidebarItem::Create(GURL("brave://wallet/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      false));
  items.push_back(SidebarItem::Create(GURL("brave://bookmarks/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      true));
  items.push_back(SidebarItem::Create(GURL("brave://history/"),
                                      base::string16(),
                                      SidebarItem::Type::kTypeBuiltIn,
                                      true));
    return items;
}

}  // namespace

// static void
void SidebarService::RegisterPrefs(PrefRegistrySimple* registry) {
  if (!base::FeatureList::IsEnabled(kSidebarFeature))
    return;

  registry->RegisterListPref(kSidebarItems);
}

SidebarService::SidebarService(PrefService* prefs) : prefs_(prefs) {
  DCHECK(prefs_);
  LoadSidebarItems();
}

SidebarService::~SidebarService() = default;

void SidebarService::AddItem(const SidebarItem& item) {
  items_.push_back(item);

  for (Observer& obs : observers_) {
    // Index starts at zero.
    obs.OnItemAdded(item, items_.size() - 1);
  }
}

void SidebarService::RemoveItem(const SidebarItem& item) {
}

void SidebarService::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void SidebarService::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void SidebarService::LoadSidebarItems() {
  auto* items_value = prefs_->Get(kSidebarItems);
  if (items_value->GetList().empty()) {
    items_ = GetDefaultSidebarItems();
  }
}

}  // namespace sidebar
