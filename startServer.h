#include "includes.h"
#include "handlers.h"


using namespace std;


class Proxy {
	
	int portNumber;	

	public:
		Proxy(int portNumber) {
			this->portNumber = portNumber;
		}

		~Proxy() {

		}

	/*
	 *  Function to be invoked after every successful accept request.
	 *  @param sockfdPtr
	 *  @return void*
	 */
	static void* beginExecution(void* sockfdPtr) {


		//pthread_mutex_lock(&lock);
		cout << "Connection Established Successfully!!" << endl;
		line;
		int sockfd = (intptr_t)sockfdPtr;

		int n, ret = 1;
		char buffer[1331];
		memset(buffer, 0, 1330);

		while((n = read(sockfd, buffer, 1330)) > 0) {
			string str(buffer);

			if(n < 0) {
				cout << "Error in reading request.." << endl;
				pthread_exit(&ret);
			}

			printf("Received Request:\n%s\n",buffer);

			parseRequest(sockfd, buffer);

			memset(buffer, 0, 1330);
		}

		//pthread_mutex_unlock(&lock);
		return (void*)NULL;
	}


	/*
	 *  Start the web proxy server
	 *  @return void
	 */
	void startServer() {
		int sockfd;
		struct sockaddr_in server_adr, client_adr;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);


		if(sockfd < 0) {
			std::cout << "Something strange happened... exiting!!" << std::endl;
			DIE
		}
		
		// char* sin_zero field must be set to 0 
		memset((char *)&server_adr, 0, sizeof(server_adr));

		server_adr.sin_family = AF_INET;
		// not making server bind to a particular ip
		server_adr.sin_addr.s_addr = INADDR_ANY;
		server_adr.sin_port = htons(this->portNumber);

		// bind is like providing name to the socket which was created above.
		int tempfd = bind(sockfd, (struct sockaddr *)&server_adr, sizeof(server_adr));
		if(tempfd < 0) {
			cout << "Error in binding socket..." << endl;
			DIE
		}
		//std::cout << tempfd << std::endl;
		std::cout << "Proxy server up and running..." << std::endl;
		line;

		// only applicable to sockets created using SOCK_STREAM ie
		// for connection-based sockets types.
		int l = listen(sockfd, 5);
		//std::cout << l << std::endl;

		int newsockfd;

		int count = 0;
		while(1) {
			//std::cout << "calling while again" << endl;
			socklen_t clilen = sizeof client_adr;
			newsockfd = accept(sockfd, (struct sockaddr*)& client_adr, &clilen);
			if(newsockfd < 0) {
				cout << "Error accepting connections" << endl;
			}

			// int and long on different platforms.
			pthread_t thread;
			//int ret = 1;
			//pthread_mutex_init(&lock, NULL);
			//pthread_mutex_lock(&lock);
			if(pthread_create(&thread, NULL, &Proxy::beginExecution, (void*)(intptr_t)newsockfd) < 0) {
				cout << "Error creating thread.. exiting" << endl;
				DIE
			}
			count++;
			
			//pthread_join(thread, NULL);
			//cout << count << endl;
			pthread_detach(thread);
			thread = NULL;
			//pthread_exit(&ret);
			//Proxy::beginExecution((void*)(intptr_t)newsockfd);
			//pthread_mutex_unlock(&lock);
		}
	}
};
