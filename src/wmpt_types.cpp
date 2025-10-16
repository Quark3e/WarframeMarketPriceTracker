
#include "wmpt_types.hpp"

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
