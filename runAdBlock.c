#include "adBlock.h"

int main(int argc,char *argv[]){
	char* startChar = "<!DOCTYPE html><html><body><p > bonjour</p><script>document.innerHTML;<script></body></html>";
	char* endChar = cleanAd(startChar, (size_t)strlen(startChar));
	printf("%s\n", endChar);
	return 0;

}
