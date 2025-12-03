
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



}


namespace TUINC {


    __cell::__cell(cell_types _cellType): __cellType(_cellType) {

    }
    __cell::__cell(std::string _text, cell_types _cellType): __cellType(_cellType), __text(_text), __isDefined__text(true) {

    }
    __cell::__cell(std::string _text, __type_cellFunc _func, cell_types _cellType): __cellType(_cellType), __text(_text), __function(_func), __isDefined__text(true), __isDefined__function(true) {

    }

    __cell::__cell(const __cell& _toCopy): __cellType(_toCopy.__cellType) {
        __tablePtr = _toCopy.__tablePtr;
        if(_toCopy.__isDefined__pos) __pos = _toCopy.__pos;
        if(_toCopy.__isDefined__text) __text = _toCopy.__text;
        if(_toCopy.__isDefined__function) __function = _toCopy.__function;
        // if(_toCopy.__isDefined__cellContent_null) __cellContent_null = _toCopy.__cellContent_null;
        // if(_toCopy.__isDefined__cellContent_text) __cellContent_text = _toCopy.__cellContent_text;
        // if(_toCopy.__isDefined__cellContent_function) __cellContent_function = _toCopy.__cellContent_function;

    }
    __cell::__cell(__cell&& _toMove): __cellType(std::move(_toMove.__cellType)), __tablePtr(std::move(_toMove.__tablePtr)) {
        
        if(_toMove.__isDefined__pos) std::swap(__pos, _toMove.__pos);
        if(_toMove.__isDefined__text) std::swap(__text, _toMove.__text);
        if(_toMove.__isDefined__function) std::swap(__function, _toMove.__function);
        // if(_toMove.__isDefined__cellContent_null) std::swap(__cellContent_null, _toMove.__cellContent_null);
        // if(_toMove.__isDefined__cellContent_text) std::swap(__cellContent_text, _toMove.__cellContent_text);
        // if(_toMove.__isDefined__cellContent_function) std::swap(__cellContent_function,  _toMove.__cellContent_function);

    }
    __cell::~__cell() {

    }
    __cell& __cell::operator=(const __cell& _toCopy) {
        __cellType = _toCopy.__cellType;
        __tablePtr = _toCopy.__tablePtr;
        if(_toCopy.__isDefined__pos) __pos = _toCopy.__pos;
        if(_toCopy.__isDefined__text) __text = _toCopy.__text;
        if(_toCopy.__isDefined__function) __function = _toCopy.__function;
        // if(_toCopy.__isDefined__cellContent_null) __cellContent_null = _toCopy.__cellContent_null;
        // if(_toCopy.__isDefined__cellContent_text) __cellContent_text = _toCopy.__cellContent_text;
        // if(_toCopy.__isDefined__cellContent_function) __cellContent_function = _toCopy.__cellContent_function;
    }
    __cell& __cell::operator=(__cell&& _toMove) {
        std::swap(__cellType, _toMove.__cellType);
        
        std::swap(__tablePtr, _toMove.__tablePtr);
        if(_toMove.__isDefined__pos) std::swap(__pos, _toMove.__pos);
        if(_toMove.__isDefined__text) std::swap(__text, _toMove.__text);
        if(_toMove.__isDefined__function) std::swap(__function, _toMove.__function);
        // if(_toMove.__isDefined__cellContent_null) std::swap(__cellContent_null, _toMove.__cellContent_null);
        // if(_toMove.__isDefined__cellContent_text) std::swap(__cellContent_text, _toMove.__cellContent_text);
        // if(_toMove.__isDefined__cellContent_function) std::swap(__cellContent_function,  _toMove.__cellContent_function);
    }

    void __cell::call() {
        switch (__cellType) {
        case cell_types::function:
            if(!__isDefined__function) { throw std::runtime_error("ERROR: TUINC::__cell::call() : a call function has not been defined."); }
            else {
                if(!__tablePtr) { throw std::runtime_error("ERROR: TUINC::__cell::call() : tablePtr has not been defined."); }
                __function(__tablePtr);
            }
            break;
        default:
            throw std::runtime_error("ERROR: TUINC::__cell::call() : call() member called without the cell type being a function.");
            break;
        }
    }

