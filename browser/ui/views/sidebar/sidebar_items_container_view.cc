/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/views/sidebar/sidebar_items_container_view.h"

#include "base/bind.h"
#include "base/notreached.h"
#include "brave/browser/ui/brave_browser.h"
#include "brave/browser/ui/sidebar/sidebar_controller.h"
#include "brave/browser/ui/sidebar/sidebar_model.h"
#include "brave/browser/ui/views/sidebar/sidebar_item_view.h"
#include "brave/grit/brave_theme_resources.h"
#include "chrome/browser/favicon/favicon_service_factory.h"
#include "components/favicon/core/favicon_service.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/layout/box_layout.h"

namespace {

int GetImageResourcesForBuiltInItem(const GURL& item_url, bool focused) {
  if (item_url == GURL("brave://wallet/"))
    return focused ? IDR_SIDEBAR_CRYPTO_WALLET_FOCUSED
                   : IDR_SIDEBAR_CRYPTO_WALLET;
  if (item_url == GURL("https://together.brave.com/"))
    return focused ? IDR_SIDEBAR_BRAVE_TOGETHER_FOCUSED
                   : IDR_SIDEBAR_BRAVE_TOGETHER;
  if (item_url == GURL("brave://bookmarks/"))
    return focused ? IDR_SIDEBAR_BOOKMARKS_FOCUSED : IDR_SIDEBAR_BOOKMARKS;
  if (item_url == GURL("brave://history/"))
    return focused ? IDR_SIDEBAR_HISTORY_FOCUSED : IDR_SIDEBAR_HISTORY;

  NOTREACHED();
  return IDR_SIDEBAR_BRAVE_TOGETHER;
}

}  // namespace

SidebarItemsContainerView::SidebarItemsContainerView(BraveBrowser* browser)
    : browser_(browser),
      task_tracker_(new base::CancelableTaskTracker{}) {
  DCHECK(browser_);

  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical));

  DCHECK(browser->sidebar_controller());
  observed_.Add(browser->sidebar_controller()->model());
}

SidebarItemsContainerView::~SidebarItemsContainerView() = default;

gfx::Size SidebarItemsContainerView::CalculatePreferredSize() const {
  if (children().empty())
    return {0, 0};
  const gfx::Size child_size = children()[0]->GetPreferredSize();
  return {child_size.width() + GetInsets().width(),
          children().size() * child_size.height() + GetInsets().height()};
}

void SidebarItemsContainerView::OnItemAdded(
    const sidebar::SidebarItem& item, int index) {
  AddItemView(item, index);
}

void SidebarItemsContainerView::OnActiveIndexChanged(int old_index,
                                                     int new_index) {
  if (old_index != -1)
    UpdateItemViewStateAt(old_index, false);

  if (new_index != -1)
    UpdateItemViewStateAt(new_index, true);
}

void SidebarItemsContainerView::AddItemView(
    const sidebar::SidebarItem& item,
    int index) {
  auto* item_view = AddChildViewAt(std::make_unique<SidebarItemView>(), index);
  item_view->SetCallback(
      base::BindRepeating(
          &SidebarItemsContainerView::OnItemPressed,
          base::Unretained(this),
          item_view));
  UpdateItemViewStateAt(index, false);
  if (sidebar::IsWebType(item))
    FetchFavicon(item, index);

  InvalidateLayout();
  Layout();
}

void SidebarItemsContainerView::FetchFavicon(const sidebar::SidebarItem& item,
                                             int index) {
  // Use favicon as a web type icon's image.
  auto* favicon_service = FaviconServiceFactory::GetForProfile(
      browser_->profile(),
      ServiceAccessType::EXPLICIT_ACCESS);
  favicon_service->GetFaviconImageForPageURL(
      item.url,
      base::Bind(&SidebarItemsContainerView::OnGetFaviconImage,
                 weak_ptr_factory_.GetWeakPtr(), item, index),
      task_tracker_.get());
}

void SidebarItemsContainerView::OnGetFaviconImage(
    const sidebar::SidebarItem& item,
    int index,
    const favicon_base::FaviconImageResult& image_result) {
  // TODO(simonhong): Use default image if we can't get site favicon.
  if (image_result.image.IsEmpty())
    return;

  // TODO(simonhong): Check item is still valid. Item could be removed while
  // favicon fetching.
  SidebarItemView* item_view = static_cast<SidebarItemView*>(children()[index]);
  item_view->SetImage(views::Button::STATE_NORMAL,
                      image_result.image.AsImageSkia());
}

void SidebarItemsContainerView::UpdateItemViewStateAt(int index, bool active) {
  auto* item_data =
      browser_->sidebar_controller()->model()->data()[index].get();
  SidebarItemView* item_view = static_cast<SidebarItemView*>(children()[index]);

  if (item_data->OpenInPanel())
    item_view->SetActive(active);

  if (item_data->IsBuiltInType()) {
    auto& bundle = ui::ResourceBundle::GetSharedInstance();
    const GURL& url = item_data->GetURL();
    item_view->SetImage(
        views::Button::STATE_NORMAL,
        bundle.GetImageSkiaNamed(GetImageResourcesForBuiltInItem(url, active)));
    // Use focused image for hovered/pressed state.
    item_view->SetImage(
        views::Button::STATE_HOVERED,
        bundle.GetImageSkiaNamed(GetImageResourcesForBuiltInItem(url, true)));
    item_view->SetImage(
        views::Button::STATE_PRESSED,
        bundle.GetImageSkiaNamed(GetImageResourcesForBuiltInItem(url, true)));
  }
}

void SidebarItemsContainerView::OnItemPressed(const views::View* item) {
  browser_->sidebar_controller()->ActivateItemAt(GetIndexOf(item));
}
