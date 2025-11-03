#pragma once
#ifndef HPP_WMPT_TYPES
#define HPP_WMPT_TYPES


#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <vector>
#include <chrono>

#include <stdexcept>
#include <cassert>

#include <iostream>
#include <iomanip>

#include <thread>
#include <mutex>
#include <atomic>

#include <functional>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "wmpt_useful.hpp"
#include "wmpt_variables.hpp"

#ifndef NOMINMAX
# define NOMINMAX
#endif
// #include <windows.h>
#undef max

class CustomException : public std::exception {
private:
    std::string message;
public:

    CustomException(const char* msg) : message(msg) {}

    const char* what() const noexcept {
        return message.c_str();
    }
};

enum itemType {
    itemType_default,
    itemType_Mods,
    itemType_Component,
    itemType_Set
};
itemType find_itemType(std::string _typeStr);
inline std::string getStrFromType_itemType(itemType _type) {
    switch (_type) {
    case itemType_default:
        return "default";
        break;
    case itemType_Mods:
        return "Mods";
        break;
    case itemType_Component:
        return "Component";
        break;
    case itemType_Set:
        return "Set";
        break;
    default:
        assert(false && "std::string getStrFromType_itemType(itemType); itemType is somehow invalid.");
        break;
    }

    return "";
}

struct itemInfo {
    std::string id;
    std::string name;

    itemType    type;
    
    std::string rawsStr_types;

    std::string getStr() const {
        std::stringstream ss;
        ss << "{id:" << this->id << ", name:" << Useful::formatNumber(this->name, 40, 0, "left") << ", type:" << getStrFromType_itemType(this->type) << ", raw_types:"<<this->rawsStr_types;

        return ss.str();
    }
    operator std::string() {
        return getStr();
    }

    friend auto operator<<(std::ostream &os, itemInfo const& m) -> std::ostream& {
        os << m.getStr();
    }
};



struct __keyPressHandler_keyDetails {
    std::chrono::steady_clock::time_point startTime; // time since press start
    bool isPressed; // whether the key is actually being pressed
    bool active; // whether this key is to give a signal as being pressed.


    operator std::string() {
        std::stringstream ss;
        ss << this->startTime.time_since_epoch().count() << "," << std::boolalpha << this->isPressed << "," << this->active;
        
        return ss.str();
    }

    friend auto operator<<(std::ostream &os, __keyPressHandler_keyDetails const& m) -> std::ostream& {
        os << m.startTime.time_since_epoch().count() << "," << std::boolalpha << m.isPressed <<  ","<< m.active;

        return os;
    }
};
class keyPressHandler {
private:
    // std::vector<int> __keys_pressed;
    // std::vector<std::chrono::steady_clock::time_point> __keys_timePressed;
    std::unordered_map<int, __keyPressHandler_keyDetails> __pressed_keys;

    std::vector<int> __active_keys;

public:
    std::chrono::steady_clock::time_point refrTime_now;
    std::chrono::duration<double> pressTypeDifrDecayDur_seconds = std::chrono::duration<double>(0.3);

    keyPressHandler() {
        for(size_t i=0; i<256; i++) {
            __pressed_keys[i] = __keyPressHandler_keyDetails{
                std::chrono::time_point<std::chrono::steady_clock>::max(),
                false,
                false
            };
        }
    }
    keyPressHandler(const keyPressHandler& _toCopy) {
        this->__pressed_keys = _toCopy.__pressed_keys;
        this->__active_keys  = _toCopy.__active_keys;
    }
    keyPressHandler(keyPressHandler&& _toSwap) {
        std::swap(this->__pressed_keys, _toSwap.__pressed_keys);
        std::swap(this->__active_keys,  _toSwap.__active_keys);
    }
    ~keyPressHandler() {}
    keyPressHandler& operator=(const keyPressHandler& _toCopy) {
        this->__pressed_keys = _toCopy.__pressed_keys;
        this->__active_keys  = _toCopy.__active_keys;
    }