    int __cell::set_tablePtr(__table* _tablePtr) {
        __tablePtr = _tablePtr;
        __isModified__tablePtr = true;
        return 0;
    }

    int __cell::set_pos(Pos2d<int> _pos) {
        __pos = _pos;
        __isDefined__pos = true;
        __isModified__pos = true;
        return 0;
    }
    int __cell::set_text(std::string _text) {
        __text = _text;
        __isDefined__text = true;
        __isModified__text = true;
        return 0;
    }
    int __cell::set_function(__type_cellFunc _func) {
        __function = _func;
        __isDefined__function = true;
        __isModified__function = true;
        return 0;
    }
    int __cell::setContent_null(__cellTypeContent_null _newContent) {
        __cellContent_null = _newContent;
        // __isDefined__cellContent_null = true;

        return 0;
    }
    int __cell::setContent_text(__cellTypeContent_text _newContent) {
        __cellContent_text = _newContent;
        // __isDefined__cellContent_text = true;

        return 0;
    }
    int __cell::setContent_function(__cellTypeContent_function _newContent) {
        __cellContent_function = _newContent;
        // __isDefined__cellContent_function = true;

        return 0;
    }
    void __cell::change_type(cell_types _newType) {
        __cellType = _newType;
        __isModified__cellType = true;
    }
    void __cell::change_type(cell_types _newType, std::string _text) {
        this->change_type(_newType);
        __text = _text;
        __isModified__text = true;
    }
    void __cell::change_type(cell_types _newType, std::string _text, __type_cellFunc _func) {
        this->change_type(_newType, _text);
        __function = _func;
        __isModified__function = true;
    }
    void __cell::change_type(cell_types _newType, __type_cellFunc _func) {
        this->change_type(_newType);
        __function = _func;
        __isModified__function = true;
    }

    __table*        __cell::get_tablePtr() const {
        return __tablePtr;
    }
    Pos2d<int>      __cell::get_pos() const {
        if(!__isDefined__pos) throw std::runtime_error("__cell::get_pos() const : member called when pos has not been defined.");
        return __pos;
    }
    std::string     __cell::get_text() const {
        if(!__isDefined__text) throw std::runtime_error("__cell::get_text() const : member called when text has not been defined.");
        return __text;
    }
    __type_cellFunc __cell::get_function() const {
        if(!__isDefined__function) throw std::runtime_error("__cell::get_function() const : member called when function has not been defined.");
        return __function;
    }
    __cellTypeContent_null  __cell::get_cellContent_null() const {
        // if(!__isDefined__cellContent_null) throw std::runtime_error("__cell::get_cellContent_null() const : member has been called when cellContent for null has not been defined.");
        return __cellContent_null;
    }
    __cellTypeContent_text  __cell::get_cellContent_text() const {
        // if(!__isDefined__cellContent_text) throw std::runtime_error("__cell::get_cellContent_text() const : member has been called when cellContent for text has not been defined.");
        return __cellContent_text;
    }
    __cellTypeContent_function  __cell::get_cellContent_function() const {
        // if(!__isDefined__cellContent_function) throw std::runtime_error("__cell::get_cellContent_function() const : member has been called when cellContent for function has not been defined.");
        return __cellContent_function;
    }
    cell_types __cell::get_type() const {
        return __cellType;
    }
    
