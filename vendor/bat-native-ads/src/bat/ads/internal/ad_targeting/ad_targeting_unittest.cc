/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/ad_targeting/ad_targeting.h"

#include <map>
#include <string>
#include <vector>

#include "base/strings/stringprintf.h"
#include "base/test/scoped_feature_list.h"
#include "bat/ads/internal/ad_targeting/processors/behavioral/bandits/epsilon_greedy_bandit_processor.h"
#include "bat/ads/internal/ad_targeting/processors/behavioral/purchase_intent/purchase_intent_processor.h"
#include "bat/ads/internal/ad_targeting/processors/contextual/text_classification/text_classification_processor.h"
#include "bat/ads/internal/ad_targeting/resources/behavioral/bandits/epsilon_greedy_bandit_resource.h"
#include "bat/ads/internal/ad_targeting/resources/behavioral/purchase_intent/purchase_intent_resource.h"
#include "bat/ads/internal/ad_targeting/resources/contextual/text_classification/text_classification_resource.h"
#include "bat/ads/internal/features/bandits/epsilon_greedy_bandit_features.h"
#include "bat/ads/internal/features/purchase_intent/purchase_intent_features.h"
#include "bat/ads/internal/features/text_classification/text_classification_features.h"
#include "bat/ads/internal/unittest_base.h"
#include "bat/ads/internal/unittest_util.h"

// npm run test -- brave_unit_tests --filter=BatAds*

