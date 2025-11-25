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

    
    class __cell;
    class __table_row;
    class __table;

    using __type_cellFunc = std::function<void(__table*)>;


    enum cell_types {
        null    = 0,
        text    = 1,
        function= 2
    };

    struct __cell_singleTempHolder {

    };

    struct __cellTypeContent_null {

    };
    struct __cellTypeContent_text {
        bool rule_followDelimiter{true};
    };
    struct __cellTypeContent_function {

    };

    class  __cell {
        private:
            
        __table*        __tablePtr{nullptr};
        /**
         * Pointer to the table_row class this cell is stored in.
         * 
         */
        __table_row*    __table_rowPtr{nullptr};
        cell_types      __cellType{null};
        Pos2d<int>      __pos;

        std::string     __text;
        __type_cellFunc __function;

        __cellTypeContent_null     __cellContent_null;
        __cellTypeContent_text     __cellContent_text;
        __cellTypeContent_function __cellContent_function;

        bool __isDefined__pos{false};
        bool __isDefined__text{false};
        bool __isDefined__function{false};

        // bool __isDefined__cellContent_null{false};
        // bool __isDefined__cellContent_text{false};
        // bool __isDefined__cellContent_function{false};

        bool __isModified__tablePtr{true};
        bool __isModified__table_rowPtr{true};
        bool __isModified__cellType{true};
        bool __isModified__pos{true};
        bool __isModified__text{true};
        bool __isModified__function{true};

        public:
    
        //__cell() = delete; //?
        
        __cell(cell_types _cellType=cell_types::null);
        __cell(std::string _text, cell_types _cellType=cell_types::text);
        __cell(std::string _text, __type_cellFunc _func, cell_types _cellType=cell_types::function);
        
        __cell(const __cell& _toCopy);
        __cell(__cell&& _toMove);
        ~__cell();
        __cell& operator=(const __cell& _toCopy);
        __cell& operator=(__cell&& _toMove);
        
        void call();
        
        int set_tablePtr(__table* _tablePtr);
        int set_table_rowPtr(__table_row* _table_rowPtr);
        int set_pos(Pos2d<int> _pos);
        int set_text(std::string _text);
        int set_function(__type_cellFunc _func);
        int setContent_null(__cellTypeContent_null _newContent);
        int setContent_text(__cellTypeContent_text _newContent);
        int setContent_function(__cellTypeContent_function _newContent);

        void change_type(cell_types _newType);
        void change_type(cell_types _newType, std::string _text);
        void change_type(cell_types _newType, std::string _text, __type_cellFunc _func);
        void change_type(cell_types _newType, __type_cellFunc _func);

        __table*        get_tablePtr() const;
        __table_row*    get_table_rowPtr() const;
        Pos2d<int>      get_pos() const;
        std::string     get_text() const;
        __type_cellFunc get_function() const;
        __cellTypeContent_null  get_cellContent_null() const;
        __cellTypeContent_text  get_cellContent_text() const;
        __cellTypeContent_function  get_cellContent_function() const;
        cell_types get_type() const;

        void resetModificationFlag();
        bool isModified();
        bool isModified_tablePtr();
        bool isModified_table_rowPtr();
        bool isModified_pos();
        bool isModified_text();
        bool isModified_function();
        
    };

    class   __table_row {
        private:
        __table* __tablePtr{nullptr};
            
        std::vector<__cell>& __tableCellRow;

        std::vector<bool> __isModified;

        public:
        __table_row(std::initializer_list<__cell> _table_row);
        __table_row(std::vector<__cell> _table_row);
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

        int set_tablePtr(__table* _tablePtr);
        
        __table* get_tablePtr();
        
        size_t size();
        
        void resetModificationFlag(int _i=-1);
        /// check every cell for the coordinate
        void checkRow(bool _correctPos=true);

        bool isModified(int _i=-1);

    };
    enum style_axisCellScalingMethod {
        cellWidth,      // scale every column accordingly to their longest cell and likely go out of bounds or vice-versa to the TUI menu screen width.
        fitMenuAxis    // scale every column so they fit within the menu's axis evenly. Can cause clipping of text between column delimiters.
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
        std::vector<__table_row> __tableOfCells;


        // --- graphics settings ---
        
        /**
         * Character-size dimensions of table. -1 means use dimensions of terminal
         * 
         */
        Pos2d<int> __dimSize_table{-1, -1};

        /// @brief Maximum character-size widths of each column
        std::vector<size_t> __size_columnWidths;
        /// @brief Maximum character-size heights of each row
        std::vector<size_t> __size_rowHeights;

        std::vector<std::string> __help__separateLines(std::string _toSep, std::string _delim="\n");
        void __update__string_table();
        /// @brief Final string containing the table in full
        std::string __string_table{""};

        style_axisCellScalingMethod __scalMethod_columns{style_axisCellScalingMethod::fitMenuAxis};
        style_axisCellScalingMethod __scalMethod_rows{style_axisCellScalingMethod::fitMenuAxis};

        std::string __delimiter_columns{"|"};
        std::string __delimiter_rows{"-"};

        ///for now we ignore specific corner symbols
        std::string __borderSymb_column{"|"};
        std::string __borderSymb_row{"-"};
        std::string __rowSeparator{"\n"};

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