    void __cell::__cell_resetModificationFlag() {
        __isModified__tablePtr = false;
        __isModified__pos = false;
        __isModified__text = false;
        __isModified__function = false;

    }
    bool __cell::isModified() {
        return (__isModified__tablePtr || __isModified__pos || __isModified__text || __isModified__function);
    }
    bool __cell::isModified_tablePtr() {
        return __isModified__tablePtr;
    }
    bool __cell::isModified_pos() {
        return __isModified__pos;
    }
    bool __cell::isModified_text() {
        return __isModified__text;
    }
    bool __cell::isModified_function() {
        return __isModified__function;
    }

    
    // __table_row::__table_row(std::vector<__cell>& _table_row): __tableCellRow(_table_row) {
    // }
    // // __table_row::__table_row(const __table_row& _toCopy) {
    // //     __tableCellRow =_toCopy.__tableCellRow;
    // // }
    // __table_row::__table_row(__table_row&& _toMove): __tableCellRow(std::move(_toMove.__tableCellRow)) {
    // }
    // __table_row::~__table_row() {
    // }
    // //__table_row& __table_row::operator=(const __table_row& _toCopy) {
    // //}
    // __table_row& __table_row::operator=(__table_row&& _toMove) {
    //     std::swap(__tableCellRow, _toMove.__tableCellRow);
    // }
    // __cell& __table_row::operator[](size_t _i) {
    //     return __tableCellRow[_i];
    // }
    // __cell  __table_row::operator[](size_t _i) const {
    //     return __tableCellRow[_i];
    // }
    // __cell& __table_row::at(size_t _i) {
    //     return __tableCellRow.at(_i);
    // }
    // __cell __table_row::at(size_t _i) const  {
    //     return __tableCellRow.at(_i);
    // }
    // int __table_row::set_tablePtr(__table* _tablePtr) {
    //     __tablePtr = _tablePtr;
    //     __isModified__tablePtr = true;
    //     return 0;
    // };
    // __table* __table_row::get_tablePtr() {
    //     return __tablePtr;
    // }
    // size_t __table_row::size() {
    //     return __tableCellRow.size();
    // }
    // void __table_row::__resetModificationFlag() {
    //     for(size_t _i=0; _i<__isModified__cells.size(); _i++) {
    //         __tableCellRow.at(_i).__cell_resetModificationFlag();
    //         __isModified__cells.at(_i) = false;
    //     }
    // }
    // void __table_row::__set_cell_isModified(__cell* _cellPtrToFlag) {
    //     if(!_cellPtrToFlag) throw std::runtime_error("__table_row::__set_cell_isModified(_cell*) : pointer argument for __cell cannot be nullptr.");
    //     bool cellFound = false;
    //     for(size_t _i=0; _i<__tableCellRow.size(); _i++) {
    //         if(&__tableCellRow.at(_i)==_cellPtrToFlag) {
    //             __isModified__cells.at(_i) = true;
    //             cellFound = true;
    //             break;
    //         }
    //     }
    //     if(!cellFound) throw std::runtime_error("__table_row::__set_cell_isModified(_cell*) : invalid pointer argument. Given __cell address doesn't exist in this row as a stored cell.");
    // }
    // void __table_row::__checkCells(Enum_checkCells_whenIncorrect _operation) {
    //     for(size_t _i=0; _i<__tableCellRow.size(); _i++) {
    //         auto cell = __tableCellRow.at(_i);
    //         auto currCellPos = cell.get_pos();
    //         if(currCellPos.x!=_i || (__isDefined__table_yRow? currCellPos.y!=__table_yRow : true)) {
    //             switch (_operation) {
    //             case Enum_checkCells_whenIncorrect::perform_correction:
    //                 cell.set_pos({_i, (__isDefined__table_yRow? __table_yRow : currCellPos.y)});
    //                 break;
    //             case Enum_checkCells_whenIncorrect::throw_exception:
    //                 throw std::runtime_error(std::string("__table_row::checkCells(Enum_checkCells_whenIncorrect) : incorrect pos value of cell at [i]:")+std::to_string(_i));
    //                 break;
    //             default:
    //                 throw std::runtime_error(std::string("__table_row::checkCells(Enum_checkCells_whenIncorrect) : invalid enum value at [i]:")+std::to_string(_i));
    //                 break;
    //             }
    //         }
    //     }
    // }
    // bool __table_row::isModified(int _i) {
    //     if(_i>=__isModified__cells.size()) throw std::out_of_range(std::string("__table_row::isModified(int) : input value of ")+std::to_string(_i)+"is out of range "+std::to_string(__isModified__cells.size()));
    //     else if(_i<-1) throw std::runtime_error("__table_row::isModified(int) : invalid negative value.");
    //     bool isModif = false;
    //     if(_i>-1) isModif = __isModified__cells.at(_i);
    //     else {
    //         for(bool _var : __isModified__cells) {
    //             if(_var) {
    //                 isModif = true;
    //                 break;
    //             }
    //         }
    //     }
    //     return isModif;
    // }