    const std::vector<int>& updateKeys(const std::vector<int>& newKeys) {
        refrTime_now = std::chrono::steady_clock::now();

        for(size_t i=0; i<256; i++) __pressed_keys[i].isPressed = false;
        for(size_t i=0; i<newKeys.size(); i++) {
            __pressed_keys[newKeys.at(i)].isPressed = true;
            if(__pressed_keys[newKeys.at(i)].startTime==std::chrono::time_point<std::chrono::steady_clock>::max()) {
                __pressed_keys[newKeys.at(i)].startTime = refrTime_now;
                // __pressed_keys[newKeys.at(i)].active = true;
            }
            else {

            }
        }
        __active_keys.clear();
        for(size_t i=0; i<256; i++) {
            __keyPressHandler_keyDetails& keyRef = __pressed_keys[i];

            if(!keyRef.isPressed) {
                keyRef.startTime = std::chrono::time_point<std::chrono::steady_clock>::max();
                keyRef.active = false;
                continue;
            }

            if(keyRef.startTime==refrTime_now || (refrTime_now-keyRef.startTime)>pressTypeDifrDecayDur_seconds) {
                keyRef.active = true;
                __active_keys.push_back(i);
            }
            else {
                keyRef.active = false;
            }
        }

        return __active_keys;
    }

    const std::vector<int>& getActiveKeys() {
        return __active_keys;
    }
    __keyPressHandler_keyDetails getKeyDetail(int _key) {
        return __pressed_keys.at(_key);
    }
    std::unordered_map<int, __keyPressHandler_keyDetails> getAllKeyDetails() {
        return __pressed_keys;
    }

    bool isPressed(int _key) {
        return this->__pressed_keys.at(_key).isPressed;
    }

};

enum keyASCII {
    ENTER       = 13,
    SHIFT       = 16,
    CTRL        = 17,
    ESC         = 27,
    ARROW_LEFT  = 37,
    ARROW_UP    = 38,
    ARROW_RIGHT = 39,
    ARROW_DOWN  = 40,
};


namespace PriceTracker {
    class apiCallTimer {
    private:
        const std::chrono::duration<double> rateLimit_minRequestDelay{std::chrono::milliseconds(500)};

        static std::mutex __mtx_callWM;
        static std::chrono::time_point<std::chrono::steady_clock> __time_prevAPIrequest;

        std::unique_lock<std::mutex> __u_lck_mtxCallWM;
    public:
        apiCallTimer() {
            this->__u_lck_mtxCallWM = std::unique_lock<std::mutex>(__mtx_callWM, std::defer_lock);
        }

        std::string call(const std::string& _url);

    };

    enum trackType {
        trackType_buy,
        trackType_sell
    };
    enum userStatus {
        userStatus_offline,
        userStatus_online,
        userStatus_ingame
    };
    inline userStatus get_userStatus_fromStr(std::string _str) {
        if(_str.size()==0) throw std::runtime_error("get_userStatus_fromStr(std::string) : invalid _str argument. Cannot be empty string.");

        if(_str=="offline")     return userStatus_offline;
        else if(_str=="online") return userStatus_online;
        else if(_str=="ingame") return userStatus_ingame;

        throw std::runtime_error(std::string("get_userStatus_fromStr(std::string) : invalid _str argument. \"")+_str+"\" is invalid.");

        return userStatus_offline;
    }
    
    struct TrackItem {
        std::string id; //item id
        std::string name;

        trackType   type;

        int rank_min;
        int rank_max;

        int plat_min;
        int plat_max;

        TrackItem(
            std::string _id, std::string _name, trackType _type, int _rank_min=-1, int _rank_max=-1, int _plat_min=-1, int _plat_max=-1
        ): id(_id), name(_name), type(_type), rank_min(_rank_min), rank_max(_rank_max), plat_min(_plat_min), plat_max(_plat_max)
        {}


        std::string getTrackID() {
            std::string id_str = "";
            std::string delim = ",";

            id_str += name + delim;
            id_str += "rMin" + std::to_string(rank_min) + delim;
            id_str += "rMax" + std::to_string(rank_max) + delim;
            id_str += "pMin" + std::to_string(plat_min) + delim;
            id_str += "pmax" + std::to_string(plat_max);

            return id_str;
        }
    };

    struct UserInfo {
        std::string id; //user id
        std::string ingameName;
        std::string name_slug;
        int reputation;
        userStatus status;
        std::string date_lastSeen;
    };
    struct ItemOffer {
        std::string id; //offer id
        trackType   type;
        int     platinum;
        int     quantity;
        int     rank;

        std::string date_createdAt;
        std::string date_updatedAt;
        std::string item_id;

        UserInfo    user;
    };


