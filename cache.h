#include "includes.h"

using namespace std;

int respondBackToClient(int sockfd, char* response) {
	if (write(sockfd, response, strlen(response)) < 0) {
		printf("%s\n",response);
		fprintf(stderr, "ERROR writing to socket\n");
		int ret = 1;	
		pthread_exit(&ret);
	}

	return 0;
}

char* tryCache(int sockfd, char* requestUrl, char* message) {
	char* filename = (char*)malloc(strlen(requestUrl)+9);
	sprintf(filename, "tmp/%s.txt", requestUrl);

	printf("This should be found in cache--%s\n",filename);
	// testing the existence of file given the path
	if(access(filename, F_OK) != -1) {
		FILE *fp;
		fp = fopen(filename, "r");

		if(fp == NULL) {
			printf("Error reading cache file...\n");
			return NULL;
		}
		cout << "caching working but fucking upp" << endl;
		char responseFromCache[10001];
		responseFromCache[10000] = '\0';
		memset(responseFromCache, 0, 10000);
		int count = 0;
		while(fgets(responseFromCache, 10000, fp)) {
			//cout << count << endl;
			//printf("%s",responseFromCache);
			//puts(responseFromCache);
			respondBackToClient(sockfd, responseFromCache);
			memset(responseFromCache, 0, 10000);
		}
		fclose(fp);
		return filename;
	}
	return NULL;
}


int returnFromCache(int sockfd, char* requestUrl, char* message) {
	if(tryCache(sockfd, requestUrl, message) == NULL) {
		return 0;
	}
	return 1;
}