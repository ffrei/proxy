#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/************************************************************************************
* sources : https://en.wikibooks.org/wiki/C_Programming/C_Reference/stdlib.h/itoa   *
************************************************************************************/

/* itoa:  convert n to characters in s */
void itoa(int n, char s[],int ssize)
{
   int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
      if(i<ssize){
          s[i++] = n % 10 + '0';   /* get next digit */
      }
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0 && i<ssize)
        s[i++] = '-';
    if(i<ssize){
      s[i] = '\0';
    }else{
      s[ssize-1] = '\0';
    }

    reverse(s);
}


/* reverse:  reverse string s in place */
void reverse(char s[]){
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}



void printAll(char* s){
  int i=0;

  while(s[i]!='\0'){
    switch (s[i]) {
      case '\n':
          printf("\\n\n");
          break;
      case '\r':
          printf("\\r");
          break;
      case '\t':
          printf("\\t");
          break;
      default:
          printf("%c", s[i]);
      break;
   }
   i++;
  }
}
