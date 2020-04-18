#include <iostream>
#include <cstring>
using namespace std;

int parseSign(char* str, char* strpiped[256], char* Sign) 
{ 
    const char* sign = Sign;
	int i; 
    if(str!=NULL){
        strpiped[i] = strtok(str,sign);
        while(strpiped[i]!=NULL){
            i+=1;
            strpiped[i] = strtok(NULL,sign);
        }
    }
	return i;
}  

int main(){
    char *string[256];
    char *str2[256][256];
    char input[1064];
    char delimit1[] = "*;";
    char pipe[100];
    pipe[0] = "|";
    pipe[1] = "&";
    int i = 0;


    cin>>input; 


    i = parseSign(input, string, pipe[0]);
    cout<<i;
}