
#include "wmpt_tui.hpp"
#include "wmpt_types.hpp"
#include "wmpt_functions.hpp"

#include "wintoastlib.h"
#include <string>
#include <windows.h>

using namespace WinToastLib;

class CustomHandler : public IWinToastHandler {
public:
    void toastActivated() const {
        std::wcout << L"The user clicked in this toast" << std::endl;
        exit(0);
    }

    void toastActivated(int actionIndex) const {
        std::wcout << L"The user clicked on action #" << actionIndex << std::endl;
        exit(16 + actionIndex);
    }

    void toastActivated(std::wstring response) const {
        std::wcout << L"The user replied with: " << response << std::endl;
        exit(0);
    }

    void toastDismissed(WinToastDismissalReason state) const {
        switch (state) {
            case UserCanceled:
                std::wcout << L"The user dismissed this toast" << std::endl;
                exit(1);
                break;
            case TimedOut:
                std::wcout << L"The toast has timed out" << std::endl;
                exit(2);
                break;
            case ApplicationHidden:
                std::wcout << L"The application hid the toast using ToastNotifier.hide()" << std::endl;
                exit(3);
                break;
            default:
                std::wcout << L"Toast not activated" << std::endl;
                exit(4);
                break;
        }
    }

    void toastFailed() const {
        std::wcout << L"Error showing current toast" << std::endl;
        exit(5);
    }
};


PriceTracker::callbackType_trackedAllOffers TUINC::callbackFunc_trackedAllOffers = [](PriceTracker::TrackItem _item, std::vector<PriceTracker::ItemOffer> _offers) {
    std::unique_lock<std::mutex> u_lck__TrackingItemOffer(__mtx_access_TrackingItemsOffers);
    TrackingItemsOffers[_item.getTrackID()] = {_item, _offers};
};
PriceTracker::callbackType_trackedFound TUINC::callbackFunc_offersFound = [](PriceTracker::TrackItem _item, std::vector<PriceTracker::ItemOffer> _offers) {
    std::unique_lock<std::mutex> u_lck__TrackingItemOffer(__mtx_access_TrackingItemsOffers);
    TrackingItemsOffers[_item.getTrackID()] = {_item, _offers};
    

    std::wstring appName        = L"Console WinToast Example";
    std::wstring appUserModelID = L"WinToast Console Example";
    std::wstring text           = L"text test";
    std::wstring imagePath      = L"";
    std::wstring attribute      = L"default";
    
    // std::vector<std::wstring> actions;
    INT64 expiration = 0;
    // bool input       = false;

    // bool onlyCreateShortcut                   = false;
    WinToastTemplate::AudioOption audioOption = WinToastTemplate::AudioOption::Default;


    WinToast::instance()->setAppName(appName);
    WinToast::instance()->setAppUserModelId(appUserModelID);

    // if (onlyCreateShortcut) {
    //     if (!imagePath.empty() || !text.empty() || actions.size() > 0 || expiration) {
    //         std::wcerr << L"--only-create-shortcut does not accept images/text/actions/expiration" << std::endl;
    //         // return 9;
    //     }
    //     enum WinToast::ShortcutResult result = WinToast::instance()->createShortcut();
    //     // return result ? 16 + result : 0;
    // }
    // if (text.empty()) {text = L"Hello, world!";}

    if (!WinToast::instance()->initialize()) {
        std::wcerr << L"Error, your system in not compatible!" << std::endl;
        // return Results::InitializationFailure;
    }

    WinToastTemplate templ(!imagePath.empty() ? WinToastTemplate::ImageAndText02 : WinToastTemplate::Text02);
    templ.setTextField(text, WinToastTemplate::FirstLine);
    templ.setAudioOption(audioOption);
    templ.setAttributionText(attribute);
    templ.setImagePath(imagePath);
    // if (input) {templ.addInput();}
    // for (auto const& action : actions) {templ.addAction(action);}

    if (expiration) {templ.setExpiration(expiration);}

    if (WinToast::instance()->showToast(templ, new CustomHandler()) < 0) {
        std::wcerr << L"Could not launch your toast notification!";
        // return Results::ToastFailed;
    }

    // Give the handler a chance for 15 seconds (or the expiration plus 1 second)
    Sleep(expiration ? (DWORD) expiration + 1000 : 15000);
};


PriceTracker::threadClass TUINC::threadObj_PriceTracker{callbackFunc_offersFound, callbackFunc_trackedAllOffers, false};


TUINC::Results TUINC::Initialise() {
    if(!WinToast::isCompatible()) {
        std::wcerr << L"Error, your system in not supported!" << std::endl;
        return Results::SystemNotSupported;
    }
    try {
        threadObj_PriceTracker.startThread();
    }
    catch(const std::exception &e) {
        std::wcerr << L"Failed to start PriceTracker threadClass object with exception\"" << e.what() << "\"\n";
        return Results::ThreadInitialisationFailure;
    }

    return Results::Success;
}

int TUINC::Drive() {
    std::vector<int> _keys;

    // Need to create individual tables for each window.

}


namespace TUINC {

    Pos2d<size_t> helper_getConsoleDimensions() {
        Pos2d<size_t> console_dimensions(-1, -1);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        console_dimensions.x = csbi.srWindow.Right  - csbi.srWindow.Left    + 1;
        console_dimensions.y = csbi.srWindow.Bottom - csbi.srWindow.Top     + 1;

        return console_dimensions;
    }

