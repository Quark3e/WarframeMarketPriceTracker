
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
    std::unique_lock<std::mutex> u_lck_accss_callbackAllOffers(_refObj.__mtx_access_callbackAllOffers,  std::defer_lock);
    std::unique_lock<std::mutex> u_lck_pauseThreadIteration(_refObj.__mtx_pauseThreadIteration,         std::defer_lock);
    
    apiCallTimer callTimer;

    assert(_refObj.__isDefined__callbackFound);

    while(_refObj.__running) {

        u_lck_pauseThreadIteration.lock();
        for(TrackItem _item : _refObj.__ItemsToTrack) {
            std::string url_item = apiURL_base+"orders/item/"+_item.name;
            std::string str_itemGET;
            json json_itemListings;
            std::vector<ItemOffer> itemOffers_found;
            std::vector<ItemOffer> itemOffers_allOffers;
            try {

                // str_itemGET = http_get(url_item);
                callTimer.call(url_item);
                json_itemListings = json::parse(str_itemGET);

                /// iterate over every offer for current `_item`
                for(auto itr=json_itemListings.at("data").begin(); itr!=json_itemListings.at("data").end(); ++itr) {
                    
                    ItemOffer _offer;
                    _offer.id   = itr->at("id");
                    _offer.type = (itr->at("type")=="sell"? trackType_sell : trackType_buy);
                    _offer.platinum = itr->at("platinum").get<int>();
                    _offer.quantity = itr->at("quantity").get<int>();
                    _offer.rank = (itr->contains("rank")? itr->at("rank").get<int>() : -1);
                    _offer.date_createdAt   = itr->at("createdAt");
                    _offer.date_updatedAt   = itr->at("updatedAt");
                    _offer.item_id  = itr->at("itemId");

                    UserInfo _user;
                    auto userJson = itr->at("user");
                    _user.id    = userJson.at("id");
                    _user.ingameName= userJson.at("ingameName");
                    _user.name_slug = userJson.at("slug");
                    _user.reputation= userJson.at("reputation").get<int>();
                    _user.status    = get_userStatus_fromStr(userJson.at("status"));
                    _user.date_lastSeen = userJson.at("lastSeen");

                    _offer.user = _user;


                    itemOffers_allOffers.push_back(_offer);
                    if(_item.type!=_offer.type) continue;

                    struct foundOfferChecks {
                        bool rank_min = true;
                        bool rank_max = true;
                        bool plat_min = true;
                        bool plat_max = true;

                        operator bool() {
                            return rank_min && rank_max && plat_min && plat_max;
                        }
                    } quickCheck;

                    if(_item.rank_min!=-1 && _offer.rank<_item.rank_min)    quickCheck.rank_min = false;
                    if(_item.rank_max!=-1 && _offer.rank>_item.rank_max)    quickCheck.rank_max = false;
                    if(_item.plat_min!=-1 && _offer.platinum<_item.plat_min)    quickCheck.plat_min = false;
                    if(_item.plat_max!=-1 && _offer.platinum>_item.plat_max)    quickCheck.plat_max = false;

                    if(!quickCheck) continue;

                    itemOffers_found.push_back(_offer);

                }

                u_lck_accss_callbackAllOffers.lock();
                _refObj.__callbackAllOffers(_item, itemOffers_allOffers);
                u_lck_accss_callbackAllOffers.unlock();
                u_lck_accss_callbackFound.lock();
                _refObj.__callbackFound(_item, itemOffers_found);
                u_lck_accss_callbackFound.unlock();

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
