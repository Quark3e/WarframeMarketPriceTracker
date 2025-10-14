#pragma once
#ifndef HPP_WMPT_FUNCTIONS
#define HPP_WMPT_FUNCTIONS


#include <string>
#include <curl/curl.h>


// Callback function to write data to string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

std::string http_get(const std::string& url);






#endif // HPP_WMPT_FUNCTIONS