#include <stdio.h>
#include<string.h>

void showstring(char **string){
    for(int i=0;i<5;i++){
        printf("%s \t",string[i]);
    }
}

int main(void)
{    
  char input[1024];
  char *string[256];            // 1) 3 is dangerously small,256 can hold a while;-) 
   for(int i=0;i<5;i++){
        string[i] = "zzz";
    }                            // You may want to dynamically allocate the pointers
                                // in a general, robust case. 
  showstring(string);
  char delimit[]="; "; // 2) POSIX whitespace characters
  int i = 0, j = 0;

  if(fgets(input, sizeof input, stdin)) // 3) fgets() returns NULL on error.
                                        // 4) Better practice to use sizeof 
                                        //    input rather hard-coding size 
  {                                        
    string[i]=strtok(input,delimit);    // 5) Make use of i to be explicit 
    // 切一个；存一个
    while(string[i]!=NULL)                    
    {
      showstring(string);
      printf("string [%d]=%s\n",i,string[i]);
      i++;
      string[i]=strtok(NULL,delimit); // 从上次切的地方开始找下一个可以切的点
    }

    for (j=0;j<i;j++)
    printf("%s", string[i]);
  }

  
  return 0;
}

