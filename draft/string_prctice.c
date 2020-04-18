#include <stdio.h>
#include <string.h>

void showstring(char **string){
    for(int i=0;i<5;i++){
        printf("%s \n",string[i]);
    }
}

int main(){
    char *string[256];
    for(int i=0;i<5;i++){
        string[i] = "zzz";
    }

    showstring(string);
    return 0;


}

