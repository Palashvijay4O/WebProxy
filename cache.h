#include "includes.h"

using namespace std;

char* tryCache(char* requestUrl, char* message) {
	return NULL;
}


int returnFromCache(int sockfd, char* requestUrl, char* message) {
	if(tryCache(requestUrl, message) == NULL) {
		return 0;
	}
	return 1;
}