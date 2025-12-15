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

    /// ---------- custom independant library stuff ---------
    
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

    class cell;
    class table;
    class menu;

    using type_cellFunc = std::function<void(table*)>;


    enum cell_types {
        null    = 0,
        text    = 1,
        function= 2,
        menuPtr = 3
    };

    struct cell_singleTempHolder {
        
    };

    struct cellTypeContent_null {

    };
    struct cellTypeContent_text {
        bool rule_followDelimiter{true};
    };
    struct cellTypeContent_function {

    };
    struct cellTypeContent_menuPtr {
        
    };

    class  cell {
        private:
        
        table*    tablePtr{nullptr};

        cell_types      cellType{null};
        Pos2d<int>      pos;

        std::string     text;
        type_cellFunc   function;
        menu*           menuPtr;
        

        cellTypeContent_null        cellContent_null;
        cellTypeContent_text        cellContent_text;
        cellTypeContent_function    cellContent_function;
        cellTypeContent_menuPtr     cellContent_menuPtr;

        bool isDefined__pos{false};
        bool isDefined__text{false};
        bool isDefined__function{false};
        bool isDefined__menuPtr{false}; //this way of checking definition is currently used just in case it's changed from pointer later on.'

        bool isModified__tablePtr{true};
        bool isModified__cellType{true};
        bool isModified__pos{true};
        bool isModified__text{true};
        bool isModified__function{true};
        bool isModified__menuPtr{true};
        
        void cell_resetModificationFlag();

        friend class table;
        friend class menu;


        public:
    
        
        cell(cell_types _cellType=cell_types::null);
        cell(std::string _text, cell_types _cellType=cell_types::text);
        cell(std::string _text, type_cellFunc _func, cell_types _cellType=cell_types::function);
        cell(std::string _text, menu* _menuPtr, cell_types _cellType=cell_types::menuPtr);
        
        cell(const cell& _toCopy);
        cell(cell&& _toMove);
        ~cell();
        cell& operator=(const cell& _toCopy);
        cell& operator=(cell&& _toMove);
        
        void call();
        
        int set_tablePtr(table* _tablePtr);
        int set_pos(Pos2d<int> _pos);
        int set_text(std::string _text);
        int set_function(type_cellFunc _func);
        int set_menuPtr(menu* menuPtr);
        
        int setContent_null(cellTypeContent_null _newContent);
        int setContent_text(cellTypeContent_text _newContent);
        int setContent_function(cellTypeContent_function _newContent);
        int setContent_menuPtr(cellTypeContent_menuPtr _newContent);


        void change_type(cell_types _newType);
        void change_type(cell_types _newType, std::string _text);
        void change_type(cell_types _newType, std::string _text, type_cellFunc _func);
        void change_type(cell_types _newType, type_cellFunc _func);
        void change_type(cell_types _newType, std::string _text, menu* _menuPtr);
        void change_type(cell_types _newType, menu* _menuPtr);

        table*          get_tablePtr() const;
        Pos2d<int>      get_pos() const;
        std::string     get_text() const;
        type_cellFunc   get_function() const;
        menu*           get_menuPtr() const;
        cellTypeContent_null    get_cellContent_null() const;
        cellTypeContent_text    get_cellContent_text() const;
        cellTypeContent_function  get_cellContent_function() const;
        cellTypeContent_menuPtr get_cellContent_menuPtr() const;
        cell_types get_type() const;

        bool isModified();
        bool isModified_tablePtr();
        bool isModified_pos();
        bool isModified_text();
        bool isModified_function();
        bool isModified_menuPtr();
        
    };


    enum class style_axisCellScalingMethod {
        cellWidth,      // scale every column accordingly to their longest cell and likely go out of bounds or vice-versa to the TUI menu screen width.
        fitMenuAxis    // scale every column so they fit within the menu's axis evenly. Can cause clipping of text between column delimiters.
    };
    
    class table {
        protected:
        /**
         * access index: [row][column] ([y][x])
         *  {
         *      {[0, 0], [0, 1], [0, 2]},
         *      {[1, 0], [1, 1], [1, 2]}
         *  }
         */
        std::vector<std::vector<cell>> tableOfCells;
        
        /**
         * Character-size max dimensions of table. -1 means use dimensions of terminal
         * 
         */
        Pos2d<int> dimSize_table{-1, -1};

        /// @brief Maximum character-size widths of each column
        std::vector<size_t> size_columnWidths;
        /// @brief Maximum character-size heights of each row
        std::vector<size_t> size_rowHeights;

        
        void func_loadInitialiserCellMatrix(std::initializer_list<std::initializer_list<cell>> _matrixInput);

        void helper_updateTablePtrInCells();
        
        std::vector<std::string> help__separateLines(std::string _toSep, std::string _delim="\n");
        /**
         * 
         * 
         */
        void update__string_table();
        /// @brief Final string containing the table in full
        std::string string_table{""};

        style_axisCellScalingMethod scalMethod_columns{style_axisCellScalingMethod::fitMenuAxis};
        style_axisCellScalingMethod scalMethod_rows{style_axisCellScalingMethod::fitMenuAxis};

        std::string delimiter_columns{"|"};
        std::string delimiter_rows{"-"};

        ///for now we ignore specific corner symbols
        std::string borderSymb_column{"|"};
        std::string borderSymb_row{"-"};
        std::string rowSeparator{"\n"};
        

        public:

        table(std::initializer_list<std::initializer_list<cell>> _matrixInput);

        table();
        table(const table& _toCopy);
        table(table&& _toMove);
        ~table();
        table& operator=(const table& _toCopy);
        table& operator=(table&& _toMove);
        table& operator=(std::initializer_list<std::initializer_list<cell>> _matrixInput);
        
        std::vector<cell>& operator[](size_t _i);
        std::vector<cell>  operator[](size_t _i) const;
        std::vector<cell>& at(size_t _i);
        std::vector<cell>  at(size_t _i) const;

        cell& get_cell(size_t _x, size_t _y);
        cell  get_cell(size_t _x, size_t _y) const;
        cell& get_cell(Pos2d<int> _pos);
        cell  get_cell(Pos2d<int> _pos) const;

        size_t rows();
        size_t columns();
        size_t size();
        
        std::vector<cell> continuous(bool _includeNullCells=true);

        // int addCell(size_t _x, size_t _y);
        // int addCell(Pos2d<int> _pos);
        // int moveCell(size_t _current_x, size_t _current_y, size_t _new_x, size_t _new_y);
        // int moveCell(Pos2d<int> _current_pos, Pos2d<int> _new_pos);
        // int eraseCell(size_t _x, size_t _y);
        // int eraseCell(Pos2d<int> _pos);

        Results call(size_t _x, size_t _y);
        Results call(Pos2d<int> _pos);

    };

    /**
     * A user defiend type that holds the info of the menu of the current menu window consisting of table and cell user defiend type.
     * can be called from cell type.
     */
    class menu: public table {
        private:

        Pos2d<int> pos_selectedCell;
        

        public:

        menu(std::initializer_list<std::initializer_list<cell>> _matrixInput);
        menu(const table& _tableToCopy);
        menu(table&& _tableToCopy);
        
        menu();
        menu(const menu& _toCopy);
        menu(menu&& _toMove);
        ~menu();
        menu& operator=(const menu& _toCopy);
        menu& operator=(menu&& _toMove);
            
        void Driver();
    
    };


    /// ---------- Project specific functions and variables ----------

    
    std::string str_printBuffer;
    
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


};


#endif //HPP_WMPT_TUI
