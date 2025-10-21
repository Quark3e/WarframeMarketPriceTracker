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

#include "wmpt_useful.hpp"

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

enum trackType {
    trackType_buy,
    trackType_sell
};
struct TrackItem {
    std::string id;
    std::string name;

    trackType   typeOfTrack;

    int plat_min;
    int plat_max;
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
    std::chrono::duration<double> pressTypeDifrDecayDur_seconds = std::chrono::duration<double>(0.5);

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


/**
 * Type holder for the container that holds all items.
 * 
 * structure: `std::map<{item name}, itemInfo>`
 */
using type_AllItems = std::map<std::string, itemInfo>;


#endif //HPP_WMPT_TYPES