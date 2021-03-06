/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ipfs/ipfs_service_factory.h"

#include "base/feature_list.h"
#include "base/path_service.h"
#include "brave/browser/brave_browser_process_impl.h"
#include "brave/browser/profiles/profile_util.h"
#include "brave/components/ipfs/features.h"
#include "brave/components/ipfs/ipfs_constants.h"
#include "brave/components/ipfs/ipfs_service.h"
#include "brave/components/ipfs/pref_names.h"
#include "chrome/browser/profiles/incognito_helpers.h"
#include "chrome/common/chrome_paths.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/prefs/pref_service.h"
#include "components/user_prefs/user_prefs.h"
#include "extensions/browser/extension_registry_factory.h"
#include "extensions/browser/extension_system.h"
#include "extensions/browser/extension_system_provider.h"
#include "extensions/browser/extensions_browser_client.h"

namespace ipfs {

// static
bool IpfsServiceFactory::IsIpfsDisabledByPolicy() {
  if (!g_brave_browser_process)
    return false;

  PrefService* local_state = g_brave_browser_process->local_state();

  // Because we currently do not provide a settings switch for IPFSEnabled
  // preference to be overwritten by users, the policy is configured that only
  // a mandatory value can be set by admins.
  return local_state->IsManagedPreference(kIPFSEnabled) &&
         !local_state->GetBoolean(kIPFSEnabled);
}

// static
bool IpfsServiceFactory::IsIpfsEnabled(content::BrowserContext* context) {
  if (!brave::IsRegularProfile(context) || IsIpfsDisabledByPolicy() ||
      !base::FeatureList::IsEnabled(ipfs::features::kIpfsFeature))
    return false;

  return true;
}

// static
bool IpfsServiceFactory::IsIpfsResolveMethodDisabled(
    content::BrowserContext* context) {
  // Ignore the actual pref value if IPFS feature is disabled.
  if (!IsIpfsEnabled(context)) {
    return true;
  }

  PrefService* user_prefs = user_prefs::UserPrefs::Get(context);
  return user_prefs->FindPreference(kIPFSResolveMethod) &&
         user_prefs->GetInteger(kIPFSResolveMethod) ==
            static_cast<int>(ipfs::IPFSResolveMethodTypes::IPFS_DISABLED);
}

// static
IpfsServiceFactory* IpfsServiceFactory::GetInstance() {
  return base::Singleton<IpfsServiceFactory>::get();
}

// static
IpfsService* IpfsServiceFactory::GetForContext(
    content::BrowserContext* context) {
  if (!IsIpfsEnabled(context))
    return nullptr;

  return static_cast<IpfsService*>(
      GetInstance()->GetServiceForBrowserContext(context, true));
}

IpfsServiceFactory::IpfsServiceFactory()
    : BrowserContextKeyedServiceFactory(
          "IpfsService",
          BrowserContextDependencyManager::GetInstance()) {
  DependsOn(extensions::ExtensionRegistryFactory::GetInstance());
  DependsOn(
      extensions::ExtensionsBrowserClient::Get()->GetExtensionSystemFactory());
}

IpfsServiceFactory::~IpfsServiceFactory() {}

KeyedService* IpfsServiceFactory::BuildServiceInstanceFor(
    content::BrowserContext* context) const {
  base::FilePath user_data_dir;
  base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir);
  return new IpfsService(context,
                         g_brave_browser_process
                             ? g_brave_browser_process->ipfs_client_updater()
                             : nullptr,
                         user_data_dir);
}

content::BrowserContext* IpfsServiceFactory::GetBrowserContextToUse(
    content::BrowserContext* context) const {
  return chrome::GetBrowserContextRedirectedInIncognito(context);
}

}  // namespace ipfs
