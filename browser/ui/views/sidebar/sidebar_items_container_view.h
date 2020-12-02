/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_ITEMS_CONTAINER_VIEW_H_
#define BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_ITEMS_CONTAINER_VIEW_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "base/scoped_observer.h"
#include "brave/browser/ui/sidebar/sidebar_model.h"
#include "brave/components/sidebar/sidebar_item.h"
#include "ui/views/view.h"

namespace base {
class CancelableTaskTracker;
}  // namespace base

namespace favicon_base {
struct FaviconImageResult;
}  // namespace favicon_base

class BraveBrowser;

class SidebarItemsContainerView : public views::View,
                                  public sidebar::SidebarModel::Observer {
 public:
  explicit SidebarItemsContainerView(BraveBrowser* browser);
  ~SidebarItemsContainerView() override;

  SidebarItemsContainerView(const SidebarItemsContainerView&) = delete;
  SidebarItemsContainerView operator=(
      const SidebarItemsContainerView&) = delete;

  // views::View overrides:
  gfx::Size CalculatePreferredSize() const override;

  // sidebar::SidebarModel::Observer overrides:
  void OnItemAdded(const sidebar::SidebarItem& item, int index) override;
  void OnActiveIndexChanged(int old_index, int new_index) override;

 private:
  void AddItemView(const sidebar::SidebarItem& item, int index);
  void UpdateItemViewStateAt(int index, bool active);

  // Called when each item is pressed.
  void OnItemPressed(const views::View* item);
  void FetchFavicon(const sidebar::SidebarItem& item, int index);
  void OnGetFaviconImage(
      const sidebar::SidebarItem& item,
      int index,
      const favicon_base::FaviconImageResult& image_result);

  BraveBrowser* browser_ = nullptr;
  ScopedObserver<sidebar::SidebarModel,
                 sidebar::SidebarModel::Observer> observed_{this};
  std::unique_ptr<base::CancelableTaskTracker> task_tracker_;
  base::WeakPtrFactory<SidebarItemsContainerView> weak_ptr_factory_{this};
};

#endif  // BRAVE_BROWSER_UI_VIEWS_SIDEBAR_SIDEBAR_ITEMS_CONTAINER_VIEW_H_