    void __table::__helper_updateTablePtrInCells() {
        for(size_t _y=0; _y<__tableOfCells.size(); _y++) {
            for(size_t _x=0; _x<__tableOfCells.at(_y).size(); _x++) {
                __cell& cell = __tableOfCells.at(_y).at(_x);
                cell.__tablePtr = this;
            }
        }
    }
    std::vector<std::string> __table::__help__separateLines(std::string _toSep, std::string _delim) {
        if(_toSep.size()==0) throw std::runtime_error("__table::__help__separateLines(std::string, std::string) : _toSep value cannot be empty.");
        if(_delim.size()==0) throw std::runtime_error("__table::__help__separateLines(std::string, std::string) : _delim value cannot be empty.");
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
    void __table::__update__string_table() {
        
        if(__tableOfCells.size()==0) {
            return;
        }
        if(__tableOfCells.at(0).size()==0) {
            return;
        }


        if(__dimSize_table.x==-1 || __dimSize_table.y==-1) {
            Pos2d<int> dimSize_terminal{0, 0};
            
            /// NOTE: currently windows only: change/rewrite if implementing cross-platform
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            dimSize_terminal.x = csbi.srWindow.Right   - csbi.srWindow.Left    + 1;
            dimSize_terminal.y = csbi.srWindow.Bottom  - csbi.srWindow.Top     + 1;

            if(__dimSize_table.x==-1) __dimSize_table.x = dimSize_terminal.x;
            if(__dimSize_table.y==-1) __dimSize_table.y = dimSize_terminal.y;
        }
        

        std::vector<size_t> __size_columns(__tableOfCells.at(0).size(), 0);
        std::vector<size_t> __size_rows(__tableOfCells.size(), 1);
        
        if(__scalMethod_columns==style_axisCellScalingMethod::cellWidth || __scalMethod_rows==style_axisCellScalingMethod::cellWidth) {
            /// iterate through every cell that isn't null or doesn't follow delimiter separations to get the biggest cell's text dimensions
            for(size_t _y=0; _y<__tableOfCells.size(); _y++) { // iterate over every row in table
                for(size_t _x=0; _x<__tableOfCells.at(0).size(); _x++) { // iterate over every cell in row
                    
                    auto cell = __tableOfCells.at(_y).at(_x);
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
    
                    if(textBoxDim.x>__size_columns.at(_x))  __size_columns.at(_x)   = textBoxDim.x;
                    if(textBoxDim.y>__size_rows.at(_y))     __size_rows.at(_y)      = textBoxDim.y;
    
                }
            }
        }
        
        if(__scalMethod_columns==style_axisCellScalingMethod::fitMenuAxis) {
            for(auto& _el : __size_columns) {
                int temp_cellWidth = (__dimSize_table.x-__borderSymb_column.size()*2-(__size_columns.size()-1)*__delimiter_columns.size())/__size_columns.size();
                _el = (temp_cellWidth < 0? 0 : static_cast<size_t>(temp_cellWidth));
            }
        }
        if(__scalMethod_rows==style_axisCellScalingMethod::fitMenuAxis) {
            for(auto& _el : __size_rows) {
                int temp_cellHeight = (__dimSize_table.y-__borderSymb_row.size()*2-(__size_rows.size()-1)*__delimiter_rows.size())/__size_rows.size();
                _el = (temp_cellHeight < 0? 0 : static_cast<size_t>(temp_cellHeight));
            }
        }

        /// NOTE! To-do: Add a modification check to each cell so all of this can be optimised and potentially avoided
        std::string temporaryFinalString = "";
        if(__borderSymb_row.size()>0) {
            for(;temporaryFinalString.size()<__dimSize_table.x; temporaryFinalString+=__borderSymb_row);
            if(temporaryFinalString.size() > __dimSize_table.x) temporaryFinalString.erase(__dimSize_table.x, std::string::npos);
            temporaryFinalString+=__rowSeparator;
        }
        for(size_t _y=0; _y<__size_rows.size(); _y++) {
            for(size_t cellY=0; cellY<__size_rows.at(_y); cellY++) {
                temporaryFinalString+=__borderSymb_column;
                for(size_t _x=0; _x<__size_columns.size(); _x++) {
                    auto cell = __tableOfCells.at(_y).at(_x);
                    bool useText = false;
                    Pos2d<size_t> cellLim{__size_columns.at(_x), __size_rows.at(_y)};
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
                        std::vector<std::string> cells_rows = __help__separateLines(cell.get_text(), "\n");
                        std::string cell_line = (cells_rows.size()>cellY? cells_rows.at(cellY) : "");
                        temporaryFinalString+=cell_line.substr(0, cellLim.x);
                        int cellSpace = cellLim.x-cell_line.size();
                        if(cellSpace>0) temporaryFinalString+=std::string(cellSpace, ' ');
                    }
                    if(_x+1<__size_columns.size()) temporaryFinalString+=__delimiter_columns;
                }
                temporaryFinalString+=__borderSymb_column+__rowSeparator;
            }
            if(_y+1<__size_rows.size()) {
                temporaryFinalString+=__borderSymb_column;
                for(size_t _x=0; _x<__size_columns.size(); _x++) {
                    std::string _temp="";
                    for(; _temp.size()<__size_columns.at(_x); _temp+=__delimiter_rows);
                    if(_temp.size()>__size_columns.at(_x)) _temp.erase(__size_columns.at(_x), std::string::npos);
                    temporaryFinalString+=_temp;
                    if(_x+1<__size_columns.size()) temporaryFinalString+=__delimiter_columns;
                }
                temporaryFinalString+=__borderSymb_column+__rowSeparator;
            }
        }
        if(__borderSymb_row.size()>0) {
            for(;temporaryFinalString.size()<__dimSize_table.x; temporaryFinalString+=__borderSymb_row);
            if(temporaryFinalString.size() > __dimSize_table.x) temporaryFinalString.erase(__dimSize_table.x, std::string::npos);
            temporaryFinalString+=__rowSeparator;
        }
        

        std::swap(__string_table, temporaryFinalString);
    }

