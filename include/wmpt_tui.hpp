#pragma once
#ifndef HPP_WMPT_TUI
#define HPP_WMPT_TUI

#include <mutex>
#include <thread>
#include <atomic>

#include <string>

#include <initializer_list>
#include <vector>
#include <functional>
#include <map>

#include "wmpt_useful.hpp"
#include <Pos2d.hpp>



namespace TUINC {

    enum Results {
        Success,
        ThreadInitialisationFailure,
        ToastClicked,             // user clicked on the toast
        ToastDismissed,           // user dismissed the toast
        ToastTimeOut,             // toast timed out
        ToastHided,               // application hid the toast
        ToastNotActivated,        // toast was not activated
        ToastFailed,              // toast failed
        SystemNotSupported,       // system does not support toasts
        UnhandledOption,          // unhandled option
        MultipleTextNotSupported, // multiple texts were provided
        InitialisationFailure,    // toast notification manager initialization failure
        ToastNotLaunched          // toast could not be launched
        
    };

    std::string __str_printBuffer;
    // std::string __str_printBuffer_trackedOffers;
    struct TrackItemOffers {
        PriceTracker::TrackItem trackItem;
        std::vector<PriceTracker::ItemOffer> trackOffers;
    };
    /**
     * A std::map with TrackID std::string as key and a custom struct holding PriceTracker::TrackItem and std::vector<PriceTracker::ItemOffer> as value.
     * 
     */
    std::map<std::string, TrackItemOffers> TrackingItemsOffers;
    std::mutex __mtx_access_TrackingItemsOffers;

    std::atomic<char> settingTUI__charColumnDelim{'|'};

    std::atomic<int> settingTUI__charWidth_itemName{15};
    std::atomic<int> settingTUI__charWidth_trackingRank{3};
    std::atomic<int> settingTUI__charWidth_trackingPrice{3};
    std::atomic<int> settingTUI__charWidth_currrentBestPrice{3};
    

    extern PriceTracker::callbackType_trackedAllOffers callbackFunc_trackedAllOffers;
    extern PriceTracker::callbackType_trackedFound callbackFunc_offersFound;


    extern PriceTracker::threadClass threadObj_PriceTracker;


    Results Initialise();

    int Drive();

    
    struct  __cell;
    class   __table;

    using __type_cellFunc = std::function<void(__table&)>;


    enum cell_types {
        null    = 0,
        text    = 1,
        function= 2
    };

    struct __cell_singleTempHolder {

    };

    struct __cellType_null {

    };
    struct __cellType_text {

        std::string text;

        bool rule_followDelimiter{true};
    };
    struct __cellType_function {
        std::string text;
        
        __type_cellFunc function;

    };

    class  __cell {
        private:
        // std::string     label;
        // __type_cellFunc function;
        // Pos2d<int>      pos;
        // bool    __nullCell  = false;
        // bool    __isDefined_label   = false;
        // bool    __isDefined_function= false;
        // bool    __isDefined_pos     = false;
        
            
        std::string     __table_ID;
        cell_types      __cellType{null};
        Pos2d<int>      __pos;

        __cellType_null     __cellContent_null;
        __cellType_text     __cellContent_text;
        __cellType_function __cellContent_function;

        public:
    
        //__cell() = delete; //?
        __cell(cell_types _cellType=cell_types::null);
        __cell(std::string _text, cell_types _cellType=cell_types::text);
        __cell(std::string _text, __type_cellFunc _func);
        __cell(__type_cellFunc _func);
        
        __cell(const __cell& _toCopy);
        __cell(__cell&& _toMove);
        ~__cell();
        __cell& operator=(const __cell& _toCopy);
        __cell& operator=(__cell&& _toMove);
        
        
        Pos2d<int>& pos();
        Pos2d<int>  pos() const;
        
        std::string& text();
        std::string  text() const;
        

        // __cell() = delete; // ?
        // __cell(bool _nullCell=true);
        // __cell(std::string _label);
        // __cell(std::string _label, Pos2d<int> _pos);
        // __cell(std::string _label, __type_cellFunc _function);
        // __cell(std::string _label, __type_cellFunc _function, Pos2d<int> _pos);
        // __cell(const __cell& _toCopy);
        // __cell(__cell&& _toMove);
        // ~__cell();
        // __cell& operator=(const __cell& _toCopy);
        // __cell& operator=(__cell&& _toMove);
    };

    class   __table_row {
        private:
        std::vector<__cell>& __tableCellRow;

        public:
        __table_row(std::vector<__cell>& _table_row);
        __table_row() = delete;
        __table_row(const __table_row& _toCopy) = delete;
        __table_row(__table_row&& _toMove);
        ~__table_row();
        __table_row& operator=(const __table_row& _toCopy) = delete;
        __table_row& operator=(__table_row&& _toMove);

        __cell& operator[](size_t _i);
        __cell  operator[](size_t _i) const;
        __cell& at(size_t _i);
        __cell  at(size_t _i) const;

        size_t size();

    };
    enum style_columnWidthPriority {
        prio_cellWidth,
        prio_fitMenuWidth
    };
    
    class   __table {
        private:
        /**
         * access index: [row][column] ([y][x])
         *  {
         *      {[0, 0], [0, 1], [0, 2]},
         *      {[1, 0], [1, 1], [1, 2]}
         *  }
         */
        std::vector<std::vector<__cell>> __tableOfCells;


        // --- graphics settings ---
        
        /// @brief Maximum character-size dimensions of table.
        Pos2d<size_t> __dimSize_table{std::string::npos, std::string::npos};

        /// @brief Maximum character-size widths of each column
        std::vector<size_t> __size_columnWidths;
        /// @brief Maximum character-size heights of each row
        std::vector<size_t> __size_rowHeights;

        
        void __update__string_table();
        /// @brief Final string containing the table in full
        std::string __string_table{""};

        style_columnWidthPriority widthPrio{style_columnWidthPriority::prio_fitMenuWidth};


        std::string delimiter_columns{"|"};
        std::string delimiter_rows{" "};

        public:
        __table(std::initializer_list<std::initializer_list<__cell>> _matrixInput);

        __table();
        __table(const __table& _toCopy);
        __table(__table&& _toMove);
        ~__table();
        __table& operator=(const __table& _toCopy);
        __table& operator=(__table&& _toMove);
        __table& operator=(std::initializer_list<std::initializer_list<__cell>> _matrixInput);
        
        __table_row& operator[](size_t _i);
        // __table_row  operator[](size_t _i) const;
        __table_row& at(size_t _i);
        // __table_row  at(size_t _i) const;

        __cell& cell(size_t _x, size_t _y);
        __cell  cell(size_t _x, size_t _y) const;
        __cell& cell(Pos2d<int> _pos);
        __cell  cell(Pos2d<int> _pos) const;

        size_t rows();
        size_t columns();
        size_t size();
        
        std::vector<__cell> continuous();
        std::vector<__cell> nonNull_continuous();

        // int addCell(size_t _x, size_t _y);
        // int addCell(Pos2d<int> _pos);
        // int moveCell(size_t _current_x, size_t _current_y, size_t _new_x, size_t _new_y);
        // int moveCell(Pos2d<int> _current_pos, Pos2d<int> _new_pos);
        // int eraseCell(size_t _x, size_t _y);
        // int eraseCell(Pos2d<int> _pos);

        Results call(size_t _x, size_t _y);
        Results call(Pos2d<int> _pos);

    };

};


#endif //HPP_WMPT_TUI