    core::Cell::Cell(cell_types _cellType): cellType(_cellType) {

    }
    core::Cell::Cell(std::string _text, cell_types _cellType): cellType(_cellType), text(_text), isDefined__text(true) {

    }
    core::Cell::Cell(std::string _text, type_cellFunc _func, cell_types _cellType): cellType(_cellType), text(_text), function(_func), isDefined__text(true), isDefined__function(true) {

    }
    core::Cell::Cell(std::string _text, Menu&& _menu, cell_types _cellType=cell_types::menu): cellType(_cellType), text(_text), menu(std::move(_menu)), isDefined__text(true), isDefined__menu(true) {
        
    }

    core::Cell::Cell(const Cell& _toCopy): cellType(_toCopy.cellType) {
        tablePtr = _toCopy.tablePtr;
        if(_toCopy.isDefined__pos) pos = _toCopy.pos;
        if(_toCopy.isDefined__text) text = _toCopy.text;
        if(_toCopy.isDefined__function) function = _toCopy.function;
        if(_toCopy.isDefined__menu) menu = _toCopy.menu;
        // if(_toCopy.__isDefinedcellContent_null) cellContent_null = _toCopy.cellContent_null;
        // if(_toCopy.__isDefinedcellContent_text) cellContent_text = _toCopy.cellContent_text;
        // if(_toCopy.__isDefinedcellContent_function) cellContent_function = _toCopy.cellContent_function;

    }
    core::Cell::Cell(Cell&& _toMove): cellType(std::move(_toMove.cellType)), tablePtr(std::move(_toMove.tablePtr)) {
        
        if(_toMove.isDefined__pos) std::swap(pos, _toMove.pos);
        if(_toMove.isDefined__text) std::swap(text, _toMove.text);
        if(_toMove.isDefined__function) std::swap(function, _toMove.function);
        if(_toMove.isDefined__menu) std::swap(menu, _toMove.menu);
        // if(_toMove.__isDefinedcellContent_null) std::swap(cellContent_null, _toMove.cellContent_null);
        // if(_toMove.__isDefinedcellContent_text) std::swap(cellContent_text, _toMove.cellContent_text);
        // if(_toMove.__isDefinedcellContent_function) std::swap(cellContent_function,  _toMove.cellContent_function);

    }
    core::Cell::~Cell() {

    }
    core::Cell& core::Cell::operator=(const Cell& _toCopy) {
        cellType = _toCopy.cellType;
        tablePtr = _toCopy.tablePtr;
        if(_toCopy.isDefined__pos) pos = _toCopy.pos;
        if(_toCopy.isDefined__text) text = _toCopy.text;
        if(_toCopy.isDefined__function) function = _toCopy.function;
        if(_toCopy.isDefined__menu) menu = _toCopy.menu;
        // if(_toCopy.__isDefinedcellContent_null) cellContent_null = _toCopy.cellContent_null;
        // if(_toCopy.__isDefinedcellContent_text) cellContent_text = _toCopy.cellContent_text;
        // if(_toCopy.__isDefinedcellContent_function) cellContent_function = _toCopy.cellContent_function;
    }
    core::Cell& core::Cell::operator=(Cell&& _toMove) {
        std::swap(cellType, _toMove.cellType);
        
        std::swap(tablePtr, _toMove.tablePtr);
        if(_toMove.isDefined__pos) std::swap(pos, _toMove.pos);
        if(_toMove.isDefined__text) std::swap(text, _toMove.text);
        if(_toMove.isDefined__function) std::swap(function, _toMove.function);
        if(_toMove.isDefined__menu) std::swap(menu, _toMove.menu);
        // if(_toMove.__isDefinedcellContent_null) std::swap(cellContent_null, _toMove.cellContent_null);
        // if(_toMove.__isDefinedcellContent_text) std::swap(cellContent_text, _toMove.cellContent_text);
        // if(_toMove.__isDefinedcellContent_function) std::swap(cellContent_function,  _toMove.cellContent_function);
    }

    void core::Cell::call() {
        switch (cellType) {
        case cell_types::function:
            if(!isDefined__function) { throw std::runtime_error("ERROR: TUINC::cell::call() : a call function has not been defined."); }
            else {
                if(!tablePtr) { throw std::runtime_error("ERROR: TUINC::cell::call() : tablePtr has not been defined."); }
                function(tablePtr);
            }
            break;
        case cell_types::menu:
            if(!isDefined__menu) { throw std::runtime_error("ERROR: TUINC::cell::call() : a call menu has not been defined."); }
            else {
                menu.Driver();
            }
            break;
        default:
            throw std::runtime_error("ERROR: TUINC::cell::call() : call() member called without the cell type being a callable type.");
            break;
        }
    }

    int core::Cell::set_tablePtr(Table* _tablePtr) {
        tablePtr = _tablePtr;
        isModified__tablePtr = true;
        return 0;
    }

