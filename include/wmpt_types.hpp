#pragma once
#ifndef HPP_WMPT_TYPES
#define HPP_WMPT_TYPES


#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <chrono>


#include <stdexcept>


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
};
itemType find_itemType(std::string _typeStr);


struct itemInfo {
    std::string id;
    std::string name;

    itemType    type;
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
};
class keyPressHandler {
private:

    // std::vector<int> __keys_pressed;
    // std::vector<std::chrono::steady_clock::time_point> __keys_timePressed;
    std::unordered_map<int, __keyPressHandler_keyDetails> __pressed_keys;

    std::vector<int> __active_keys;

public:
    std::chrono::duration<double> pressTypeDifrDecayDur_seconds = std::chrono::duration<double>(0.5);

    keyPressHandler() {
        for(size_t i=0; i<256; i++) {
            __pressed_keys[i] = {
                std::chrono::steady_clock::time_point::max(),
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
        auto time_now = std::chrono::steady_clock::now();

        for(size_t i=0; i<256; i++) __pressed_keys[i].isPressed = false;
        for(size_t i=0; i<newKeys.size(); i++) {
            __pressed_keys[i].isPressed = true;
            if(__pressed_keys[newKeys.at(i)].startTime==std::chrono::steady_clock::time_point::max()) {
                __pressed_keys[newKeys.at(i)].startTime = time_now;
                // __pressed_keys[newKeys.at(i)].active = true;
            }
            else {

            }
        }
        __active_keys.clear();
        for(size_t i=0; i<256; i++) {
            __keyPressHandler_keyDetails& keyRef = __pressed_keys[i];

            if(!keyRef.isPressed) {
                keyRef.startTime = std::chrono::steady_clock::time_point::max();
                keyRef.active = false;
                continue;
            }

            if(keyRef.startTime==time_now || (time_now-keyRef.startTime)>pressTypeDifrDecayDur_seconds) {
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

};


/**
 * Type holder for the container that holds all items.
 * 
 * structure: `std::map<{item name}, itemInfo>`
 */
using type_AllItems = std::map<std::string, itemInfo>;


#endif //HPP_WMPT_TYPES