    /**
     * Callback invoked when a tracked item is found.
     *
     * This callback is called with the tracked item and the corresponding offer details.
     *
     * @param tracked The TrackItem identifying the item that was tracked (passed by value).
     * @param offer   The ItemOffer describing the offer found for the tracked item (passed by value).
     *
     * The callback returns void. Implementations should avoid long-running or blocking work
     * and offload heavy processing if necessary to prevent stalling the caller.
     */
    using callbackType_trackedFound = std::function<void(TrackItem, const std::vector<ItemOffer>&)>;
    
    using callbackType_trackedAllOffers = std::function<void(TrackItem, const std::vector<ItemOffer>&)>;

    

    class threadClass {
    private:
        std::vector<TrackItem> __ItemsToTrack;
        std::vector<TrackItem> __ItemsToTrack_updates;
        std::atomic<bool> isChanged_ItemsToTrack{false};
        // std::mutex __mtx_access_updateItemsToTrack;

        callbackType_trackedFound       __callbackFound;
        callbackType_trackedAllOffers   __callbackAllOffers;
        std::mutex __mtx_access_callbackFound;
        std::mutex __mtx_access_callbackAllOffers;
        std::atomic<bool> __isDefined__callbackFound{false};
        std::atomic<bool> __isDefined__callbackAllOffers{false};

        std::atomic<bool>   __running{false};
        std::mutex          __mtx_pauseThreadIteration;
        bool                __isInitialised = false;

        std::thread threadObj;
        friend void threadFunc_threadClass(threadClass& _refObj);
    public:

        threadClass() {
            
        }
        threadClass(callbackType_trackedFound _callback_trackedFound, bool _init=false): __callbackFound(_callback_trackedFound) {
            __isDefined__callbackFound = true;

            if(_init) this->startThread();
        }
        threadClass(callbackType_trackedFound _callback_trackedFound, callbackType_trackedAllOffers _callback_AllOffers, bool _init=false): __callbackFound(_callback_trackedFound), __callbackAllOffers(_callback_AllOffers) {
            __isDefined__callbackFound = true;
            __isDefined__callbackAllOffers = true;

            if(_init) this->startThread();
        }
        threadClass(const threadClass& _toCopy) {
            __ItemsToTrack  = _toCopy.__ItemsToTrack;
            if(_toCopy.__isDefined__callbackFound)      __callbackFound     = _toCopy.__callbackFound;
            if(_toCopy.__isDefined__callbackAllOffers)  __callbackAllOffers = _toCopy.__callbackAllOffers;
            __isDefined__callbackFound      = _toCopy.__isDefined__callbackFound.load();
            __isDefined__callbackAllOffers  = _toCopy.__isDefined__callbackAllOffers.load();
            
        }
        threadClass(threadClass&& _toMove) = delete;
        // {
        //     if(_toMove.__running) {
        //         _toMove.__running = false;
        //         std::unique_lock<std::mutex> u_lck_pauseThread(_toMove.__mtx_pauseThreadIteration, std::defer_lock);
        //         u_lck_pauseThread.lock();
        //         threadObj.swap(_toMove.threadObj);
        //         std::swap(__ItemsToTrack, _toMove.__ItemsToTrack);
        //         std::swap(__mtx_access_updateItemsToTrack, _toMove.__mtx_access_updateItemsToTrack);
        //         if(_toMove.__isDefined__callbackFound)      std::swap(__callbackFound, _toMove.__callbackFound);
        //         if(_toMove.__isDefined__callbackAllOffers)  std::swap(__callbackAllOffers, _toMove.__callbackAllOffers);
        //         std::swap(__mtx_access_callbackFound, _toMove.__mtx_access_callbackFound);
        //         std::swap(__mtx_access_callbackAllOffers, _toMove.__mtx_access_callbackAllOffers);
        //         u_lck_pauseThread.unlock();
        //     }
        // }
        ~threadClass() {
            this->stopThread();
            
        }
        threadClass& operator=(const threadClass& _toCopy) {
            __ItemsToTrack  = _toCopy.__ItemsToTrack;
            if(_toCopy.__isDefined__callbackFound)      __callbackFound     = _toCopy.__callbackFound;
            if(_toCopy.__isDefined__callbackAllOffers)  __callbackAllOffers = _toCopy.__callbackAllOffers;
        }
        threadClass& move(threadClass&& _toMove) = delete;

