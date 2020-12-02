/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/sidebar/sidebar_model.h"

#include "base/logging.h"
#include "brave/browser/ui/sidebar/sidebar_model_data.h"
#include "brave/browser/ui/sidebar/sidebar_service_factory.h"
#include "brave/components/sidebar/sidebar_item.h"

namespace sidebar {

SidebarModel::SidebarModel(Profile* profile) : profile_(profile) {}

SidebarModel::~SidebarModel() = default;

void SidebarModel::Init() {
  SidebarService* service =
      SidebarServiceFactory::GetForProfile(profile_);
  DCHECK(service);

  // Start with saved item list.
  for (const auto& item : service->items())
    AddItem(item, -1);

  observed_.Add(service);
}

void SidebarModel::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void SidebarModel::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void SidebarModel::AddItem(const SidebarItem& item, int index) {
  data_.push_back(std::make_unique<SidebarModelData>(item, profile_));
  for (Observer& obs : observers_) {
    // Index starts at zero. If |index| is -1, add as a last item.
    obs.OnItemAdded(item, index == -1 ? data_.size() - 1 : index);
  }

  // If active_index_ is not -1, check this addition affetcs active index.
  if (active_index_ != -1 && active_index_ >= index)
    UpdateActiveIndexAndNotify(index);
}

void SidebarModel::RemoveItem(int index) {
}

void SidebarModel::SetActiveIndex(int index) {
  if (index == active_index_)
    return;

  UpdateActiveIndexAndNotify(index);
}

base::Optional<int> SidebarModel::GetActiveIndex() {
  if (active_index_ < 0)
    return base::nullopt;
  return active_index_;
}

void SidebarModel::UpdateActiveIndexAndNotify(int new_active_index) {
  const int old_active_index = active_index_;
  active_index_ = new_active_index;

  for (Observer& obs : observers_)
    obs.OnActiveIndexChanged(old_active_index, active_index_);
}

void SidebarModel::OnItemAdded(const SidebarItem& item, int index) {
  AddItem(item, index);
}

}  // namespace sidebar
