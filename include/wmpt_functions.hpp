#pragma once
#ifndef HPP_WMPT_FUNCTIONS
#define HPP_WMPT_FUNCTIONS


#include <string>
#include <vector>
#include <functional>
#include <curl/curl.h>
#include <iostream>
#include <Windows.h>

#include <Pos2d.hpp>
#include "wmpt_useful.hpp"



// Callback function to write data to string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
std::string http_get(const std::string& url);


/**
 * @brief Retrieves a vector of key codes from the user input.
 *
 * This function prompts the user to enter key codes and returns them as a vector of integers.
 * 
 * @param _verbose If set to true, the function provides detailed output about the process.
 *                 Defaults to false.
 * @return std::vector<int> A vector containing the entered key codes.
 */
std::vector<int> getKeyCode(bool _verbose=false);


void TUI_drawBG();


#include "wmpt_variables.hpp"
#include "wmpt_types.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

type_AllItems LoadAllItems();


#endif // HPP_WMPT_FUNCTIONS