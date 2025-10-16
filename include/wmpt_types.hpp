#pragma once
#ifndef HPP_WMPT_TYPES
#define HPP_WMPT_TYPES


#include <string>
#include <map>

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


/**
 * Type holder for the container that holds all items.
 * 
 * structure: `std::map<{item name}, itemInfo>`
 */
using type_AllItems = std::map<std::string, itemInfo>;


#endif //HPP_WMPT_TYPES