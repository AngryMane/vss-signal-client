#pragma once
#include <string>

namespace Constants {
    namespace viss {
        // tags
        constexpr char SUB_PROTOCOL[] = "VISSv2";
        constexpr char REQUEST_ID_TAG[] = "requestId";
        constexpr char ACTION_TAG[] = "action";
        constexpr char PATH_TAG[] = "path";
        constexpr char VALUE_TAG[] = "value";
        constexpr char SUBSCRIPTION_ID_TAG[] = "subscriptionId";

        // actions
        constexpr char ACTION_GET[] = "get";
        constexpr char ACTION_SET[] = "set";
        constexpr char ACTION_SUBSCRIBE[] = "subscribe";
        constexpr char ACTION_UNSUBSCRIBE[] = "unsubscribe";
    };
};