    __table::__table(std::initializer_list<std::initializer_list<__cell>> _matrixInput) {
        this->operator=(_matrixInput);
        
    }

    __table::__table() {

    }
    __table::__table(const __table& _toCopy): 
        __tableOfCells(_toCopy.__tableOfCells), __dimSize_table(_toCopy.__dimSize_table), __size_columnWidths(_toCopy.__size_columnWidths), __size_rowHeights(_toCopy.__size_rowHeights), __string_table(_toCopy.__string_table),
        __scalMethod_columns(_toCopy.__scalMethod_columns), __scalMethod_rows(_toCopy.__scalMethod_rows), __rowSeparator(_toCopy.__rowSeparator),
        __delimiter_columns(_toCopy.__delimiter_columns), __delimiter_rows(_toCopy.__delimiter_rows), __borderSymb_column(_toCopy.__borderSymb_column), __borderSymb_row(_toCopy.__borderSymb_row)
    {
        this->__helper_updateTablePtrInCells();
        
        
        
    }
    __table::__table(__table&& _toMove):
        __tableOfCells(std::move(_toMove.__tableOfCells)), __dimSize_table(std::move(_toMove.__dimSize_table)), __size_columnWidths(std::move(_toMove.__size_columnWidths)), __size_rowHeights(std::move(_toMove.__size_rowHeights)), __string_table(std::move(_toMove.__string_table)),
        __scalMethod_columns(std::move(_toMove.__scalMethod_columns)), __scalMethod_rows(std::move(_toMove.__scalMethod_rows)), __rowSeparator(std::move(_toMove.__rowSeparator)),
        __delimiter_columns(std::move(_toMove.__delimiter_columns)), __delimiter_rows(std::move(_toMove.__delimiter_rows)), __borderSymb_column(std::move(_toMove.__borderSymb_column)), __borderSymb_row(std::move(_toMove.__borderSymb_row))
    {
        this->__helper_updateTablePtrInCells();
        
    }
    __table::~__table() {

    }
    __table& __table::operator=(const __table& _toCopy) {
        __tableOfCells  = _toCopy.__tableOfCells;
        __dimSize_table = _toCopy.__dimSize_table;
        __size_columnWidths = _toCopy.__size_columnWidths; 
        __size_rowHeights   = _toCopy.__size_rowHeights;
        __string_table  = _toCopy.__string_table;
        __scalMethod_columns    = _toCopy.__scalMethod_columns;
        __scalMethod_rows   = _toCopy.__scalMethod_rows;
        __rowSeparator  = _toCopy.__rowSeparator;
        __delimiter_columns = _toCopy.__delimiter_columns;
        __delimiter_rows    = _toCopy.__delimiter_rows;
        __borderSymb_column = _toCopy.__borderSymb_column;
        __borderSymb_row    = _toCopy.__borderSymb_row;
        
    }
    __table& __table::operator=(__table&& _toMove) {
        std::swap(__tableOfCells, _toMove.__tableOfCells);
        std::swap(__tableOfCells, _toMove.__tableOfCells);
        std::swap(__dimSize_table, _toMove.__dimSize_table);
        std::swap(__size_columnWidths, _toMove.__size_columnWidths); 
        std::swap(__size_rowHeights, _toMove.__size_rowHeights);
        std::swap(__string_table, _toMove.__string_table);
        std::swap(__scalMethod_columns, _toMove.__scalMethod_columns);
        std::swap(__scalMethod_rows, _toMove.__scalMethod_rows);
        std::swap(__rowSeparator,  _toMove.__rowSeparator);
        std::swap(__delimiter_columns, _toMove.__delimiter_columns);
        std::swap(__delimiter_rows, _toMove.__delimiter_rows);
        std::swap(__borderSymb_column, _toMove.__borderSymb_column);
        std::swap(__borderSymb_row,  _toMove.__borderSymb_row);
        
    }
    __table& __table::operator=(std::initializer_list<std::initializer_list<__cell>> _matrixInput) {
        Pos2d<int>  limMatrix_min(0, 0);
        Pos2d<int>  limMatrix_max(0, 0);
        Pos2d<bool> limDefined_min(false, false);
        Pos2d<bool> limDefined_max(false, false);
        Pos2d<int>  lineCountedDim(0, _matrixInput.size());
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(itr_cell->__isDefined__pos) {
                    if(itr_cell->__pos.x > limMatrix_max.x || !limDefined_max.x) {
                        limMatrix_max.x = itr_cell->__pos.x;
                        limDefined_max.x = true;
                    }
                    else if(itr_cell->__pos.x < limMatrix_min.x || !limDefined_min.x) {
                        limMatrix_min.x = itr_cell->__pos.x;
                        limDefined_min.x = true;
                    }
                    if(itr_cell->__pos.y > limMatrix_max.y || !limDefined_max.y) {
                        limMatrix_max.y = itr_cell->__pos.y;
                        limDefined_max.y = true;
                    }
                    else if(itr_cell->__pos.y < limMatrix_min.y || !limDefined_min.y) {
                        limMatrix_min.y = itr_cell->__pos.y;
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

        __tableOfCells = std::vector<std::vector<__cell>>(limMatrix_max.y-limMatrix_min.y, std::vector<__cell>(limMatrix_max.x-limMatrix_min.x, {cell_types::null}));
        
        Pos2d<int> lineCountedPos = limMatrix_min;
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            lineCountedPos.x = limMatrix_min.x;
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(!itr_cell->__cellType==cell_types::null) {
                    if(itr_cell->__isDefined__pos) __tableOfCells[itr_cell->__pos.y][itr_cell->__pos.x] = *itr_cell;
                    else {
                        if(__tableOfCells[lineCountedPos.y][lineCountedPos.x].__cellType==cell_types::null) {
                            __tableOfCells[itr_cell->__pos.y][itr_cell->__pos.x] = *itr_cell;
                        }
                    }
                }
            }
            lineCountedPos.y++;
        }

        this->__helper_updateTablePtrInCells();
        
        return *this;
    }
    
