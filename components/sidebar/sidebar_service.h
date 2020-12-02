/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_SIDEBAR_SIDEBAR_SERVICE_H_
#define BRAVE_COMPONENTS_SIDEBAR_SIDEBAR_SERVICE_H_

#include <vector>

#include "base/observer_list.h"
#include "brave/components/sidebar/sidebar_item.h"
#include "components/keyed_service/core/keyed_service.h"

class PrefRegistrySimple;
class PrefService;

namespace sidebar {

// This manages per-context sidebar items list.
class SidebarService : public KeyedService {
 public:
  class Observer : public base::CheckedObserver {
   public:
    virtual void OnItemAdded(const SidebarItem& item, int index) = 0;

   protected:
    ~Observer() override = default;
  };

  static void RegisterPrefs(PrefRegistrySimple* registry);

  explicit SidebarService(PrefService* prefs);
  ~SidebarService() override;

  std::vector<SidebarItem> items() const { return items_; }

  void AddItem(const SidebarItem& item);
  void RemoveItem(const SidebarItem& item);

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  SidebarService(const SidebarService&) = delete;
  SidebarService& operator=(const SidebarService&) = delete;

 private:
  void LoadSidebarItems();

  PrefService* prefs_ = nullptr;
  std::vector<SidebarItem> items_;
  base::ObserverList<Observer> observers_;
};

}  // namespace sidebar

#endif  // BRAVE_COMPONENTS_SIDEBAR_SIDEBAR_SERVICE_H_
