/**
 * @file wmpt_main.cpp
 * @author B.Erkhme (b.erkhme@gmail.com)
 * @brief warframe.market price tracker where the user can select items to be notified of if the sell/buy price is at or beyond a custom value.
 * @version 1.0.0
 * @date 2025-10-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "wmpt_useful.hpp"
#include "wmpt_functions.hpp"
#include "wmpt_variables.hpp"
#include "wmpt_types.hpp"

#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

#include <Pos2d.hpp>


int main(int argc, char** argv) {
	Pos2d<int> terminalDim{0, 0};
	Useful::getTerminalSize(terminalDim.x, terminalDim.y);
	Useful::clearScreen();
	while (true) {
		// Useful::ANSI_mvprint(0, 0, "", false, "abs", "abs", true);
		TUI_drawBG();
		std::vector<int> pressedKeys = getKeyCode();
		std::string printStr = std::string("Reading key: ")+Useful::formatVector(pressedKeys);
		Useful::ANSI_mvprint(2, 2, printStr+std::string((printStr.size()<terminalDim.x-1? terminalDim.x-1-printStr.size() : 0), ' '), true, "abs", "abs");
		std::this_thread::sleep_for(std::chrono::duration<double>(0.1));
		if(pressedKeys.size()>0 && 'x'==static_cast<char>(pressedKeys.at(0))) break;
	}

	exit(0);

	/// ---------- Load all items ----------
	type_AllItems AllItems = LoadAllItems();

	std::vector<TrackItem> AllTrackedItems;


	float fpsCap = 60;
	std::chrono::steady_clock::time_point time_T1, time_T2;
	std::chrono::duration<double> dur_deltaT(0);
	std::chrono::duration<double> dur_deltaSet(double(1)/fpsCap);
	time_T2 = (time_T1 = std::chrono::steady_clock::now());
	/// ---------- Run main loop ----------
	TUI_drawBG();
	while(true) {
		/// ---------- select items to track ----------


		/// ---------- tracker loop ----------
		while(true) {


			dur_deltaT = (time_T2=std::chrono::steady_clock::now())-time_T1;

			if(dur_deltaT<dur_deltaSet) std::this_thread::sleep_for(dur_deltaSet-dur_deltaT);
			time_T1 = time_T2;
			TUI_drawBG();
		}
	}

    return 0;
}
