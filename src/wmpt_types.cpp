
#include "wmpt_types.hpp"
#include "wmpt_functions.hpp"

itemType find_itemType(std::string _typeStr) {
    itemType _type = itemType_default;


    if(_typeStr=="mod") {
        return itemType_Mods;
    }
    else if(_typeStr=="component") {
        return itemType_Component;
    }
    else if(_typeStr=="set") {
        return itemType_Set;
    }
    
    // throw CustomException("no valid type found");

    return _type;
}

void PriceTracker::threadFunc_threadClass(threadClass& _refObj) {
    std::unique_lock<std::mutex> u_lck_accss_ItemsToTrack(_refObj.__mtx_access_ItemsToTrack,            std::defer_lock);
    std::unique_lock<std::mutex> u_lck_accss_callbackFound(_refObj.__mtx_access_callbackFound,          std::defer_lock);
    std::unique_lock<std::mutex> u_lck_accss_callbackClosestNF(_refObj.__mtx_access_callbackClosestNF,  std::defer_lock);
    std::unique_lock<std::mutex> u_lck_pauseThreadIteration(_refObj.__mtx_pauseThreadIteration,         std::defer_lock);

    assert(_refObj.__isDefined__callbackFound);

    while(_refObj.__running) {

        u_lck_pauseThreadIteration.lock();
        for(TrackItem _item : _refObj.__ItemsToTrack) {
            std::string url_item = apiURL_base+"orders/item/"+_item.name;
            std::string str_itemGET;
            json json_itemListings;
            std::vector<ItemOffer> itemOffers_found;
            std::vector<ItemOffer> itemOffers_closestNF;
            try {
                str_itemGET = http_get(url_item);
                json_itemListings = json::parse(str_itemGET);

                
            }
            catch(const std::exception& e) {
                std::cerr << e.what() << '\n';
                continue;
            }
            


        }
        u_lck_pauseThreadIteration.unlock();
    }
    _refObj.__running = false;
}
