#pragma once
#ifndef HPP_WMPT_TUI
#define HPP_WMPT_TUI

#include <mutex>
#include <thread>
#include <atomic>

#include <string>

#include <vector>
#include <map>

#include "wmpt_types.hpp"
#include "wmpt_useful.hpp"
#include <Pos2d.hpp>



namespace TUINC {
    

    std::string __str_printBuffer;
    // std::string __str_printBuffer_trackedOffers;
    struct TrackItemOffers {
        PriceTracker::TrackItem trackItem;
        std::vector<PriceTracker::ItemOffer> trackOffers;
    };
    /**
     * A std::map with TrackID std::string as key and a custom struct holding PriceTracker::TrackItem and std::vector<PriceTracker::ItemOffer> as value.
     * 
     */
    std::map<std::string, TrackItemOffers> TrackingItemsOffers;
    std::mutex __mtx_access_TrackingItemsOffers;

    std::atomic<char> settingTUI__charColumnDelim{'|'};

    std::atomic<int> settingTUI__charWidth_itemName{15};
    std::atomic<int> settingTUI__charWidth_trackingRank{3};
    std::atomic<int> settingTUI__charWidth_trackingPrice{3};
    std::atomic<int> settingTUI__charWidth_currrentBestPrice{3};
    

    PriceTracker::callbackType_trackedAllOffers callbackFunc_trackedAllOffers = [](PriceTracker::TrackItem _item, std::vector<PriceTracker::ItemOffer> _offers) {
        std::unique_lock<std::mutex> u_lck__TrackingItemOffer(__mtx_access_TrackingItemsOffers);
        TrackingItemsOffers[_item.getTrackID()] = {_item, _offers};
    };
    PriceTracker::callbackType_trackedFound callbackFunc_offersFound = [](PriceTracker::TrackItem _item, std::vector<PriceTracker::ItemOffer> _offers) {
        std::unique_lock<std::mutex> u_lck__TrackingItemOffer(__mtx_access_TrackingItemsOffers);
        TrackingItemsOffers[_item.getTrackID()] = {_item, _offers};

    
    };

    bool init() {
        // callbackFunc_trackedAlloffers = [](PriceTracker::TrackItem _item, std::vector<PriceTracker::ItemOffer> _offers) {
        // };
    }


};


#endif //HPP_WMPT_TUI