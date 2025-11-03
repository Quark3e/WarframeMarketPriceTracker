
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
    __table_row::__table_row(__table_row&& _toMove) {
        std::swap(__tableCellRow, _toMove.__tableCellRow);
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
        for(auto itr=_matrixInput.begin(); itr!=_matrixInput.end(); ++itr) {
            __tableOfCells.push_back(*itr);
        }
        Pos2d<int> tableDim(0, __tableOfCells.size());
        Pos2d<int> tempPos(0, 0);
        for(auto& itr_y=__tableOfCells.begin(); itr_y!=__tableOfCells.end(); ++itr_y) {
            tempPos.x = 0;
            for(auto& itr=itr_y->begin(); itr!=itr_y->end(); ++(itr)) {
                if(itr->__nullCell) {

                }
                else {
                    if(itr->__isDefined_pos) {

                    }
                    else {
                        itr->pos = tempPos;
                        itr->__isDefined_pos = true;
                    }

                }

                tempPos.x++;
            }
            tempPos.y++;
        }
    }

    __table::__table() {

    }
    __table::__table(const __table& _toCopy) {

    }
    __table::__table(__table&& _toMove) {
        
    }
    __table::~__table() {

    }
    __table& __table::operator=(const __table& _toCopy) {

    }
    __table& __table::operator=(__table&& _toMove) {

    }

    
};
