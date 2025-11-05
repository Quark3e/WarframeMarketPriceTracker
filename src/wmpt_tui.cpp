
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

    
    __cell::__cell(bool _nullCell): __nullCell(_nullCell) {}
    __cell::__cell(std::string _label): label(_label), __isDefined_label(true) {}
    __cell::__cell(std::string _label, Pos2d<int> _pos): label(_label), pos(_pos), __isDefined_label(true), __isDefined_pos(true) {

    }
    __cell::__cell(std::string _label, __type_cellFunc _function): label(_label), function(_function), __isDefined_label(true), __isDefined_function(true) {

    }
    __cell::__cell(std::string _label, __type_cellFunc _function, Pos2d<int> _pos): label(_label), function(_function), pos(_pos), __isDefined_label(true), __isDefined_function(true), __isDefined_pos(true) {

    }

    __cell::__cell(const __cell& _toCopy) {
        if(_toCopy.__isDefined_label) {
            this->label = _toCopy.label;
        }
        if(_toCopy.__isDefined_function) {
            this->function = _toCopy.function;
        }
        if(_toCopy.__isDefined_pos) {
            this->pos = _toCopy.pos;
        }
        
        __isDefined_label = _toCopy.__isDefined_label;
        __isDefined_function = _toCopy.__isDefined_function;
        __isDefined_pos = _toCopy.__isDefined_pos;

        __nullCell = _toCopy.__nullCell;
    }
    __cell::__cell(__cell&& _toMove) {
        if(_toMove.__isDefined_label) {
            std::swap(this->label, _toMove.label);
        }
        if(_toMove.__isDefined_function) {
            std::swap(this->function, _toMove.function);
        }
        if(_toMove.__isDefined_pos) {
            std::swap(this->pos, _toMove.pos);
        }

        __isDefined_label = _toMove.__isDefined_label;
        __isDefined_function = _toMove.__isDefined_function;
        __isDefined_pos = _toMove.__isDefined_pos;

        __nullCell = _toMove.__nullCell;
    }
    __cell::~__cell() {

    }
    __cell& __cell::operator=(const __cell& _toCopy) {
        if(_toCopy.__isDefined_label) {
            this->label = _toCopy.label;
        }
        if(_toCopy.__isDefined_function) {
            this->function = _toCopy.function;
        }
        if(_toCopy.__isDefined_pos) {
            this->pos = _toCopy.pos;
        }
        
        __isDefined_label = _toCopy.__isDefined_label;
        __isDefined_function = _toCopy.__isDefined_function;
        __isDefined_pos = _toCopy.__isDefined_pos;

        __nullCell = _toCopy.__nullCell;

        return *this;
    }
    __cell& __cell::operator=(__cell&& _toMove) {
        if(_toMove.__isDefined_label) {
            std::swap(this->label, _toMove.label);
        }
        if(_toMove.__isDefined_function) {
            std::swap(this->function, _toMove.function);
        }
        if(_toMove.__isDefined_pos) {
            std::swap(this->pos, _toMove.pos);
        }

        __isDefined_label = _toMove.__isDefined_label;
        __isDefined_function = _toMove.__isDefined_function;
        __isDefined_pos = _toMove.__isDefined_pos;

        __nullCell = _toMove.__nullCell;

        return *this;
    }


    __table_row::__table_row(std::vector<__cell>& _table_row): __tableCellRow(_table_row) {

    }
    // __table_row::__table_row(const __table_row& _toCopy) {
    //     __tableCellRow =_toCopy.__tableCellRow;
    // }
    __table_row::__table_row(__table_row&& _toMove): __tableCellRow(_toMove.__tableCellRow) {

    }
    __table_row::~__table_row() {

    }
    //__table_row& __table_row::operator=(const __table_row& _toCopy) {
    //}
    __table_row& __table_row::operator=(__table_row&& _toMove) {
        std::swap(__tableCellRow, _toMove.__tableCellRow);
    }

    __cell& __table_row::operator[](size_t _i) {
        return __tableCellRow[_i];
    }
    __cell  __table_row::operator[](size_t _i) const {
        return __tableCellRow[_i];
    }
    __cell& __table_row::at(size_t _i) {
        return __tableCellRow.at(_i);
    }
    __cell __table_row::at(size_t _i) const  {
        return __tableCellRow.at(_i);
    }

    size_t __table_row::size() {
        return __tableCellRow.size();
    }


    __table::__table(std::initializer_list<std::initializer_list<__cell>> _matrixInput) {
        this->operator=(_matrixInpit);
        
    }

    __table::__table() {

    }
    __table::__table(const __table& _toCopy): __tableOfCells(_toCopy.__tableOfCells) {

    }
    __table::__table(__table&& _toMove): __tableOfCells(_toMove.__tableOfCells) {
        
    }
    __table::~__table() {

    }
    __table& __table::operator=(const __table& _toCopy) {
        __tableOfCells = _toCopy.__tableOfCells;
    }
    __table& __table::operator=(__table&& _toMove) {
        std::swap(__tableOfCells, _toMove.__tableOfCells);

    }
    __table& __table::operator=(std::initializer_list<std::initializer_list<__cell>> _matrixInput) {
        Pos2d<int>  limMatrix_min(0, 0);
        Pos2d<int>  limMatrix_max(0, 0);
        Pos2d<bool> limDefined_min(false, false);
        Pos2d<bool> limDefined_max(false, false);
        limMatrix.y = _matrixInput.size();
        Pos2d<int>  limCountedDim(0, _matrixInput.size());
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(itr_cell->__isDefined_pos) {
                    if(itr_cell->pos.x > limMatrix_max.x || !limDefined_max.x) {
                        limMatrix_max.x = itr_cell.x;
                        limDefined_max.x = true;
                    }
                    else if(itr_cell->pos.x < limMatrix_min.x || !limDefined_min.x) {
                        limMatrix_min.x = itr_cell.x;
                        limDefined_min.x = true;
                    }
                    if(itr_cell->pos.y > limMatrix_max.y || !limDefined_max.y) {
                        limMatrix_max.y = itr_cell.y;
                        limDefined_max.y = true;
                    }
                    else if(itr_cell->pos.y < limMatrix_min.y || !limDefined_min.y) {
                        limMatrix_min.y = itr_cell.y;
                        limDefined_min.y = true;
                    }

                }
            }
            if(itr_row->size() > lineCountedDim.x) lineCountedDim.x = itr_row->size();
        }
        if(limDefined_min.x) assert(limDefined_max.x && " how the fuck is minimum defined but not maximum for x");
        if(limDefined_min.y) assert(limDefined_max.y && " how the fuck is minimum defined but not maximum for y");
        if(limCountedDim.x > limMatrix_max.x-limMatrix_min.x) { //the defined limits are smaller than the existing number of cells for a row
            limMatrix_max.x = lineCountedDim.x;
        }
        if(limCountedDim.y > limMatrix_max.y-limMatrix_min.y) {
            limMatrix_max.y = lineCountedDim.y;
        }

        __tableOfCells = std::vector<std::vector<__cell>>(limMatrix_max.y-limMatrix_min.y, std::vector<__cell>(limMatrix_max.x-limMatrix_min.x, {true}));
        
        Pos2d<int> lineCountedPos = limMatrix_min;
        for(auto itr_row=_matrixInput.begin(); itr_row!=_matrixInput.end(); ++itr_row) {
            lineCounterPos.x = limMatrix_min.x;
            for(auto itr_cell=itr_row->begin(); itr_cell!=itr_row->end(); ++itr_cell) {
                if(!itr_cell->__nullCell) {
                    if(itr_cell->__isDefined_pos) __tableOfCells[itr_cell->pos.y][itr_cell->pos.x] = *itr_cell;
                    else {
                        if(__tableOfCells[lineCountedPos.y][lineCountedPos.x].__nullCell) {
                            __tableOfCells[itr_cell->pos.y][itr_cell->pos.x] = *itr_cell;
                        }
                    }
                }
            }
            lineCountedPos.y++;
        }

        
        
    }
    
    
    __table_row& __table::operator[](size_t _i) {
        return __table_row(__tableOfCells[_i]);
    }
    // __table_row  operator[](size_t _i) const;
    __table_row& __table::at(size_t _i) {
        return __table_row(__tableOfCells.at(_i));
    }
    // __table_row  at(size_t _i) const;

    __cell& __table::getCell(size_t _x, size_t _y) {
        return __tableOfCells.at(_x).at(_y);
    }
    __cell __table::getCell(size_t _x, size_t _y) const {
        return getCell(_x, _y);
    }
    __cell& __table::getCell(Pos2d<int> _pos) {
        return getCell(_pos.x, _pos.y);
    }
    __cell __table::getCell(Pos2d<int> _pos) const {
        return getCell(_pos);
    }

    size_t __table::rows() {
        return __tableOfCells.size();
    }
    size_t __table::columns() {
        return (__tableOfCells.size()>0? __tableOfCells.at(0).size() : 0);
    }
    size_t __table::size() {

    }
    std::vector<__cell> __table::continuous() {
        std::vector<__cell> returVec;
        for(auto row : __tableOfCells) {
            for(auto cell : row) {
                returVec.push_back(cell);
            }
        }
        
        return returVec;
    }
    std::vector<__cell> __table::nonNull_continuous() {
        std::vector<__cell> returVec;
        for(auto row : __tableOfCells) {
            for(auto cell : row) {
                if(!cell.__nullCell) returVec.push_back(cell);
            }
        }
        
        return returVec;
    }

    int __table::addCell(size_t _x, size_t _y) {

    }
    int __table::addCell(Pos2d<int> _pos) {

    }
    int __table::moveCell(size_t _current_x, size_t _current_y, size_t _new_x, size_t _new_y) {

    }
    int __table::moveCell(Pos2d<int> _current_pos, Pos2d<int> _new_pos) {

    }
    int __table::eraseCell(size_t _x, size_t _y) {

    }
    int __table::eraseCell(Pos2d<int> _pos) {

    }

    
};