namespace ads {
namespace ad_targeting {

namespace {

struct ModelCombinationsParamInfo {
  bool epsilon_greedy_bandits_enabled;
  bool purchase_intent_enabled;
  bool text_classification_enabled;
  bool previously_processed;
  size_t number_of_segments;
};

// Expected number of segments for all possible model combinations for both,
// never processed and previously processed state
const ModelCombinationsParamInfo kTests[] = {
  // Never processed
  {false, false, false, false, 0},
  {false, false, true, false, 1},
  {false, true, false, false, 0},
  {false, true, true, false, 1},
  {true, false, false, false, 3},
  {true, false, true, false, 4},
  {true, true, false, false, 3},
  {true, true, true, false, 4},
  // Previously processed
  {false, false, false, true, 0},
  {false, false, true, true, 3},
  {false, true, false, true, 2},
  {false, true, true, true, 5},
  {true, false, false, true, 3},
  {true, false, true, true, 6},
  {true, true, false, true, 5},
  {true, true, true, true, 8},
};

}  // namespace

class BatAdsAdTargetingTest
    : public UnitTestBase,
      public ::testing::WithParamInterface<ModelCombinationsParamInfo> {
 protected:
  BatAdsAdTargetingTest() = default;

  ~BatAdsAdTargetingTest() override = default;
};

TEST_P(BatAdsAdTargetingTest,
    GetSegments) {
  // Arrange
  // We always instantitate processors even if features are disabled
  processor::EpsilonGreedyBandit bandit_processor;

  resource::PurchaseIntent purchase_intent_resource;
  purchase_intent_resource.LoadForLocale("en-US");
  processor::PurchaseIntent purchase_intent_processor(
      &purchase_intent_resource);

  resource::TextClassification text_classification_resource;
  text_classification_resource.LoadForLocale("en-US");
  processor::TextClassification text_classification_processor(
      &text_classification_resource);

  std::vector<base::test::ScopedFeatureList::FeatureAndParams> enabled_features;
  std::vector<base::Feature> disabled_features;

  ModelCombinationsParamInfo param(GetParam());
  if (param.previously_processed) {
    const std::vector<std::string> texts = {
      "Some content about cooking food",
      "Some content about finance & banking",
      "Some content about technology & computing"
    };

    for (const auto& text : texts) {
      text_classification_processor.Process(text);
    }

    const std::vector<GURL> urls = {
      GURL("https://www.brave.com/test?foo=bar"),
      GURL("https://www.basicattentiontoken.org/test?bar=foo"),
      GURL("https://www.brave.com/test?foo=bar")
    };

    for (const auto& url : urls) {
      purchase_intent_processor.Process(url);
    }

    const std::vector<processor::BanditFeedback> feedbacks = {
      {"science", AdNotificationEventType::kClicked},
      {"science", AdNotificationEventType::kClicked},
      {"science", AdNotificationEventType::kClicked},
      {"travel", AdNotificationEventType::kDismissed},
      {"travel", AdNotificationEventType::kClicked},
      {"travel", AdNotificationEventType::kClicked},
      {"technology & computing", AdNotificationEventType::kDismissed},
      {"technology & computing", AdNotificationEventType::kDismissed},
      {"technology & computing", AdNotificationEventType::kClicked}
    };

    for (const auto& segment : resource::kSegments) {
      bandit_processor.Process({segment, AdNotificationEventType::kDismissed});
    }

    for (const auto& feedback : feedbacks) {
      bandit_processor.Process(feedback);
    }
  }

  if (param.epsilon_greedy_bandits_enabled) {
    const char kEpsilonValue[] = "epsilon_value";
    base::FieldTrialParams kEpsilonGreedyBanditParameters;
    // Set bandit to always exploit for deterministic execution
    kEpsilonGreedyBanditParameters[kEpsilonValue] = "0.0";
    enabled_features.push_back({features::kEpsilonGreedyBandit,
        kEpsilonGreedyBanditParameters});
  } else {
    disabled_features.push_back(features::kEpsilonGreedyBandit);
  }

  if (param.purchase_intent_enabled) {
    enabled_features.push_back({features::kPurchaseIntent, {}});
  } else {
    disabled_features.push_back(features::kPurchaseIntent);
  }

  if (param.text_classification_enabled) {
    enabled_features.push_back({features::kTextClassification, {}});
  } else {
    disabled_features.push_back(features::kTextClassification);
  }

  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitWithFeaturesAndParameters(enabled_features,
      disabled_features);

  // Act
  AdTargeting ad_targeting;
  const SegmentList segments = ad_targeting.GetSegments();

  // Assert
  EXPECT_EQ(param.number_of_segments, segments.size());
}

static std::string GetTestCaseName(
    ::testing::TestParamInfo<ModelCombinationsParamInfo> param_info) {
  const char* epsilon_greedy_bandits_enabled =
      param_info.param.epsilon_greedy_bandits_enabled ?
          "EpsilonGreedyBanditEnabledAnd" : "";

  const char* purchase_intent_enabled =
      param_info.param.purchase_intent_enabled ?
          "PurchaseIntentEnabledAnd" : "";

  const char* text_classification_enabled =
      param_info.param.text_classification_enabled ?
          "TextClassificationEnabledAnd" : "";

  const char* previously_processed =
      param_info.param.previously_processed ?
          "PreviouslyProcessed" : "NeverProcessed";

  return base::StringPrintf("For%s%s%s%s", epsilon_greedy_bandits_enabled,
      purchase_intent_enabled, text_classification_enabled,
          previously_processed);
}

INSTANTIATE_TEST_SUITE_P(BatAdsAdTargetingTest,
    BatAdsAdTargetingTest, ::testing::ValuesIn(kTests), GetTestCaseName);

TEST_F(BatAdsAdTargetingTest,
    GetSegmentsForAllModelsIfPreviouslyProcessed) {
  // Arrange
  // TODO(Moritz Haller): Maybe pull out processing in function
  const char kEpsilonValue[] = "epsilon_value";
  std::map<std::string, std::string> kEpsilonGreedyBanditParameters;
  // Set bandit to always exploit for deterministic execution
  kEpsilonGreedyBanditParameters[kEpsilonValue] = "0.0";

  base::test::ScopedFeatureList scoped_feature_list;
  scoped_feature_list.InitWithFeaturesAndParameters({
      {features::kPurchaseIntent, /* default params */ {}},
      {features::kEpsilonGreedyBandit, kEpsilonGreedyBanditParameters},
      {features::kTextClassification, /* default params */ {}}}, {});

  const std::vector<std::string> texts = {
    "Some content about cooking food",
    "Some content about finance & banking",
    "Some content about technology & computing"
  };
  resource::TextClassification text_classification_resource;
  text_classification_resource.LoadForLocale("en-US");
  processor::TextClassification text_classification_processor(
      &text_classification_resource);
  for (const auto& text : texts) {
    text_classification_processor.Process(text);
  }

  const std::vector<GURL> urls = {
    GURL("https://www.brave.com/test?foo=bar"),
    GURL("https://www.basicattentiontoken.org/test?bar=foo"),
    GURL("https://www.brave.com/test?foo=bar")
  };
  resource::PurchaseIntent purchase_intent_resource;
  purchase_intent_resource.LoadForLocale("en-US");
  processor::PurchaseIntent purchase_intent_processor(
      &purchase_intent_resource);
  for (const auto& url : urls) {
    purchase_intent_processor.Process(url);
  }

  const std::vector<processor::BanditFeedback> feedbacks = {
    {"science", AdNotificationEventType::kClicked},
    {"science", AdNotificationEventType::kClicked},
    {"science", AdNotificationEventType::kClicked},
    {"travel", AdNotificationEventType::kDismissed},
    {"travel", AdNotificationEventType::kClicked},
    {"travel", AdNotificationEventType::kClicked},
    {"technology & computing", AdNotificationEventType::kDismissed},
    {"technology & computing", AdNotificationEventType::kDismissed},
    {"technology & computing", AdNotificationEventType::kClicked}
  };
  processor::EpsilonGreedyBandit bandit_processor;
  // Set all values to zero by choosing a zero-reward action due to optimistic
  // initial values for arms
  for (const auto& segment : resource::kSegments) {
    bandit_processor.Process({segment, AdNotificationEventType::kDismissed});
  }
  for (const auto& feedback : feedbacks) {
    bandit_processor.Process(feedback);
  }

  // Act
  AdTargeting ad_targeting;
  const SegmentList segments = ad_targeting.GetSegments();

  // Assert
  const SegmentList expected_segments = {
    "technology & computing-technology & computing",
    "personal finance-banking",
    "food & drink-cooking",
    "segment 3",
    "segment 2",
    "science",
    "travel",
    "technology & computing"  // TODO(Moritz): logic to remove redunant segments?? NOLINT
  };

  EXPECT_EQ(expected_segments, segments);
}

}  // namespace ad_targeting
}  // namespace ads