    int core::Cell::set_pos(Pos2d<int> _pos) {
        pos = _pos;
        isDefined__pos = true;
        isModified__pos = true;
        return 0;
    }
    int core::Cell::set_text(std::string _text) {
        text = _text;
        isDefined__text = true;
        isModified__text = true;
        return 0;
    }
    int core::Cell::set_function(type_cellFunc _func) {
        function = _func;
        isDefined__function = true;
        isModified__function = true;
        return 0;
    }
    int core::Cell::set_menu(Menu&& _menu) {
        menu = std::move(_menu);
        isDefined__menu = true;
        isModified__menu = true;
        return 0;
    }
    int core::Cell::setContent_null(cellTypeContent_null _newContent) {
        cellContent_null = _newContent;
        // __isDefinedcellContent_null = true;

        return 0;
    }
    int core::Cell::setContent_text(cellTypeContent_text _newContent) {
        cellContent_text = _newContent;
        // __isDefinedcellContent_text = true;

        return 0;
    }
    int core::Cell::setContent_function(cellTypeContent_function _newContent) {
        cellContent_function = _newContent;
        // __isDefinedcellContent_function = true;

        return 0;
    }
    int core::Cell::setContent_menu(cellTypeContent_menu _newContent) {
        cellContent_menu = _newContent;
        
    }
    void core::Cell::change_type(cell_types _newType) {
        cellType = _newType;
        isModified__cellType = true;
    }
    void core::Cell::change_type(cell_types _newType, std::string _text) {
        this->change_type(_newType);
        text = _text;
        isModified__text = true;
    }
    void core::Cell::change_type(cell_types _newType, std::string _text, type_cellFunc _func) {
        this->change_type(_newType, _text);
        function = _func;
        isModified__function = true;
    }
    void core::Cell::change_type(cell_types _newType, type_cellFunc _func) {
        this->change_type(_newType);
        function = _func;
        isModified__function = true;
    }
    void core::Cell::change_type(cell_types _newType, std::string _text, Menu&& _menu) {
        this->change_type(_newType, _text);
        menu = std::move(_menu);
        isModified__menu = true;
    }
    void core::Cell::change_type(cell_types _newType, Menu&& _menu) {
        this->change_type(_newType);
        menu = std::move(_menu);
        isModified__menu = true;
    }

    core::Table* core::Cell::get_tablePtr() const {
        return tablePtr;
    }
    Pos2d<int> core::Cell::get_pos() const {
        if(!isDefined__pos) throw std::runtime_error("cell::get_pos() const : member called when pos has not been defined.");
        return pos;
    }
    std::string core::Cell::get_text() const {
        if(!isDefined__text) throw std::runtime_error("cell::get_text() const : member called when text has not been defined.");
        return text;
    }
    type_cellFunc core::Cell::get_function() const {
        if(!isDefined__function) throw std::runtime_error("cell::get_function() const : member called when function has not been defined.");
        return function;
    }
    core::Menu core::Cell::get_menu() const {
        if(!isDefined__menu) throw std::runtime_error("cell::get_menu() const : member called when menu* has not been defined.");
        return menu;
    }
    cellTypeContent_null core::Cell::get_cellContent_null() const {
        // if(!__isDefinedcellContent_null) throw std::runtime_error("cell::get_cellContent_null() const : member has been called when cellContent for null has not been defined.");
        return cellContent_null;
    }
    cellTypeContent_text core::Cell::get_cellContent_text() const {
        // if(!__isDefinedcellContent_text) throw std::runtime_error("cell::get_cellContent_text() const : member has been called when cellContent for text has not been defined.");
        return cellContent_text;
    }
    cellTypeContent_function core::Cell::get_cellContent_function() const {
        // if(!__isDefinedcellContent_function) throw std::runtime_error("cell::get_cellContent_function() const : member has been called when cellContent for function has not been defined.");
        return cellContent_function;
    }
    cellTypeContent_menu core::Cell::get_cellContent_menu() const {
        return cellContent_menu;
    }
    cell_types core::Cell::get_type() const {
        return cellType;
    }
    
    void core::Cell::cell_resetModificationFlag() {
        isModified__tablePtr = false;
        isModified__pos = false;
        isModified__text = false;
        isModified__function = false;

    }
    bool core::Cell::isModified() {
        return (isModified__tablePtr || isModified__pos || isModified__text || isModified__function);
    }
    bool core::Cell::isModified_tablePtr() {
        return isModified__tablePtr;
    }
    bool core::Cell::isModified_pos() {
        return isModified__pos;
    }
    bool core::Cell::isModified_text() {
        return isModified__text;
    }
    bool core::Cell::isModified_function() {
        return isModified__function;
    }
    bool core::Cell::isModified_menu() {
        return isModified__menu;
    }