        void setCallback_trackedFound(callbackType_trackedFound _newCallback) {
            std::unique_lock<std::mutex> u_lck(this->__mtx_access_callbackFound);
            __callbackFound = _newCallback;
            __isDefined__callbackFound = true;
        }
        void setCallback_allOffers(callbackType_trackedAllOffers _newCallback) {
            std::unique_lock<std::mutex> u_lck(this->__mtx_access_callbackAllOffers);
            __callbackAllOffers = _newCallback;
            __isDefined__callbackAllOffers = true;
        }

        size_t size() const {
            return __ItemsToTrack.size();
        }
        std::vector<TrackItem> getAllItems() const {
            if(!__isInitialised) throw std::runtime_error("getAllItems() const : class instance has not been initialised.");
            if(!__running.load()) throw std::runtime_error("getAllItems() const : Tracking sub-thread is not runnng.");

            return __ItemsToTrack;
        }
        // TrackItem& at(size_t _i) {
        //     if(!__isInitialised) throw std::runtime_error("at(size_t) : class instance has not been initialised.");
        //     if(!__running.load()) throw std::runtime_error("at(size_t) : Tracking sub-thread is not runnng.");
        //     std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);
        //     return __ItemsToTrack.at(_i);
        // }
        TrackItem at(size_t _i) {
            if(!__isInitialised) throw std::runtime_error("at(size_t) : class instance has not been initialised.");
            if(!__running.load()) throw std::runtime_error("at(size_t) : Tracking sub-thread is not runnng.");

            std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);
            return __ItemsToTrack.at(_i);
        }
        // TrackItem& operator[](size_t _i) {
        //     std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);
        //     return __ItemsToTrack[_i];
        // }
        TrackItem operator[](size_t _i) {
            std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);
            return __ItemsToTrack[_i];
        }

        void add(TrackItem _newItem) {
            if(!__isInitialised) throw std::runtime_error("add(TrackItem) : class instance has not been initialised.");
            if(!__running.load()) throw std::runtime_error("add(TrackItem) : Tracking sub-thread is not runnng.");

            // std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);

            __ItemsToTrack_updates.push_back(_newItem);
            isChanged_ItemsToTrack = true;
        }
        void modify(size_t _i, TrackItem _modified) {
            if(!__isInitialised) throw std::runtime_error("modify(size_t, TrackItem) : class instance has not been initialised.");
            if(!__running.load()) throw std::runtime_error("modify(size_t, TrackItem) : Tracking sub-thread is not runnng.");
            if(_i>__ItemsToTrack_updates.size()) throw std::out_of_range("modify(size_t, TrackItem) : _i out of range");

            // std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);

            __ItemsToTrack_updates.at(_i) = _modified;
            isChanged_ItemsToTrack = true;
        }
        void remove(size_t _i) {
            if(!__isInitialised) throw std::runtime_error("remove(size_t) : class instance has not been initialised.");
            if(!__running.load()) throw std::runtime_error("remove(size_t) : Tracking sub-thread is not runnng.");
            if(_i>__ItemsToTrack_updates.size()) throw std::out_of_range("remove(size_t) : _i out of range");

            // std::unique_lock<std::mutex> u_lck(__mtx_pauseThreadIteration);

            auto itr = __ItemsToTrack_updates.begin();
            std::advance(itr, _i);
            __ItemsToTrack_updates.erase(itr);
            isChanged_ItemsToTrack = true;
        }

        void startThread() {
            if(__isInitialised) throw std::runtime_error("startThread() : the member has already been called.");
            if(__running.load()) throw std::runtime_error("startThread() : thread is already running");
            if(!__isDefined__callbackFound) throw std::runtime_error("startThread() : Error. Cannot start the main tracking thread without the callback for signaling when a tracked item is found, is defined.");
            
            this->threadObj = std::thread(threadFunc_threadClass, std::ref(*this));
            __isInitialised = true;
        }
        void stopThread() {
            //if(!__isInitialised) throw std::runtime_error("close() : the object has not been initialised.");
            
            if(__running.load()) {
                __running = false;
                
                if(this->threadObj.joinable()) this->threadObj.join();
            }

        }

        
    };


};


/**
 * Type holder for the container that holds all items.
 * 
 * structure: `std::map<{item name}, itemInfo>`
 */
using type_AllItems = std::map<std::string, itemInfo>;


#endif //HPP_WMPT_TYPES
