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


char* remove3WandHTTP(char* request) {
	/*line;line;
	printf("%s\n", request);
	line;line;*/


	/*int i = 0, count = 0;
	while(request[i++] == 'w') {
		count++;
		if(count > 3) {
			break;	
		}
	}*/
	if(strstr(request, HTTP_BEGIN) != NULL)
		return strstr(request, "http://") + 7;
	else
		return request;
	//return request + count;

}

void httpRequest(int sockfd, char* request, char* ClientSeAayaMsg) {

	printf("Message ye aaya hai client se -->\n %s\n", ClientSeAayaMsg);
	int reqsock = socket(AF_INET, SOCK_STREAM, 0);
	int ret = 1;

	if(reqsock < 0) {
		cout << "Error in making request" << endl;
		pthread_exit(&ret);
	}

	struct sockaddr_in req_adr;
	memset((char*)& req_adr, 0, sizeof req_adr);

	char* requestKiCopy = (char*)malloc(strlen(request) + 1);
	strcpy(requestKiCopy, request);


	// trim off the leading http://. 
	char* hostname = remove3WandHTTP(requestKiCopy);


	printf("Hostname : %s\n", hostname);
	char* tokens = strtok(hostname, "/");

	//string str(tokens);

	cout << "Connecting to domain\n"; line;
	printf("%s\n",tokens);
	line;

	char* fullrequest = (char*)malloc(strlen(tokens)+23);
	sprintf(fullrequest, "GET %s HTTP/1.1",tokens);

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
		cout << "Couldn't connect to server.." << endl;
		pthread_exit(&ret);
	}
	
	//cout << connectionfd << endl;
	//FILE* fp;
	//fp = fopen("temp.txt", "w");
	printf("request bhej raha hu ye ->\n %s\n", ClientSeAayaMsg);
	//fclose(fp);
	int writer = write(reqsock, ClientSeAayaMsg, strlen(ClientSeAayaMsg));

	//trace1(writer);

	char responseFromProxy[10001];
	responseFromProxy[10000] = '\0';
	memset(responseFromProxy, 0, 10000);

	int n;
	cout << "Response From Proxy" << endl;
	line;

	int total_size = 0;
	//ofstream fp;
	//fp.open("courier.txt", ios::out);
	FILE* fp;

	// TODO hostname ko sahi karna hai.. problem with slashes
	// will have to replace / with _
	char *filename = (char *)malloc(strlen(hostname)+9);
	sprintf(filename, "tmp/%s.txt", hostname);
	fp = fopen(filename, "w");

	if(!fp) {
		cout << "lag gayi" << endl;
	}
	//char serverResponse[500001];
	//serverResponse[500000] = '\0';
	while(1) {
		n = recv(reqsock, responseFromProxy, 10000, 0);
		//cout << n << endl;
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
			
			//strcat(serverResponse, responseFromProxy);
			respondBackToClient(sockfd, responseFromProxy);
			fprintf(fp,"%s\n",responseFromProxy);
			//fflush(stdout);
			//trace1(n);
			total_size += n;
		}
		memset(responseFromProxy, 0, 10000);
	}
	//fflush(NULL);
	fclose(fp);
	//free(responseFromProxy);


	//respondBackToClient(sockfd, serverResponse);
	
	/*string webPageKaKachra;
	FILE* fp;
	fp = fopen("temp2.txt", "w");
	if(fp == NULL) {
		cout << "Error opening file" << endl;
	}
	//fprintf(fp, "hiiii\n");
	while((n = read(reqsock, responseFromProxy, 1330)) > 0) {
		//webPageKaKachra.append(responseFromProxy);
		trace1(n);
		fprintf(fp,"%s\n", responseFromProxy);
		//printf("%s\n", responseFromProxy);
		//string str(responseFromProxy);
		//fp << responseFromProxy;
		//fflush(fp);
		//cout << webPageKaKachra << endl;
		
		if(n < 0) {
			cout << "Ooops... Error reading the Webpage.. Try again.." << endl;
			pthread_exit(&ret);
			break;
		}

		memset(responseFromProxy, 0, 1330);
	}*/

	// fflush(NULL);
	cout << "I am out now...." << endl;
	//cout << webPageKaKachra << endl;
	line;

	close(reqsock);
}


void parseRequest(int sockfd, char* message) {

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

	char* request = (char*)malloc(strlen(tokens)+1);
	strcpy(request, tokens);
	line;
	cout << "Processing Request...Please wait..." << endl;
	line;
	printf("%s\n", request);
	line;
	httpRequest(sockfd, request, messagekicopy);

	return;
}