    void core::Table::func_loadInitialiserCellMatrix(std::initializer_list<std::initializer_list<core::Cell>> _matrixInput) {
        Pos2d<int>  limMatrix_min(0, 0);
        Pos2d<int>  limMatrix_max(0, 0);
        Pos2d<bool> limDefined_min(false, false);
        Pos2d<bool> limDefined_max(false, false);
        Pos2d<int>  lineCountedDim(0, _matrixInput.size());
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(itr_cell->isDefined__pos) {
                    if(itr_cell->pos.x > limMatrix_max.x || !limDefined_max.x) {
                        limMatrix_max.x = itr_cell->pos.x;
                        limDefined_max.x = true;
                    }
                    else if(itr_cell->pos.x < limMatrix_min.x || !limDefined_min.x) {
                        limMatrix_min.x = itr_cell->pos.x;
                        limDefined_min.x = true;
                    }
                    if(itr_cell->pos.y > limMatrix_max.y || !limDefined_max.y) {
                        limMatrix_max.y = itr_cell->pos.y;
                        limDefined_max.y = true;
                    }
                    else if(itr_cell->pos.y < limMatrix_min.y || !limDefined_min.y) {
                        limMatrix_min.y = itr_cell->pos.y;
                        limDefined_min.y = true;
                    }

                }
            }
            if(itr_row->size() > lineCountedDim.x) lineCountedDim.x = itr_row->size();
        }
        if(limDefined_min.x) assert(limDefined_max.x && " how the fuck is minimum defined but not maximum for x");
        if(limDefined_min.y) assert(limDefined_max.y && " how the fuck is minimum defined but not maximum for y");
        if(lineCountedDim.x > limMatrix_max.x-limMatrix_min.x) { //the defined limits are smaller than the existing number of cells for a row
            limMatrix_max.x = lineCountedDim.x;
        }
        if(lineCountedDim.y > limMatrix_max.y-limMatrix_min.y) {
            limMatrix_max.y = lineCountedDim.y;
        }

        tableOfCells = std::vector<std::vector<core::Cell>>(limMatrix_max.y-limMatrix_min.y, std::vector<core::Cell>(limMatrix_max.x-limMatrix_min.x, {cell_types::null}));
        
        Pos2d<int> lineCountedPos = limMatrix_min;
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            lineCountedPos.x = limMatrix_min.x;
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(itr_cell->cellType!=cell_types::null) {
                    if(itr_cell->isDefined__pos) tableOfCells[itr_cell->pos.y][itr_cell->pos.x] = *itr_cell;
                    else {
                        if(tableOfCells[lineCountedPos.y][lineCountedPos.x].cellType==cell_types::null) {
                            tableOfCells[itr_cell->pos.y][itr_cell->pos.x] = *itr_cell;
                        }
                    }
                }
            }
            lineCountedPos.y++;
        }

    }

    void core::Table::update__maxSize_axisVectors() {
        
        /// If either of the axis in the cell matrix is empty, exit the function because there is nothing to update.
        if(tableOfCells.size()==0) {
            throw std::runtime_error("ERROR: void core::Table::update__maxSize_axisVectors() : tableOfCells.size()==0");
        }
        if(tableOfCells.at(0).size()==0) {
            throw std::runtime_error("ERROR: void core::Table::update__maxSize_axisVectors() : tableOfCells.at(0).size()==0");
        }

        /// xy variable that holds the character-count axis dimensions of the table, i.e. how many characters wide/high
        Pos2d<int> tableDimensions(dimSize_table);
        if(dimSize_table.x==-1 || dimSize_table.y==-1) {
            Pos2d<int> dimSize_terminal{0, 0};
            
            /// NOTE: currently windows only: change/rewrite if implementing cross-platform
            auto consoleDims = helper_getConsoleDimensions();
            if(dimSize_table.x==-1) tableDimensions.x = consoleDims.x;
            if(dimSize_table.y==-1) tableDimensions.y = consoleDims.y;
        }
        
        /// Local containers holding the boggest size for that axis column/row
        std::vector<size_t> maxSize_columns(tableOfCells.at(0).size(), 0);
        std::vector<size_t> maxSize_rows(tableOfCells.size(), 0);
        
        /// If any of the axis has a scalMethod that is cellWidth, go through every cell and get the maximum cell's width/height for that column/row.
        if(scalMethod_columns==style_axisCellScalingMethod::cellWidth || scalMethod_rows==style_axisCellScalingMethod::cellWidth) {
            /// iterate through every cell that isn't null or doesn't follow delimiter separations to get the biggest cell's text dimensions
            for(size_t _y=0; _y<tableOfCells.size(); _y++) { // iterate over every row in table
                for(size_t _x=0; _x<tableOfCells.at(0).size(); _x++) { // iterate over every cell in row
                    
                    auto cell = tableOfCells.at(_y).at(_x);
                    switch (cell.get_type()) {
                    case cell_types::null:
                        continue;
                        break;
                    case cell_types::text:
                        if(!cell.get_cellContent_text().rule_followDelimiter) continue;
                        break;
                    case cell_types::function:
                        break;
                    default:
                        assert(false && "Something is wrong where an undefined enum value was entered [1].");
                        break;
                    }
                    
                    Pos2d<size_t> textBoxMaxDim(0, 0);
                    /// Get the max width and height of the texts in the text cell
                    size_t textWidth = 0;
                    for(size_t i=0; i<cell.get_text().size(); i++) {
                        if(cell.get_text().at(i)=='\n') {
                            textBoxMaxDim.y++;
                            if(textWidth > textBoxMaxDim.x) textBoxMaxDim.x = textWidth;
                            textWidth = 0;
                        }
                        else {
                            textWidth++;
                        }
                    }

    
                    if(textBoxMaxDim.x>maxSize_columns.at(_x))  maxSize_columns.at(_x)   = textBoxMaxDim.x;
                    if(textBoxMaxDim.y>maxSize_rows.at(_y))     maxSize_rows.at(_y)      = textBoxMaxDim.y;
    
                }
            }
        }
        /**
         * If cellScaling method for columns/x-axis is fitMenuAxis_even then adjust the sizes for the cellSize values in maxSize_columnWidths to an evenly divided cell width,
         *  that takes into account borderSymb size.
         * 
         */
        if(scalMethod_columns==style_axisCellScalingMethod::fitMenuAxis_even) {
            /**
             * Cell width solved from evenly dividing the table width
             * 
             * size_of_cell = total_size_of_all_cells( width_of_table - symbol_size_of_borderColumn*2 - (number of column delimiters)*size_of_column_delimiter ) / number_of_cells
             */
            int avgCellWidth = (tableDimensions.x-borderSymb_column.size()*2-(maxSize_columns.size()-1)*delimiter_columns.size())/maxSize_columns.size();
            for(auto& _el : maxSize_columns) {
                _el = (avgCellWidth < 0? 0 : static_cast<size_t>(avgCellWidth));
            }
        }
        /**
         * If cellScaling method for rows/y-axis is fitMenuAxis_even then adjust the sizes for the cellSize values in maxSize_rowHeights to an evenly divided cell height,
         *  that takes into account borderSymb size.
         * 
         */
        if(scalMethod_rows==style_axisCellScalingMethod::fitMenuAxis_even) {
            int avgCellHeight = (tableDimensions.y-borderSymb_row.size()*2-(maxSize_rows.size()-1)*delimiter_rows.size())/maxSize_rows.size();
            for(auto& _el : maxSize_rows) {
                _el = (avgCellHeight < 0? 0 : static_cast<size_t>(avgCellHeight));
            }
        }
        
        this->maxSize_columnWidths  = maxSize_columns;
        this->maxSize_rowHeights    = maxSize_rows;
    }

    Pos2d<size_t> core::Table::helper_PSVmatrixLocator(Pos2d<size_t> _cellPos) {
        if(tableOfCells.size()==0) throw std::runtime_error("Pos2d<size_t> core::Table::helper_PSVmatrixLocator(Pos2d<size_t>) : tableOfCells.size()==0");
        if(tableOfCells.at(0).size()==0) throw std::runtime_error("Pos2d<size_t> core::Table::helper_PSVmatrixLocator(Pos2d<size_t>) : tableOfCells.at(0).size()==0");
    
        /**
         * Using the currently defined maxSize_{axis label} size vectors, figure out the 2d matrix coordinate to the first char in the PRV-matrix (PrintableStringVectorMatrix)
         * 
         * 
         */
        
    }
    void core::Table::helper_updateTablePtrInCells() {
        /// Iterate through every cell in the table matrix and set their dedicated Table pointer to this tables's address
        for(size_t _y=0; _y<tableOfCells.size(); _y++) {
            for(size_t _x=0; _x<tableOfCells.at(_y).size(); _x++) {
                Cell& cell = tableOfCells.at(_y).at(_x);
                cell.tablePtr = this;
            }
        }
    }
    std::vector<std::string> core::Table::help__separateLines(std::string _toSep, std::string _delim) {
        if(_toSep.size()==0) throw std::runtime_error("table::help__separateLines(std::string, std::string) : _toSep value cannot be empty.");
        if(_delim.size()==0) throw std::runtime_error("table::help__separateLines(std::string, std::string) : _delim value cannot be empty.");
        std::vector<std::string> returVec;
        size_t ignPos = 0;
        for(size_t _c=0; _c<_toSep.size()-_delim.size()+1; _c++) {
            if(_toSep.substr(_c, _delim.size())==_delim) {
                returVec.push_back(_toSep.substr(ignPos, _c-ignPos));
                _c+=_delim.size();
                ignPos = _c;
            }
        }
        if(ignPos!=_toSep.size()) returVec.push_back(_toSep.substr(ignPos, std::string::npos));

        return returVec;
    }
    void core::Table::update__string_table() {
        
        this->update__maxSize_axisVectors();

        
        Pos2d<int> tableDimensions(dimSize_table);
        if(dimSize_table.x==-1 || dimSize_table.y==-1) {
            Pos2d<int> dimSize_terminal{0, 0};
            
            /// NOTE: currently windows only: change/rewrite if implementing cross-platform
            auto consoleDims = helper_getConsoleDimensions();
            if(dimSize_table.x==-1) tableDimensions.x = consoleDims.x;
            if(dimSize_table.y==-1) tableDimensions.y = consoleDims.y;
        }
        

        /// NOTE! To-do: Add a modification check to each cell so all of this can be optimised and potentially avoided
        /// as in, change the current method for writing to the "final string" so that instead of creating a copy, edit the existing "string" container.

        /// Check size of string vector against currently defined dimensions and
        /// resize the main storage string vectors' (PSVmatrix) dimensions accordingly.
        Pos2d<int> diffCount(static_cast<int>(tableDimensions.x)-static_cast<int>(PrintableStringVectorMatrix.at(0).size()), static_cast<int>(tableDimensions.y)-static_cast<int>(PrintableStringVectorMatrix.size()));
        if(diffCount.y!=0) { /// update row count
            if(diffCount.y<0) { /// new tableDimensions.y is smaller than current size
                auto itr_erasePos = PrintableStringVectorMatrix.begin();
                std::advance(itr_erasePos, static_cast<int>(PrintableStringVectorMatrix.size())+diffCount.y);
                PrintableStringVectorMatrix.erase(itr_erasePos, PrintableStringVectorMatrix.end());
            }
            else { /// new tableDimensions.y is bigger than current size
                PrintableStringVectorMatrix.insert(PrintableStringVectorMatrix.end(), diffCount.y, std::string(PrintableStringVectorMatrix.at(0).size(), ' '));
            }
        }
        if(diffCount.x!=0) { /// update column count
            for(auto itr_rowVec=PrintableStringVectorMatrix.begin(); itr_rowVec!=PrintableStringVectorMatrix.end(); ++itr_rowVec) {
                if(diffCount.x<0) { /// new tableDimensions.x is smaller than current size
                    auto itr_erasePos = itr_rowVec->begin();
                    std::advance(itr_erasePos, static_cast<int>(itr_rowVec->size())+diffCount.x);
                    itr_rowVec->erase(itr_erasePos, itr_rowVec->end());
                }
                else { /// new tableDimensions.x is bigger than current size
                    itr_rowVec->insert(itr_rowVec->end(), diffCount.x, ' ');
                }

            }
        }
        

        /// ----------------- REWRITE THE CONTENTS BELOW THIS TO BE UPDATES ---------------------------
        
        /// Local cursorPos variable for editing the characters where .x is a std::strings character(column), .y is a std::vector's element (row)
        /// NOTE: Start at 0. Need to index by 1 when the coordinates are used in ansii positions since they start at 1
        Pos2d<int> cursorPos_edit(0, 0);
        


        // std::string temporaryFinalString = "";   
        if(borderSymb_row.size()>0) { /// If the border symbol for row isn't empty then create the top border/frame side
            for(;temporaryFinalString.size()<tableDimensions.x; temporaryFinalString+=borderSymb_row);
            if(temporaryFinalString.size() > tableDimensions.x) temporaryFinalString.erase(tableDimensions.x, std::string::npos);
            temporaryFinalString+=rowSeparator;
        }
        Pos2d<int> cursorPos_editing(0, 0);
        for(size_t _y=0; _y<maxSize_rows.size(); _y++) {
            for(size_t cellY=0; cellY<maxSize_rows.at(_y); cellY++) { /// Iterate over every line/y-value for this row height
                temporaryFinalString+=borderSymb_column;
                cursorPos_editing.y = _y+cellY;
                for(size_t _x=0; _x<maxSize_columns.size(); _x++) { /// Iterate over every cell's cellY line for this row
                    cursorPos_editing.x = _x;
                    auto cell = tableOfCells.at(_y).at(_x);
                    bool useText = false;
                    Pos2d<size_t> cellLim{maxSize_columns.at(_x), maxSize_rows.at(_y)};
                    switch (cell.get_type()) { // check cell type to decide whether to use the text inside the cell (deprecated)
                        case cell_types::null:
                            temporaryFinalString+=std::string(cellLim.x, ' ');
                        case cell_types::text: {
                                if(!cell.get_cellContent_text().rule_followDelimiter) { /// If rule_followDelimiter is set to false then this text will be overlayed later on on top of everything.
                                    temporaryFinalString+=std::string(cellLim.x, ' ');
                                }
                                else {
                                    useText = true;
                                    break;
                                }
                            }
                            break;
                        case cell_types::function: {
                                useText = true;
                                break;
                            }
                            break;
                        default:
                            assert(false && "Something is wrong where an undefined enum value was entered [2].");
                            break;
                    }
                    if(useText) {
                        std::vector<std::string> cells_rows = help__separateLines(cell.get_text(), "\n");
                         /// if this cell's row count is bigger than the current y-value for this row height, use the string, otherwise the row count is bigger than the number of rows
                         ///  that this cell's text has
                        std::string cell_line = (cells_rows.size()>cellY? cells_rows.at(cellY) : "");
                        /// To the final string, append only the section that is allowed by the cellLim.x
                        temporaryFinalString+=cell_line.substr(0, cellLim.x);
                        int cellSpace = cellLim.x-cell_line.size();
                        if(cellSpace>0) temporaryFinalString+=std::string(cellSpace, ' ');
                    }
                    if(_x+1<maxSize_columns.size()) temporaryFinalString+=delimiter_columns;
                }
                temporaryFinalString+=borderSymb_column+rowSeparator;
            }
            if(_y+1<maxSize_rows.size()) { /// If current _y value isn't the last row, create a line of delimiter_rows string to separate rows
                temporaryFinalString+=borderSymb_column;
                for(size_t _x=0; _x<maxSize_columns.size(); _x++) {
                    std::string _temp="";
                    for(; _temp.size()<maxSize_columns.at(_x); _temp+=delimiter_rows);
                    if(_temp.size()>maxSize_columns.at(_x)) _temp.erase(maxSize_columns.at(_x), std::string::npos);
                    temporaryFinalString+=_temp;
                    if(_x+1<maxSize_columns.size()) temporaryFinalString+=delimiter_columns;
                }
                temporaryFinalString+=borderSymb_column+rowSeparator;
            }
        }
        if(borderSymb_row.size()>0) {
            for(;temporaryFinalString.size()<tableDimensions.x; temporaryFinalString+=borderSymb_row);
            if(temporaryFinalString.size() > tableDimensions.x) temporaryFinalString.erase(tableDimensions.x, std::string::npos);
            temporaryFinalString+=rowSeparator;
        }
        
        /// Draw border/background symbols and related


        /// Draw text that ignores/goes-over border/delimiter symbols


    }

    core::Table::Table(std::initializer_list<std::initializer_list<Cell>> _matrixInput) {
        this->func_loadInitialiserCellMatrix(_matrixInput);
    
    }

    core::Table::Table() {

    }
    core::Table::Table(const Table& _toCopy): 
        tableOfCells(_toCopy.tableOfCells), dimSize_table(_toCopy.dimSize_table), maxSize_columnWidths(_toCopy.maxSize_columnWidths), maxSize_rowHeights(_toCopy.maxSize_rowHeights), string_table(_toCopy.string_table),
        scalMethod_columns(_toCopy.scalMethod_columns), scalMethod_rows(_toCopy.scalMethod_rows), rowSeparator(_toCopy.rowSeparator),
        delimiter_columns(_toCopy.delimiter_columns), delimiter_rows(_toCopy.delimiter_rows), borderSymb_column(_toCopy.borderSymb_column), borderSymb_row(_toCopy.borderSymb_row)
    {
        this->helper_updateTablePtrInCells();
        
        
        
    }
    core::Table::Table(Table&& _toMove):
        tableOfCells(std::move(_toMove.tableOfCells)), dimSize_table(std::move(_toMove.dimSize_table)), maxSize_columnWidths(std::move(_toMove.maxSize_columnWidths)), maxSize_rowHeights(std::move(_toMove.maxSize_rowHeights)), string_table(std::move(_toMove.string_table)),
        scalMethod_columns(std::move(_toMove.scalMethod_columns)), scalMethod_rows(std::move(_toMove.scalMethod_rows)), rowSeparator(std::move(_toMove.rowSeparator)),
        delimiter_columns(std::move(_toMove.delimiter_columns)), delimiter_rows(std::move(_toMove.delimiter_rows)), borderSymb_column(std::move(_toMove.borderSymb_column)), borderSymb_row(std::move(_toMove.borderSymb_row))
    {
        this->helper_updateTablePtrInCells();
        
    }
    core::Table::~Table() {

    }
    core::Table& core::Table::operator=(const Table& _toCopy) {
        tableOfCells  = _toCopy.tableOfCells;
        dimSize_table = _toCopy.dimSize_table;
        maxSize_columnWidths = _toCopy.maxSize_columnWidths; 
        maxSize_rowHeights   = _toCopy.maxSize_rowHeights;
        string_table  = _toCopy.string_table;
        scalMethod_columns    = _toCopy.scalMethod_columns;
        scalMethod_rows   = _toCopy.scalMethod_rows;
        delimiter_columns = _toCopy.delimiter_columns;
        delimiter_rows    = _toCopy.delimiter_rows;
        borderSymb_column = _toCopy.borderSymb_column;
        borderSymb_row    = _toCopy.borderSymb_row;
        rowSeparator  = _toCopy.rowSeparator;
        
    }
    core::Table& core::Table::operator=(Table&& _toMove) {
        std::swap(tableOfCells, _toMove.tableOfCells);
        std::swap(tableOfCells, _toMove.tableOfCells);
        std::swap(dimSize_table, _toMove.dimSize_table);
        std::swap(maxSize_columnWidths, _toMove.maxSize_columnWidths); 
        std::swap(maxSize_rowHeights, _toMove.maxSize_rowHeights);
        std::swap(string_table, _toMove.string_table);
        std::swap(scalMethod_columns, _toMove.scalMethod_columns);
        std::swap(scalMethod_rows, _toMove.scalMethod_rows);
        std::swap(delimiter_columns, _toMove.delimiter_columns);
        std::swap(delimiter_rows, _toMove.delimiter_rows);
        std::swap(borderSymb_column, _toMove.borderSymb_column);
        std::swap(borderSymb_row,  _toMove.borderSymb_row);
        std::swap(rowSeparator,  _toMove.rowSeparator);
        
    }
    core::Table& core::Table::operator=(std::initializer_list<std::initializer_list<core::Cell>> _matrixInput) {
        this->func_loadInitialiserCellMatrix(_matrixInput);

        this->helper_updateTablePtrInCells();
        
        return *this;
    }
    
    std::vector<core::Cell>& core::Table::operator[](size_t _i) {
        return tableOfCells[_i];
    }
    std::vector<core::Cell>  core::Table::Table::operator[](size_t _i) const {
        return tableOfCells[_i];
    }
    std::vector<core::Cell>& core::Table::at(size_t _i) {
        if(tableOfCells.size()==0)        throw std::runtime_error("at(size_t) : table is empty of cells.");
        if(_i>=tableOfCells.size())       throw std::runtime_error("at(size_t) : _i argument is bigger than existing rows in table.");
        

        return tableOfCells.at(_i);
    }
    std::vector<core::Cell>  core::Table::at(size_t _i) const {
        if(tableOfCells.size()==0)        throw std::runtime_error("at(size_t) const : table is empty of cells.");
        if(_i>=tableOfCells.size())       throw std::runtime_error("at(size_t) const : _i argument is bigger than existing rows in table.");
        

        return tableOfCells.at(_i);
    }
    // table_row  at(size_t _i) const;

    core::Cell& core::Table::get_cell(size_t _x, size_t _y) {
        return tableOfCells.at(_x).at(_y);
    }
    core::Cell core::Table::get_cell(size_t _x, size_t _y) const {
        return get_cell(_x, _y);
    }
    core::Cell& core::Table::get_cell(Pos2d<int> _pos) {
        return get_cell(_pos.x, _pos.y);
    }
    core::Cell core::Table::get_cell(Pos2d<int> _pos) const {
        return get_cell(_pos);
    }

    size_t core::Table::rows() {
        return tableOfCells.size();
    }
    size_t core::Table::columns() {
        return (tableOfCells.size()>0? tableOfCells.at(0).size() : 0);
    }
    size_t core::Table::size() {
        return tableOfCells.size() * (tableOfCells.size()>0? tableOfCells.at(0).size() : 0);
    }
    std::vector<core::Cell> core::Table::continuous(bool _includeNullCells) {
        std::vector<core::Cell> returVec;
        for(auto row : tableOfCells) {
            for(auto cell : row) {
                if(!_includeNullCells && cell.cellType==cell_types::null) continue;
                returVec.push_back(cell);
            }
        }
        
        return returVec;
    }


    // int table::addCell(size_t _x, size_t _y) {
    // }
    // int table::addCell(Pos2d<int> _pos) {
    // }
    // int table::moveCell(size_t _current_x, size_t _current_y, size_t _new_x, size_t _new_y) {
    // }
    // int table::moveCell(Pos2d<int> _current_pos, Pos2d<int> _new_pos) {
    // }
    // int table::eraseCell(size_t _x, size_t _y) {
    // }
    // int table::eraseCell(Pos2d<int> _pos) {
    // }

    Results core::Table::call(size_t _x, size_t _y) {
        if(tableOfCells.size()==0)        throw std::runtime_error("call(size_t, size_t) : table is empty of cells.");
        if(_y>=tableOfCells.size())       throw std::runtime_error("call(size_t, size_t) : _y argument is bigger than existing rows in table.");
        if(_x>=tableOfCells.at(0).size()) throw std::runtime_error("call(size_t, size_t) : _x argument is bigger than existing columns in table.");
        if(tableOfCells.at(_y).at(_x).cellType!=cell_types::function) throw std::runtime_error(
            std::string("call(size_t, size_t) : called cell at row:")+std::to_string(_y)+" column:"+std::to_string(_x)+" is not a function cell but defined as a "+(tableOfCells.at(_y).at(_x).cellType==cell_types::null? "null" : "text")+" type cell."
        );
        if(!tableOfCells.at(_y).at(_x).isDefined__function) throw std::runtime_error(std::string("call(size_t, size_t) : cell at [")+std::to_string(_y)+"]["+std::to_string(_x)+"] doesn't have a defined function to call.");

        tableOfCells.at(_y).at(_x).call();

        return Results::Success;
    }
    Results core::Table::call(Pos2d<int> _pos) {
        if(tableOfCells.size()==0)        throw std::runtime_error("call(Pos2d<int>) : table is empty of cells.");
        if(_pos.y>=tableOfCells.size())       throw std::runtime_error("call(Pos2d<int>) : _y argument is bigger than existing rows in table.");
        if(_pos.x>=tableOfCells.at(0).size()) throw std::runtime_error("call(Pos2d<int>) : _x argument is bigger than existing columns in table.");
        if(tableOfCells.at(_pos.y).at(_pos.x).cellType!=cell_types::function) throw std::runtime_error(
            std::string("call(Pos2d<int>) : called cell at row:")+std::to_string(_pos.y)+" column:"+std::to_string(_pos.x)+" is not a function cell but defined as a "+(tableOfCells.at(_pos.y).at(_pos.x).cellType==cell_types::null? "null" : "text")+" type cell."
        );
        if(!tableOfCells.at(_pos.y).at(_pos.x).isDefined__function) throw std::runtime_error(std::string("call(Pos2d<int>) : cell at [")+std::to_string(_pos.y)+"]["+std::to_string(_pos.x)+"] doesn't have a defined function to call.");

        tableOfCells.at(_pos.y).at(_pos.x).call();

        return Results::Success;
    }

    

    core::Menu::Menu(std::initializer_list<std::initializer_list<core::Cell>> _matrixInput) {
        this->func_loadInitialiserCellMatrix(_matrixInput);

        
    }
    core::Menu::Menu(const Table& _tableToCopy) {

    }
    core::Menu::Menu(Table&& _tableToMove) {

    }
    
    core::Menu::Menu() {
        
    }
    core::Menu::Menu(const Menu& _toCopy) {

    }
    core::Menu::Menu(Menu&& _toMove) {
        
    }
    core::Menu::~Menu() {
        
    }

    core::Menu& core::Menu::operator=(const Menu& _toCopy) {

    }
    core::Menu& core::Menu::operator=(Menu&& _toMove) {

    }

    void core::Menu::Driver() {

    }
    
};
