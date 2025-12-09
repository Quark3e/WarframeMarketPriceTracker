
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


    cell::cell(cell_types _cellType): cellType(_cellType) {

    }
    cell::cell(std::string _text, cell_types _cellType): cellType(_cellType), text(_text), isDefined__text(true) {

    }
    cell::cell(std::string _text, type_cellFunc _func, cell_types _cellType): cellType(_cellType), text(_text), function(_func), isDefined__text(true), isDefined__function(true) {

    }
    cell::cell(std::string _text, menu* _menuPtr, cell_types _cellType=cell_types::menuPtr): cellType(_cellType), text(_text), menuPtr(_menuPtr), isDefined__text(true), isDefined__menuPtr(true) {
        
    }

    cell::cell(const cell& _toCopy): cellType(_toCopy.cellType) {
        tablePtr = _toCopy.tablePtr;
        if(_toCopy.isDefined__pos) pos = _toCopy.pos;
        if(_toCopy.isDefined__text) text = _toCopy.text;
        if(_toCopy.isDefined__function) function = _toCopy.function;
        if(_toCopy.isDefined__menuPtr) menuPtr = _toCopy.menuPtr;
        // if(_toCopy.__isDefinedcellContent_null) cellContent_null = _toCopy.cellContent_null;
        // if(_toCopy.__isDefinedcellContent_text) cellContent_text = _toCopy.cellContent_text;
        // if(_toCopy.__isDefinedcellContent_function) cellContent_function = _toCopy.cellContent_function;

    }
    cell::cell(cell&& _toMove): cellType(std::move(_toMove.cellType)), tablePtr(std::move(_toMove.tablePtr)) {
        
        if(_toMove.isDefined__pos) std::swap(pos, _toMove.pos);
        if(_toMove.isDefined__text) std::swap(text, _toMove.text);
        if(_toMove.isDefined__function) std::swap(function, _toMove.function);
        if(_toMove.isDefined__menuPtr) std::swap(menuPtr, _toMove.menuPtr);
        // if(_toMove.__isDefinedcellContent_null) std::swap(cellContent_null, _toMove.cellContent_null);
        // if(_toMove.__isDefinedcellContent_text) std::swap(cellContent_text, _toMove.cellContent_text);
        // if(_toMove.__isDefinedcellContent_function) std::swap(cellContent_function,  _toMove.cellContent_function);

    }
    cell::~cell() {

    }
    cell& cell::operator=(const cell& _toCopy) {
        cellType = _toCopy.cellType;
        tablePtr = _toCopy.tablePtr;
        if(_toCopy.isDefined__pos) pos = _toCopy.pos;
        if(_toCopy.isDefined__text) text = _toCopy.text;
        if(_toCopy.isDefined__function) function = _toCopy.function;
        if(_toCopy.isDefined__menuPtr) menuPtr = _toCopy.menuPtr;
        // if(_toCopy.__isDefinedcellContent_null) cellContent_null = _toCopy.cellContent_null;
        // if(_toCopy.__isDefinedcellContent_text) cellContent_text = _toCopy.cellContent_text;
        // if(_toCopy.__isDefinedcellContent_function) cellContent_function = _toCopy.cellContent_function;
    }
    cell& cell::operator=(cell&& _toMove) {
        std::swap(cellType, _toMove.cellType);
        
        std::swap(tablePtr, _toMove.tablePtr);
        if(_toMove.isDefined__pos) std::swap(pos, _toMove.pos);
        if(_toMove.isDefined__text) std::swap(text, _toMove.text);
        if(_toMove.isDefined__function) std::swap(function, _toMove.function);
        if(_toMove.isDefined__menuPtr) std::swap(menuPtr, _toMove.menuPtr);
        // if(_toMove.__isDefinedcellContent_null) std::swap(cellContent_null, _toMove.cellContent_null);
        // if(_toMove.__isDefinedcellContent_text) std::swap(cellContent_text, _toMove.cellContent_text);
        // if(_toMove.__isDefinedcellContent_function) std::swap(cellContent_function,  _toMove.cellContent_function);
    }

    void cell::call() {
        switch (cellType) {
        case cell_types::function:
            if(!isDefined__function) { throw std::runtime_error("ERROR: TUINC::cell::call() : a call function has not been defined."); }
            else {
                if(!tablePtr) { throw std::runtime_error("ERROR: TUINC::cell::call() : tablePtr has not been defined."); }
                function(tablePtr);
            }
            break;
        case cell_types::menuPtr:
            if(!isDefined__menuPtr) { throw std::runtime_error("ERROR: TUINC::cell::call() : a call menuPtr has not been defined."); }
            else {
                menuPtr->Driver();
            }
            break;
        default:
            throw std::runtime_error("ERROR: TUINC::cell::call() : call() member called without the cell type being a callable type.");
            break;
        }
    }

    int cell::set_tablePtr(__table* _tablePtr) {
        tablePtr = _tablePtr;
        isModified__tablePtr = true;
        return 0;
    }

    int cell::set_pos(Pos2d<int> _pos) {
        pos = _pos;
        isDefined__pos = true;
        isModified__pos = true;
        return 0;
    }
    int cell::set_text(std::string _text) {
        text = _text;
        isDefined__text = true;
        isModified__text = true;
        return 0;
    }
    int cell::set_function(type_cellFunc _func) {
        function = _func;
        isDefined__function = true;
        isModified__function = true;
        return 0;
    }
    int cell::set_menuPtr(menu* _menuPtr) {
        menuPtr = _menuPtr;
        isDefined__menuPtr = true;
        isModified__menuPtr = true;
        return 0;
    }
    int cell::setContent_null(cellTypeContent_null _newContent) {
        cellContent_null = _newContent;
        // __isDefinedcellContent_null = true;

        return 0;
    }
    int cell::setContent_text(cellTypeContent_text _newContent) {
        cellContent_text = _newContent;
        // __isDefinedcellContent_text = true;

        return 0;
    }
    int cell::setContent_function(cellTypeContent_function _newContent) {
        cellContent_function = _newContent;
        // __isDefinedcellContent_function = true;

        return 0;
    }
    int cell::setContent_menuPtr(cellTypeContent_menuPtr _newContent) {
        cellContent_menuPtr = _newContent;
        
    }
    void cell::change_type(cell_types _newType) {
        cellType = _newType;
        isModified__cellType = true;
    }
    void cell::change_type(cell_types _newType, std::string _text) {
        this->change_type(_newType);
        text = _text;
        isModified__text = true;
    }
    void cell::change_type(cell_types _newType, std::string _text, type_cellFunc _func) {
        this->change_type(_newType, _text);
        function = _func;
        isModified__function = true;
    }
    void cell::change_type(cell_types _newType, type_cellFunc _func) {
        this->change_type(_newType);
        function = _func;
        isModified__function = true;
    }
    void cell::change_type(cell_types _newType, std::string _text, menu* _menuPtr) {
        this->change_type(_newType, _text);
        menuPtr = _menuPtr;
        isModified__menuPtr = true;
    }
    void cell::change_type(cell_types _newType, menu* _menuPtr) {
        this->change_type(_newtype);
        menuPtr = _menuPtr;
        isModified__menuPtr = true;
    }

    table*        cell::get_tablePtr() const {
        return tablePtr;
    }
    Pos2d<int>      cell::get_pos() const {
        if(!isDefined__pos) throw std::runtime_error("cell::get_pos() const : member called when pos has not been defined.");
        return pos;
    }
    std::string     cell::get_text() const {
        if(!isDefined__text) throw std::runtime_error("cell::get_text() const : member called when text has not been defined.");
        return text;
    }
    type_cellFunc cell::get_function() const {
        if(!isDefined__function) throw std::runtime_error("cell::get_function() const : member called when function has not been defined.");
        return function;
    }
    menu*           cell::get_menuPtr() const {
        if(!isDefined__menuPtr) throw std::runtime_error("cell::get_menuPtr() const : member called when menu* has not been defined.");
        return menuPtr;
    }
    cellTypeContent_null  cell::get_cellContent_null() const {
        // if(!__isDefinedcellContent_null) throw std::runtime_error("cell::get_cellContent_null() const : member has been called when cellContent for null has not been defined.");
        return cellContent_null;
    }
    cellTypeContent_text  cell::get_cellContent_text() const {
        // if(!__isDefinedcellContent_text) throw std::runtime_error("cell::get_cellContent_text() const : member has been called when cellContent for text has not been defined.");
        return cellContent_text;
    }
    cellTypeContent_function  cell::get_cellContent_function() const {
        // if(!__isDefinedcellContent_function) throw std::runtime_error("cell::get_cellContent_function() const : member has been called when cellContent for function has not been defined.");
        return cellContent_function;
    }
    cellTypeContent_menuPtr cell::get_cellContent_menuPtr() const {
        return cellContent_menuPtr;
    }
    cell_types cell::get_type() const {
        return cellType;
    }
    
    void cell::cell_resetModificationFlag() {
        isModified__tablePtr = false;
        isModified__pos = false;
        isModified__text = false;
        isModified__function = false;

    }
    bool cell::isModified() {
        return (isModified__tablePtr || isModified__pos || isModified__text || isModified__function);
    }
    bool cell::isModified_tablePtr() {
        return isModified__tablePtr;
    }
    bool cell::isModified_pos() {
        return isModified__pos;
    }
    bool cell::isModified_text() {
        return isModified__text;
    }
    bool cell::isModified_function() {
        return isModified__function;
    }
    bool cell::isModified_menuPtr() {
        return isModified__menuPtr;
    }

    

    void table::helper_updateTablePtrInCells() {
        for(size_t _y=0; _y<tableOfCells.size(); _y++) {
            for(size_t _x=0; _x<tableOfCells.at(_y).size(); _x++) {
                cell& cell = tableOfCells.at(_y).at(_x);
                cell.tablePtr = this;
            }
        }
    }
    std::vector<std::string> table::help__separateLines(std::string _toSep, std::string _delim) {
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
    void table::updatestring_table() {
        
        if(tableOfCells.size()==0) {
            return;
        }
        if(tableOfCells.at(0).size()==0) {
            return;
        }


        if(dimSize_table.x==-1 || dimSize_table.y==-1) {
            Pos2d<int> dimSize_terminal{0, 0};
            
            /// NOTE: currently windows only: change/rewrite if implementing cross-platform
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            dimSize_terminal.x = csbi.srWindow.Right   - csbi.srWindow.Left    + 1;
            dimSize_terminal.y = csbi.srWindow.Bottom  - csbi.srWindow.Top     + 1;

            if(dimSize_table.x==-1) dimSize_table.x = dimSize_terminal.x;
            if(dimSize_table.y==-1) dimSize_table.y = dimSize_terminal.y;
        }
        

        std::vector<size_t> size_columns(tableOfCells.at(0).size(), 0);
        std::vector<size_t> size_rows(tableOfCells.size(), 1);
        
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
                    
                    Pos2d<size_t> textBoxDim(0, 0);
                    textBoxDim.x = cell.get_text().size();
                    /// get height of text box
                    for(size_t i=0; i<cell.get_text().size(); i++) {
                        if(cell.get_text().at(i)=='\n') textBoxDim.y++;
                    }
    
                    if(textBoxDim.x>size_columns.at(_x))  size_columns.at(_x)   = textBoxDim.x;
                    if(textBoxDim.y>size_rows.at(_y))     size_rows.at(_y)      = textBoxDim.y;
    
                }
            }
        }
        
        if(scalMethod_columns==style_axisCellScalingMethod::fitMenuAxis) {
            for(auto& _el : size_columns) {
                int temp_cellWidth = (dimSize_table.x-borderSymb_column.size()*2-(size_columns.size()-1)*delimiter_columns.size())/size_columns.size();
                _el = (temp_cellWidth < 0? 0 : static_cast<size_t>(temp_cellWidth));
            }
        }
        if(scalMethod_rows==style_axisCellScalingMethod::fitMenuAxis) {
            for(auto& _el : size_rows) {
                int temp_cellHeight = (dimSize_table.y-borderSymb_row.size()*2-(size_rows.size()-1)*delimiter_rows.size())/size_rows.size();
                _el = (temp_cellHeight < 0? 0 : static_cast<size_t>(temp_cellHeight));
            }
        }

        /// NOTE! To-do: Add a modification check to each cell so all of this can be optimised and potentially avoided
        std::string temporaryFinalString = "";
        if(borderSymb_row.size()>0) {
            for(;temporaryFinalString.size()<dimSize_table.x; temporaryFinalString+=borderSymb_row);
            if(temporaryFinalString.size() > dimSize_table.x) temporaryFinalString.erase(dimSize_table.x, std::string::npos);
            temporaryFinalString+=rowSeparator;
        }
        for(size_t _y=0; _y<size_rows.size(); _y++) {
            for(size_t cellY=0; cellY<size_rows.at(_y); cellY++) {
                temporaryFinalString+=borderSymb_column;
                for(size_t _x=0; _x<size_columns.size(); _x++) {
                    auto cell = tableOfCells.at(_y).at(_x);
                    bool useText = false;
                    Pos2d<size_t> cellLim{size_columns.at(_x), size_rows.at(_y)};
                    switch (cell.get_type()) {
                        case cell_types::null:
                            temporaryFinalString+=std::string(cellLim.x, ' ');
                        case cell_types::text: {
                                if(!cell.get_cellContent_text().rule_followDelimiter) {
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
                        std::string cell_line = (cells_rows.size()>cellY? cells_rows.at(cellY) : "");
                        temporaryFinalString+=cell_line.substr(0, cellLim.x);
                        int cellSpace = cellLim.x-cell_line.size();
                        if(cellSpace>0) temporaryFinalString+=std::string(cellSpace, ' ');
                    }
                    if(_x+1<size_columns.size()) temporaryFinalString+=delimiter_columns;
                }
                temporaryFinalString+=borderSymb_column+rowSeparator;
            }
            if(_y+1<size_rows.size()) {
                temporaryFinalString+=borderSymb_column;
                for(size_t _x=0; _x<size_columns.size(); _x++) {
                    std::string _temp="";
                    for(; _temp.size()<size_columns.at(_x); _temp+=delimiter_rows);
                    if(_temp.size()>size_columns.at(_x)) _temp.erase(size_columns.at(_x), std::string::npos);
                    temporaryFinalString+=_temp;
                    if(_x+1<size_columns.size()) temporaryFinalString+=delimiter_columns;
                }
                temporaryFinalString+=borderSymb_column+rowSeparator;
            }
        }
        if(borderSymb_row.size()>0) {
            for(;temporaryFinalString.size()<dimSize_table.x; temporaryFinalString+=borderSymb_row);
            if(temporaryFinalString.size() > dimSize_table.x) temporaryFinalString.erase(dimSize_table.x, std::string::npos);
            temporaryFinalString+=rowSeparator;
        }
        

        std::swap(string_table, temporaryFinalString);
    }

    table::table(std::initializer_list<std::initializer_list<cell>> _matrixInput) {
        this->operator=(_matrixInput);
        
    }

    table::table() {

    }
    table::table(const table& _toCopy): 
        tableOfCells(_toCopy.tableOfCells), dimSize_table(_toCopy.dimSize_table), size_columnWidths(_toCopy.size_columnWidths), size_rowHeights(_toCopy.size_rowHeights), string_table(_toCopy.string_table),
        scalMethod_columns(_toCopy.scalMethod_columns), scalMethod_rows(_toCopy.scalMethod_rows), rowSeparator(_toCopy.rowSeparator),
        delimiter_columns(_toCopy.delimiter_columns), delimiter_rows(_toCopy.delimiter_rows), borderSymb_column(_toCopy.borderSymb_column), borderSymb_row(_toCopy.borderSymb_row)
    {
        this->helper_updateTablePtrInCells();
        
        
        
    }
    table::table(__table&& _toMove):
        tableOfCells(std::move(_toMove.tableOfCells)), dimSize_table(std::move(_toMove.dimSize_table)), size_columnWidths(std::move(_toMove.size_columnWidths)), size_rowHeights(std::move(_toMove.size_rowHeights)), string_table(std::move(_toMove.string_table)),
        scalMethod_columns(std::move(_toMove.scalMethod_columns)), scalMethod_rows(std::move(_toMove.scalMethod_rows)), rowSeparator(std::move(_toMove.rowSeparator)),
        delimiter_columns(std::move(_toMove.delimiter_columns)), delimiter_rows(std::move(_toMove.delimiter_rows)), borderSymb_column(std::move(_toMove.borderSymb_column)), borderSymb_row(std::move(_toMove.borderSymb_row))
    {
        this->helper_updateTablePtrInCells();
        
    }
    table::~table() {

    }
    table& table::operator=(const table& _toCopy) {
        tableOfCells  = _toCopy.tableOfCells;
        dimSize_table = _toCopy.dimSize_table;
        size_columnWidths = _toCopy.size_columnWidths; 
        size_rowHeights   = _toCopy.size_rowHeights;
        string_table  = _toCopy.string_table;
        scalMethod_columns    = _toCopy.scalMethod_columns;
        scalMethod_rows   = _toCopy.scalMethod_rows;
        rowSeparator  = _toCopy.rowSeparator;
        delimiter_columns = _toCopy.delimiter_columns;
        delimiter_rows    = _toCopy.delimiter_rows;
        borderSymb_column = _toCopy.borderSymb_column;
        borderSymb_row    = _toCopy.borderSymb_row;
        
    }
    table& table::operator=(__table&& _toMove) {
        std::swap(tableOfCells, _toMove.tableOfCells);
        std::swap(tableOfCells, _toMove.tableOfCells);
        std::swap(dimSize_table, _toMove.dimSize_table);
        std::swap(size_columnWidths, _toMove.size_columnWidths); 
        std::swap(size_rowHeights, _toMove.size_rowHeights);
        std::swap(string_table, _toMove.string_table);
        std::swap(scalMethod_columns, _toMove.scalMethod_columns);
        std::swap(scalMethod_rows, _toMove.scalMethod_rows);
        std::swap(rowSeparator,  _toMove.rowSeparator);
        std::swap(delimiter_columns, _toMove.delimiter_columns);
        std::swap(delimiter_rows, _toMove.delimiter_rows);
        std::swap(borderSymb_column, _toMove.borderSymb_column);
        std::swap(borderSymb_row,  _toMove.borderSymb_row);
        
    }
    table& table::operator=(std::initializer_list<std::initializer_list<cell>> _matrixInput) {
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

        tableOfCells = std::vector<std::vector<cell>>(limMatrix_max.y-limMatrix_min.y, std::vector<cell>(limMatrix_max.x-limMatrix_min.x, {cell_types::null}));
        
        Pos2d<int> lineCountedPos = limMatrix_min;
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            lineCountedPos.x = limMatrix_min.x;
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(!itr_cell->cellType==cell_types::null) {
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

        this->helper_updateTablePtrInCells();
        
        return *this;
    }
    
    std::vector<cell>& table::operator[](size_t _i) {
        return tableOfCells[_i];
    }
    std::vector<cell>  table::table::operator[](size_t _i) const {
        return tableOfCells[_i];
    }
    std::vector<cell>& table::at(size_t _i) {
        if(tableOfCells.size()==0)        throw std::runtime_error("at(size_t) : table is empty of cells.");
        if(_i>=tableOfCells.size())       throw std::runtime_error("at(size_t) : _i argument is bigger than existing rows in table.");
        

        return tableOfCells.at(_i);
    }
    std::vector<cell>  table::at(size_t _i) const {
        if(tableOfCells.size()==0)        throw std::runtime_error("at(size_t) const : table is empty of cells.");
        if(_i>=tableOfCells.size())       throw std::runtime_error("at(size_t) const : _i argument is bigger than existing rows in table.");
        

        return tableOfCells.at(_i);
    }
    // table_row  at(size_t _i) const;

    cell& table::cell(size_t _x, size_t _y) {
        return tableOfCells.at(_x).at(_y);
    }
    cell table::cell(size_t _x, size_t _y) const {
        return cell(_x, _y);
    }
    cell& table::cell(Pos2d<int> _pos) {
        return cell(_pos.x, _pos.y);
    }
    cell table::cell(Pos2d<int> _pos) const {
        return cell(_pos);
    }

    size_t table::rows() {
        return tableOfCells.size();
    }
    size_t table::columns() {
        return (tableOfCells.size()>0? tableOfCells.at(0).size() : 0);
    }
    size_t table::size() {
        return tableOfCells.size() * (tableOfCells.size()>0? tableOfCells.at(0).size() : 0);
    }
    std::vector<cell> table::continuous(bool _includeNullCells) {
        std::vector<cell> returVec;
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

    Results table::call(size_t _x, size_t _y) {
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
    Results table::call(Pos2d<int> _pos) {
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

    
    
    
    
};
