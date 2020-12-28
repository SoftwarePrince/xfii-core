/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_targeting/processors/behavioral/bandits/epsilon_greedy_bandit_processor.h"

#include <algorithm>
#include <vector>
#include <utility>

#include "base/strings/string_number_conversions.h"
#include "bat/ads/internal/ads_client_helper.h"
#include "bat/ads/internal/ad_targeting/data_types/behavioral/bandits/epsilon_greedy_bandit_arms.h"
#include "bat/ads/internal/ad_targeting/resources/behavioral/bandits/epsilon_greedy_bandit_resource.h"
#include "bat/ads/internal/ad_targeting/ad_targeting_util.h"
#include "bat/ads/internal/logging.h"
#include "bat/ads/pref_names.h"

namespace ads {
namespace ad_targeting {
namespace processor {

namespace {
const double kArmDefaultValue = 1.0;
const uint64_t kArmDefaultPulls = 0;
}  // namespace

EpsilonGreedyBandit::EpsilonGreedyBandit() {
  InitializeArms();
}

EpsilonGreedyBandit::~EpsilonGreedyBandit() = default;

void EpsilonGreedyBandit::Process(
    const BanditFeedback& feedback) {
  const std::string segment = GetParentSegment(feedback.segment);

  switch (feedback.ad_event_type) {
    case AdNotificationEventType::kTimedOut:
    case AdNotificationEventType::kDismissed: {
      UpdateArm(/* reward */ 0, segment);
      break;
    }
    case AdNotificationEventType::kClicked: {
      UpdateArm(/* reward */ 1, segment);
      break;
    }
    case AdNotificationEventType::kViewed: {
      NOTREACHED();
      break;
    }
  }

  BLOG(1, "Processed ad event");
}

///////////////////////////////////////////////////////////////////////////////

void EpsilonGreedyBandit::InitializeArms() const {
  const std::string json = AdsClientHelper::Get()->GetStringPref(
      prefs::kEpsilonGreedyBanditArms);
  EpsilonGreedyBanditArmMap arms = EpsilonGreedyBanditArms::FromJson(json);

  for (const auto& segment : resource::kSegments) {
    auto iter = arms.find(segment);

    if (iter == arms.end()) {
      EpsilonGreedyBanditArmInfo arm;
      arm.value = kArmDefaultValue;
      arm.pulls = kArmDefaultPulls;
      arms[segment] = arm;
      BLOG(6, "Added arm for segment " << segment);
      continue;
    }

    if (!iter->second.IsValid()) {
      EpsilonGreedyBanditArmInfo new_arm;
      new_arm.value = kArmDefaultValue;
      new_arm.pulls = kArmDefaultPulls;
      arms[segment] = new_arm;
      BLOG(6, "Did reset invalid arm for segment  " << segment);
      continue;
    }

    BLOG(6, "Arm exists for segment " << segment);
  }

  for (const auto& arm : arms) {
    auto iter = std::find(resource::kSegments.begin(),
        resource::kSegments.end(), arm.first);

    if (iter == resource::kSegments.end()) {
      arms.erase(arm.first);
      BLOG(6, "Deleted arm for segment " << arm.first);
    }
  }

  const std::string json_out = EpsilonGreedyBanditArms::ToJson(arms);
  AdsClientHelper::Get()->SetStringPref(
      prefs::kEpsilonGreedyBanditArms, json_out);
  BLOG(1, "Successfully initialized arms");
}

void EpsilonGreedyBandit::UpdateArm(
    uint64_t reward,
    const std::string& segment) const {
  const std::string json = AdsClientHelper::Get()->GetStringPref(
      prefs::kEpsilonGreedyBanditArms);
  EpsilonGreedyBanditArmMap arms = EpsilonGreedyBanditArms::FromJson(json);

  if (arms.empty()) {
    return;
  }

  auto iter = arms.find(segment);
  if (iter == arms.end()) {
    BLOG(1, "Arm not found for segment " << segment);
    return;
  }

  EpsilonGreedyBanditArmInfo arm = iter->second;
  arm.pulls = arm.pulls + 1;
  arm.value = arm.value + (1.0 / arm.pulls * (reward - arm.value));
  iter->second = arm;

  const std::string json_out = EpsilonGreedyBanditArms::ToJson(arms);
  AdsClientHelper::Get()->SetStringPref(
      prefs::kEpsilonGreedyBanditArms, json_out);

  BLOG(1, "Arm updated for segment " << segment);
}

}  // namespace processor
}  // namespace ad_targeting
}  // namespace ads
