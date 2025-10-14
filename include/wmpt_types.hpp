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

itemType find_itemType(std::string _typeStr) {
    itemType _type = itemType_default;

    if(_typeStr=="mod") {
        return itemType_Mods;
    }
    else if(_typeStr=="component") {
        return itemType_Component;
    }
    
    // throw CustomException("no valid type found");

    return _type;
}


struct itemInfo {
    std::string id;
    std::string name;

    itemType    type;
};

/**
 * Type holder for the container that holds all items.
 * 
 * structure: `std::map<{item name}, itemInfo>`
 */
using type_AllItems = std::map<std::string, itemInfo>;

#endif //HPP_WMPT_TYPES