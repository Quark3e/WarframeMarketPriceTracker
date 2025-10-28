
#include "wmpt_tui.hpp"

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

enum Results {
    ToastClicked,             // user clicked on the toast
    ToastDismissed,           // user dismissed the toast
    ToastTimeOut,             // toast timed out
    ToastHided,               // application hid the toast
    ToastNotActivated,        // toast was not activated
    ToastFailed,              // toast failed
    SystemNotSupported,       // system does not support toasts
    UnhandledOption,          // unhandled option
    MultipleTextNotSupported, // multiple texts were provided
    InitializationFailure,    // toast notification manager initialization failure
    ToastNotLaunched          // toast could not be launched
};

#define COMMAND_ACTION     L"--action"
#define COMMAND_AUMI       L"--aumi"
#define COMMAND_APPNAME    L"--appname"
#define COMMAND_APPID      L"--appid"
#define COMMAND_EXPIREMS   L"--expirems"
#define COMMAND_TEXT       L"--text"
#define COMMAND_HELP       L"--help"
#define COMMAND_IMAGE      L"--image"
#define COMMAND_SHORTCUT   L"--only-create-shortcut"
#define COMMAND_AUDIOSTATE L"--audio-state"
#define COMMAND_ATTRIBUTE  L"--attribute"
#define COMMAND_INPUT      L"--input"

void print_help() {
    std::wcout << "WinToast Console Example [OPTIONS]" << std::endl;
    std::wcout << "\t" << COMMAND_ACTION << L" : Set the actions in buttons" << std::endl;
    std::wcout << "\t" << COMMAND_AUMI << L" : Set the App User Model Id" << std::endl;
    std::wcout << "\t" << COMMAND_APPNAME << L" : Set the default appname" << std::endl;
    std::wcout << "\t" << COMMAND_APPID << L" : Set the App Id" << std::endl;
    std::wcout << "\t" << COMMAND_EXPIREMS << L" : Set the default expiration time" << std::endl;
    std::wcout << "\t" << COMMAND_TEXT << L" : Set the text for the notifications" << std::endl;
    std::wcout << "\t" << COMMAND_IMAGE << L" : set the image path" << std::endl;
    std::wcout << "\t" << COMMAND_ATTRIBUTE << L" : set the attribute for the notification" << std::endl;
    std::wcout << "\t" << COMMAND_SHORTCUT << L" : create the shortcut for the app" << std::endl;
    std::wcout << "\t" << COMMAND_AUDIOSTATE << L" : set the audio state: Default = 0, Silent = 1, Loop = 2" << std::endl;
    std::wcout << "\t" << COMMAND_INPUT << L" : add an input to the toast" << std::endl;
    std::wcout << "\t" << COMMAND_HELP << L" : Print the help description" << std::endl;
}