    std::vector<__cell>& __table::operator[](size_t _i) {
        return __tableOfCells[_i];
    }
    std::vector<__cell>  __table::__table::operator[](size_t _i) const {
        return __tableOfCells[_i];
    }
    std::vector<__cell>& __table::at(size_t _i) {
        if(__tableOfCells.size()==0)        throw std::runtime_error("at(size_t) : table is empty of cells.");
        if(_i>=__tableOfCells.size())       throw std::runtime_error("at(size_t) : _i argument is bigger than existing rows in table.");
        

        return __tableOfCells.at(_i);
    }
    std::vector<__cell>  __table::at(size_t _i) const {
        if(__tableOfCells.size()==0)        throw std::runtime_error("at(size_t) const : table is empty of cells.");
        if(_i>=__tableOfCells.size())       throw std::runtime_error("at(size_t) const : _i argument is bigger than existing rows in table.");
        

        return __tableOfCells.at(_i);
    }
    // __table_row  at(size_t _i) const;

    __cell& __table::cell(size_t _x, size_t _y) {
        return __tableOfCells.at(_x).at(_y);
    }
    __cell __table::cell(size_t _x, size_t _y) const {
        return cell(_x, _y);
    }
    __cell& __table::cell(Pos2d<int> _pos) {
        return cell(_pos.x, _pos.y);
    }
    __cell __table::cell(Pos2d<int> _pos) const {
        return cell(_pos);
    }

