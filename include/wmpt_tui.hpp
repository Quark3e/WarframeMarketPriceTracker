#pragma once
#ifndef HPP_WMPT_TUI
#define HPP_WMPT_TUI

#include <mutex>
#include <thread>
#include <atomic>

#include <string>

#include <initializer_list>
#include <vector>
#include <functional>
#include <map>

#include "wmpt_useful.hpp"
#include <Pos2d.hpp>



namespace TUINC {

    enum Results {
        Success,
        ThreadInitialisationFailure,
        ToastClicked,             // user clicked on the toast
        ToastDismissed,           // user dismissed the toast
        ToastTimeOut,             // toast timed out
        ToastHided,               // application hid the toast
        ToastNotActivated,        // toast was not activated
        ToastFailed,              // toast failed
        SystemNotSupported,       // system does not support toasts
        UnhandledOption,          // unhandled option
        MultipleTextNotSupported, // multiple texts were provided
        InitialisationFailure,    // toast notification manager initialization failure
        ToastNotLaunched          // toast could not be launched
        
    };

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
    

    extern PriceTracker::callbackType_trackedAllOffers callbackFunc_trackedAllOffers;
    extern PriceTracker::callbackType_trackedFound callbackFunc_offersFound;


    extern PriceTracker::threadClass threadObj_PriceTracker;


    Results Initialise();

    int Drive();

    
    struct  __cell;
    class   __table;

    using __type_cellFunc = std::function<void(__table&)>;

    struct  __cell {
        std::string     label;
        __type_cellFunc function;
        Pos2d<int>      pos;
        
        bool    __nullCell  = false;

        bool    __isDefined_label   = false;
        bool    __isDefined_function= false;
        bool    __isDefined_pos     = false;

        __cell(bool _nullCell=true);
        __cell(std::string _label);
        __cell(std::string _label, Pos2d<int> _pos);
        __cell(std::string _label, __type_cellFunc _function);
        __cell(std::string _label, __type_cellFunc _function, Pos2d<int> _pos);

        __cell(const __cell& _toCopy);
        __cell(__cell&& _toMove);
        ~__cell();
        __cell& operator=(const __cell& _toCopy);
        __cell& operator=(__cell&& _toMove);
    };

    class   __table {
        private:
        /**
         * access index: [row][column] ([y][x])
         *  {
         *      {[0, 0], [0, 1], [0, 2]},
         *      {[1, 0], [1, 1], [1, 2]}
         *  }
         */
        std::vector<std::vector<__cell>> __tableOfCells;

        public:
        __table(std::initializer_list<std::initializer_list<__cell>> _matrixInput);

        __table();
        __table(const __table& _toCopy);
        __table(__table&& _toMove);
        ~__table();
        __table& operator=(const __table& _toCopy);
        __table& operator=(__table&& _toMove);

    };

};


#endif //HPP_WMPT_TUI
