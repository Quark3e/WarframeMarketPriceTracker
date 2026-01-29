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

    namespace core {
        class Cell;
        class Table;
        class Menu;
    };

    using type_cellFunc = std::function<void(core::Table*)>;

    /// @brief Retrieves the current console window dimensions.
    /// @return A Pos2d<size_t> object containing the console width and height.
    /// @details This helper function queries the console window to determine its current
    ///          size in terms of columns (width) and rows (height).
    Pos2d<size_t> helper_getConsoleDimensions();

    enum class cell_types {
        null    = 0,
        text    = 1,
        function= 2,
        menu    = 3
    };

    // struct cell_singleTempHolder {
    // };

    struct cellTypeContent_null {

    };
    struct cellTypeContent_text {
        bool rule_followDelimiter{true};
    };
    struct cellTypeContent_function {

    };
    struct cellTypeContent_menu {
        
    };

    class  core::Cell {
        private:
        
        Table*  tablePtr{nullptr};

        cell_types      cellType{null};
        Pos2d<int>      pos;

        // std::string     text;

        /**
         * Container for the text that's stored in the cell. Each element/string represents one line of the total string box,
         *  meaning no newline characters are allowed
         */
        std::vector<std::string>    text;
        type_cellFunc   function;
        Menu            menu;      //cell stores a menu object entirely in itself
        

        cellTypeContent_null        cellContent_null;
        cellTypeContent_text        cellContent_text;
        cellTypeContent_function    cellContent_function;
        cellTypeContent_menu     cellContent_menu;

        bool isDefined__pos{false};
        bool isDefined__text{false};
        bool isDefined__function{false};
        bool isDefined__menu{false}; //this way of checking definition is currently used just in case it's changed from pointer later on.'

        bool isModified__tablePtr{true};
        bool isModified__cellType{true};
        bool isModified__pos{true};
        bool isModified__text{true};
        bool isModified__function{true};
        bool isModified__menu{true};
        
        void cell_resetModificationFlag();

        friend class Table;
        friend class Menu;

        /**
         * @brief Parses a string and splits it into multiple strings at newline characters.
         * 
         * @param _fullString The input string to be parsed for newline characters.
         * 
         * @return A vector of strings, where each element represents a line from the input
         *         string that was separated by newline characters.
         */
        std::vector<std::string> _helper_parseStringForNewlines(std::string _fullString);

        public:
    
        
        Cell(cell_types _cellType=cell_types::null);
        Cell(std::string _text, cell_types _cellType=cell_types::text);
        Cell(std::string _text, type_cellFunc _func, cell_types _cellType=cell_types::function);
        Cell(std::string _text, Menu&& _menu, cell_types _cellType=cell_types::menu);
        
        Cell(const Cell& _toCopy);
        Cell(Cell&& _toMove);
        ~Cell();
        Cell& operator=(const Cell& _toCopy);
        Cell& operator=(Cell&& _toMove);
        
        void call();
        
        int set_tablePtr(Table* _tablePtr);
        int set_pos(Pos2d<int> _pos);
        int set_text(std::string _text);
        int set_function(type_cellFunc _func);
        int set_menu(Menu&& _menu);
        
        int setContent_null(cellTypeContent_null _newContent);
        int setContent_text(cellTypeContent_text _newContent);
        int setContent_function(cellTypeContent_function _newContent);
        int setContent_menu(cellTypeContent_menu _newContent);


        void change_type(cell_types _newType);
        void change_type(cell_types _newType, std::string _text);
        void change_type(cell_types _newType, std::string _text, type_cellFunc _func);
        void change_type(cell_types _newType, type_cellFunc _func);
        void change_type(cell_types _newType, std::string _text, Menu&& _menu);
        void change_type(cell_types _newType, Menu&& _menu);

        Table*          get_tablePtr() const;
        Pos2d<int>      get_pos() const;
        std::string     get_text() const;
        type_cellFunc   get_function() const;
        Menu            get_menu() const;
        cellTypeContent_null    get_cellContent_null() const;
        cellTypeContent_text    get_cellContent_text() const;
        cellTypeContent_function    get_cellContent_function() const;
        cellTypeContent_menu    get_cellContent_menu() const;
        cell_types  get_type() const;

        bool isModified();
        bool isModified_tablePtr();
        bool isModified_pos();
        bool isModified_text();
        bool isModified_function();
        bool isModified_menu();
        
    };


    enum class style_axisCellScalingMethod {
        cellWidth,      // scale every cell_axis_size accordingly to their longest core::cell and likely have the axis go out of bounds or vice-versa to the TUI core::menu screen width. NOTE: will not include axes with only null cells
        fitMenuAxis_even,       // scale every cell_axis_size so they fit within the core::menu's axis evenly. Can cause clipping of text between column delimiters.
        // fitMenuAXis_adjusted    // scale every cell_axis_size so they fit within the core::menu's axis accordingly to an ratio of the size of each cell for that axis.
    };
    
    class core::Table {
        protected:
        /**
         * access index: [row][column] ([y][x])
         *  {
         *      {[0, 0], [0, 1], [0, 2]},
         *      {[1, 0], [1, 1], [1, 2]}
         *  }
         */
        std::vector<std::vector<Cell>> tableOfCells;
        
        /**
         * Character-size max dimensions of core::table. -1 means use dimensions of terminal
         * 
         */
        Pos2d<int> dimSize_table{-1, -1};

        /// @brief Maximum character-size widths of each column
        std::vector<size_t> maxSize_columnWidths;
        /// @brief Maximum character-size heights of each row
        std::vector<size_t> maxSize_rowHeights;

        
        /// \brief Loads a 2D matrix of cells from an initializer list.
        /// 
        /// This function populates the internal cell matrix using the provided
        /// nested initializer list. Each inner initializer list represents a row
        /// of cells in the matrix.
        /// 
        /// \param _matrixInput A nested initializer list of Cell objects, where each
        ///                     inner list represents a row in the matrix.
        /// 
        /// \note The dimensions of the matrix are determined by the input dimensions.
        ///       Ensure that all rows have consistent column counts for proper matrix structure.
        void func_loadInitialiserCellMatrix(std::initializer_list<std::initializer_list<Cell>> _matrixInput);


        void update__maxSize_axisVectors();

        /// @brief  Get [y][x] matrix location to the first character for the given cell for the text in
        ///         PrintableStringVectorMatrix.
        Pos2d<size_t> helper_PSVmatrixLocator(Pos2d<size_t> _cellPos);
        
        /// @brief Updates the table pointer stored in all table cells.
        /// 
        /// This function iterates through all cells in the table and updates their
        /// internal references to point to the current table object. This is typically
        /// called after table restructuring operations to ensure cells maintain valid
        /// pointers to their parent table.
        void helper_updateTablePtrInCells();
        
        /// @brief Separates a string into multiple lines based on a delimiter.
        /// @param _toSep The string to be separated into lines.
        /// @param _delim The delimiter used to separate lines. Defaults to newline character "\n".
        /// @return A vector of strings, each representing a separated line.
        std::vector<std::string> help__separateLines(std::string _toSep, std::string _delim="\n");
        
        /// @brief Updates the string table with current data
        /// @details Refreshes and synchronizes the string table contents to reflect
        ///          the latest state of the application data
        void update__string_table();
        /// @brief Final string containing the core::table in full
        std::string string_table{""};
        /**
         * string vector that represents the rows for the printable table.
         * 
         * [row][column]
         * 
         */
        std::vector<std::string> PrintableStringVectorMatrix;

        style_axisCellScalingMethod scalMethod_columns{style_axisCellScalingMethod::fitMenuAxis_even};
        style_axisCellScalingMethod scalMethod_rows{style_axisCellScalingMethod::fitMenuAxis_even};
        
        /// Member variables that store strings as symbols for the different aspects of the Terminal User Interface.
        ///     the contents are not allowed to be completely empty and must at least hold a single space to separate the different cells.
        ///     for now we ignore specific corner symbols
        std::string symb_delimiter_columns{"|"};
        std::string symb_delimiter_rows{"-"};
        std::string symb_border_column{"|"};
        std::string symb_border_row{"-"};
        std::string rowSeparator{"\n"}; // currently not used
        

        public:

        Table(std::initializer_list<std::initializer_list<Cell>> _matrixInput);

        Table();
        Table(const Table& _toCopy);
        Table(Table&& _toMove);
        ~Table();
        Table& operator=(const Table& _toCopy);
        Table& operator=(Table&& _toMove);
        Table& operator=(std::initializer_list<std::initializer_list<Cell>> _matrixInput);
        
        std::vector<Cell>& operator[](size_t _i);
        std::vector<Cell>  operator[](size_t _i) const;
        std::vector<Cell>& at(size_t _i);
        std::vector<Cell>  at(size_t _i) const;

        Cell& get_cell(size_t _x, size_t _y);
        Cell  get_cell(size_t _x, size_t _y) const;
        Cell& get_cell(Pos2d<int> _pos);
        Cell  get_cell(Pos2d<int> _pos) const;

        size_t rows();
        size_t columns();
        size_t size();
        
        std::vector<Cell> continuous(bool _includeNullCells=true);

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
     * A user defiend type that holds the info of the core::menu of the current core::menu window consisting of core::table and core::cell user defiend type.
     * can be called from core::cell type.
     */
    class core::Menu: public Table {
        private:

        Pos2d<int> pos_selectedCell;
        
        
        void helper_drawSelectedCell(Pos2d<int> _selectPos);

        public:

        Menu(std::initializer_list<std::initializer_list<Cell>> _matrixInput);
        Menu(const Table& _tableToCopy);
        Menu(Table&& _tableToCopy);
        
        Menu();
        Menu(const Menu& _toCopy);
        Menu(Menu&& _toMove);
        ~Menu();
        Menu& operator=(const Menu& _toCopy);
        Menu& operator=(Menu&& _toMove);
        
        enum class MenuDriver_returnType {
            success,
            error,

        };

        MenuDriver_returnType Driver();
    
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
