
#include "wmpt_functions.hpp"


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}
std::string http_get(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string response;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        
        // Perform the request
        res = curl_easy_perform(curl);
        
        // Check for errors
        if (res != CURLE_OK) {
            response = "Error: " + std::string(curl_easy_strerror(res));
        }
        
        // Always cleanup
        curl_easy_cleanup(curl);
    } else {
        response = "Error: Failed to initialize CURL";
    }
    
    curl_global_cleanup();
    return response;
}

std::vector<int> getKeyCode(bool _verbose) {
    std::vector<int> pressed_keyCodes;
    for (int keyCode=0; keyCode < 256; ++keyCode) {
        // Check if the key with keyCode is currently
        // pressed
        if (GetAsyncKeyState(keyCode) & 0x8000) {
            // Convert the key code to ASCII character
            char keyChar = static_cast<char>(keyCode);
            pressed_keyCodes.push_back(keyCode);
            
            if(_verbose) std::cout << "Pressed Key: " << keyChar << " (ASCII value: " << keyCode << ")" << std::endl;
        }
    }

    // Add a small delay to avoid high CPU usage
    // Sleep(100);
    return pressed_keyCodes;
}


void TUI_drawBG() {
    Pos2d<int> _dimTerminal(0, 0);
    Useful::getTerminalSize(_dimTerminal.x, _dimTerminal.y);

	Useful::ANSI_mvprint(1, 1, "1", false, "abs", "abs", false);
	Useful::ANSI_mvprint(2, 1, std::string(_dimTerminal.x-2, '-'), false);
	Useful::ANSI_mvprint(_dimTerminal.x, 1, "2", false);
    for(size_t y=2; y<_dimTerminal.y; y++) {
        Useful::ANSI_mvprint(1, y, "|", false);
        Useful::ANSI_mvprint(_dimTerminal.x, y, "|", false);
    }
    Useful::ANSI_mvprint(1, _dimTerminal.y, "3", false);
	Useful::ANSI_mvprint(2, _dimTerminal.y, std::string(_dimTerminal.x-2, '-'), false);
    Useful::ANSI_mvprint(_dimTerminal.x, _dimTerminal.y, "4", false);
}


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