    size_t __table::rows() {
        return __tableOfCells.size();
    }
    size_t __table::columns() {
        return (__tableOfCells.size()>0? __tableOfCells.at(0).size() : 0);
    }
    size_t __table::size() {
        return __tableOfCells.size() * (__tableOfCells.size()>0? __tableOfCells.at(0).size() : 0);
    }
    std::vector<__cell> __table::continuous(bool _includeNullCells) {
        std::vector<__cell> returVec;
        for(auto row : __tableOfCells) {
            for(auto cell : row) {
                if(!_includeNullCells && cell.__cellType==cell_types::null) continue;
                returVec.push_back(cell);
            }
        }
        
        return returVec;
    }


    // int __table::addCell(size_t _x, size_t _y) {
    // }
    // int __table::addCell(Pos2d<int> _pos) {
    // }
    // int __table::moveCell(size_t _current_x, size_t _current_y, size_t _new_x, size_t _new_y) {
    // }
    // int __table::moveCell(Pos2d<int> _current_pos, Pos2d<int> _new_pos) {
    // }
    // int __table::eraseCell(size_t _x, size_t _y) {
    // }
    // int __table::eraseCell(Pos2d<int> _pos) {
    // }

    Results __table::call(size_t _x, size_t _y) {
        if(__tableOfCells.size()==0)        throw std::runtime_error("call(size_t, size_t) : table is empty of cells.");
        if(_y>=__tableOfCells.size())       throw std::runtime_error("call(size_t, size_t) : _y argument is bigger than existing rows in table.");
        if(_x>=__tableOfCells.at(0).size()) throw std::runtime_error("call(size_t, size_t) : _x argument is bigger than existing columns in table.");
        if(__tableOfCells.at(_y).at(_x).__cellType!=cell_types::function) throw std::runtime_error(
            std::string("call(size_t, size_t) : called cell at row:")+std::to_string(_y)+" column:"+std::to_string(_x)+" is not a function cell but defined as a "+(__tableOfCells.at(_y).at(_x).__cellType==cell_types::null? "null" : "text")+" type cell."
        );
        if(!__tableOfCells.at(_y).at(_x).__isDefined__function) throw std::runtime_error(std::string("call(size_t, size_t) : cell at [")+std::to_string(_y)+"]["+std::to_string(_x)+"] doesn't have a defined function to call.");

        __tableOfCells.at(_y).at(_x).call();

        return Results::Success;
    }
    Results __table::call(Pos2d<int> _pos) {
        if(__tableOfCells.size()==0)        throw std::runtime_error("call(Pos2d<int>) : table is empty of cells.");
        if(_pos.y>=__tableOfCells.size())       throw std::runtime_error("call(Pos2d<int>) : _y argument is bigger than existing rows in table.");
        if(_pos.x>=__tableOfCells.at(0).size()) throw std::runtime_error("call(Pos2d<int>) : _x argument is bigger than existing columns in table.");
        if(__tableOfCells.at(_pos.y).at(_pos.x).__cellType!=cell_types::function) throw std::runtime_error(
            std::string("call(Pos2d<int>) : called cell at row:")+std::to_string(_pos.y)+" column:"+std::to_string(_pos.x)+" is not a function cell but defined as a "+(__tableOfCells.at(_pos.y).at(_pos.x).__cellType==cell_types::null? "null" : "text")+" type cell."
        );
        if(!__tableOfCells.at(_pos.y).at(_pos.x).__isDefined__function) throw std::runtime_error(std::string("call(Pos2d<int>) : cell at [")+std::to_string(_pos.y)+"]["+std::to_string(_pos.x)+"] doesn't have a defined function to call.");

        __tableOfCells.at(_pos.y).at(_pos.x).call();

        return Results::Success;
    }

};
