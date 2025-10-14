#pragma once
#ifndef HPP_WMPT_FUNCTIONS
#define HPP_WMPT_FUNCTIONS


#include <string>
#include <curl/curl.h>


// Callback function to write data to string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

std::string http_get(const std::string& url);


#include "wmpt_variables.hpp"
#include "wmpt_types.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

type_AllItems LoadAllItems() {
    std::string urlString = apiURL_base + apiURL_addon__allItems;

    json parsedJson;
    type_AllItems parsedItems;

    try {
        std::string getStr = http_get(urlString);

        parsedJson = json::parse(getStr);
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        exit(1);
    }
    

    for(auto itr=parsedJson["data"].begin(); itr!=parsedJson["data"].end(); ++itr) {
        std::string _id=itr->at("id"), _key=itr->at("slug");
        
        itemType _type = itemType_default;
        for(auto tagItr=itr->begin(); tagItr!=itr->end(); ++tagItr) {
            _type = find_itemType(tagItr.key());
        }

        parsedItems[itr->at("slug")] = {_id, _key, _type};
    }


    return parsedItems;
}


#endif // HPP_WMPT_FUNCTIONS