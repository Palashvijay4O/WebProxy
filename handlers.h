#include "includes.h"
#include "cache.h"
using namespace std;

std::map<std::string, std::string> cache_map;


char* removeSlashes(char* hostname) {

	int len = strlen(hostname);
	char* temp = (char*)malloc(strlen(hostname)+1);
	for(int i = 0; i < len; i++) {
		if(hostname[i] != '/')
			temp[i] = hostname[i];
		else
			temp[i] = '_';
	}

	return temp;
}

char* remove3WandHTTP(char* request) {

	if(strstr(request, HTTP_BEGIN) != NULL)
		return strstr(request, "http://") + 7;
	else
		return request;

}

void httpRequest(int sockfd, char* requestUrl, char* ClientSeAayaMsg) {

	//printf("Message ye aaya hai client se -->\n %s\n", ClientSeAayaMsg);
	bool browser = false;
	if(strstr(ClientSeAayaMsg, "Host:") != NULL) {
		browser = true;
	}
	int reqsock = socket(AF_INET, SOCK_STREAM, 0);
	int ret = 1;

	if(reqsock < 0) {
		cout << "Error in making request" << endl;
		pthread_exit(&ret);
	}

	struct sockaddr_in req_adr;
	memset((char*)& req_adr, 0, sizeof req_adr);

	char* requestKiCopy = (char*)malloc(strlen(requestUrl) + 1);
	strcpy(requestKiCopy, requestUrl);


	// trim off the leading http://
	char* hostname = remove3WandHTTP(requestKiCopy);

	FILE* fp;

	char *filename = (char *)malloc(strlen(hostname)+9);

	printf("The hostname which i am getting is------------------------%s\n", hostname);
	char* newhostname = (char*)malloc(strlen(hostname)+1);
	newhostname = removeSlashes(hostname);
	sprintf(filename, "tmp/%s.txt", newhostname);
	//printf("cache filed name---%s\n", filename);
	fp = fopen(filename, "w");

	if(!fp) {
		cout << "lag gayi" << endl;
	}

	//printf("Hostname : %s\n", hostname);
	char* temphostname = (char*)malloc(strlen(hostname)+1);
	strcpy(temphostname, hostname);
	char* tokens = strtok(hostname, "/");


	//printf("Hostname after tokenization------------ : %s\n", hostname);
	//string str(tokens);

	cout << "Connecting to domain\n"; line;
	printf("Tokens----------------------------%s\n",tokens);
	line;


	// Unused
	

	// used to represent an entry in the hosts database
	struct hostent *fetchHost, *proxyHost;


	//fetchHost = gethostbyname(tokens);
	proxyHost = gethostbyname(PROXY_SERVER);

	if(proxyHost == NULL) {
		cout << "Eek.. Something unexpected happened!!" << endl;
		pthread_exit(&ret);
	}

	memcpy(&req_adr.sin_addr, proxyHost->h_addr, proxyHost->h_length);
	req_adr.sin_family = AF_INET;
	req_adr.sin_port = htons(8080);

	int connectionfd  = connect(reqsock , (struct sockaddr *)&req_adr, sizeof(req_adr));
	
	if (connectionfd < 0) {
		cout << "Couldn't connect to server.. Try again later.." << endl;
		pthread_exit(&ret);
	}

	
	//printf("temphostname---------%s\n",temphostname);
	char* fullrequest = (char*)malloc(strlen(temphostname)+30);
	sprintf(fullrequest, "GET http://%s\n HTTP/1.1", temphostname);


	int writer;
	if(browser == false) {
		printf("request bhej raha hu ye ->\n%s\n", fullrequest);
		writer = write(reqsock, fullrequest, strlen(fullrequest));
	}
	else {
		printf("request bhej raha hu ye ->\n %s\n", ClientSeAayaMsg);
		writer = write(reqsock, ClientSeAayaMsg, strlen(ClientSeAayaMsg));
	}


	char responseFromProxy[10001];
	responseFromProxy[10000] = '\0';
	memset(responseFromProxy, 0, 10000);

	int n;
	cout << "Response From Proxy" << endl;
	line;

	int total_size = 0;
	
	while(1) {
		n = recv(reqsock, responseFromProxy, 10000, 0);

		trace1(n);
		if(n < 0) {
			cout << "Not able to fetch from host.. quiting" << endl;
			close(reqsock);
			pthread_exit(&ret);
			//DIE
		}
		else if(n == 0) {
			break;
		}
		else {
			respondBackToClient(sockfd, responseFromProxy);
			fprintf(fp,"%s\n",responseFromProxy);
			fflush(fp);
			total_size += n;
		}
		memset(responseFromProxy, 0, 10000);
	}
	fflush(fp);
	fclose(fp);
	

	//cout << "I am done now...." << endl;
	// SINGLE CLIENT CACHE SYSTEM
	/*string key = hostname;
	string value = filename;
	cache_map[hostname] = filename;

	cout << cache_map.size() << endl;*/
	line;

	close(reqsock);
	pthread_exit(&ret);
}


void parseRequest(int sockfd, char* message) {

	//printf("Message -------- %s\n",message);
	char* messagekicopy = (char*)malloc(strlen(message) + 1);
	strcpy(messagekicopy, message);
	char *tokens = strtok(message, " ");

	if(strcmp(tokens, "GET") != 0) {
		cout << "Not Correct HTTP Request.. quiting" << endl;
		line;
		int ret = 1;
		pthread_exit(&ret);
	}
	tokens = strtok(NULL, " ");

	char* requestUrl = (char*)malloc(strlen(tokens)+1);
	strcpy(requestUrl, tokens);
	line;
	cout << "Processing Request...Please wait..." << endl;
	line;
	printf("%s\n", requestUrl);
	line;

	char* requestKiCopy = (char*)malloc(strlen(requestUrl) + 1);
	strcpy(requestKiCopy, requestUrl);


	// trim off the leading http://. 
	char* hostname = remove3WandHTTP(requestKiCopy);

	char* cache_file = (char*)malloc(strlen(hostname)+1);

	cache_file = removeSlashes(hostname);
	//printf("hostname ----------------- %s\n", hostname);
	printf("Cache file name should be----%s\n", cache_file);
	if(returnFromCache(sockfd, cache_file, messagekicopy) == 0)
		httpRequest(sockfd, requestUrl, messagekicopy);
	
	else {
		cout << "Woah.. cache hit......" << endl;
	}
	//free(cache_file);
	//free(hostname);
	//free(requestKiCopy);
	return;
}
