#include "startServer.h"
#include "includes.h"

using namespace std;

int main(int argv, char** argc) {

	if(argv != 2) {
		cout << "Usage: " << argc[0] << " <port>" << endl;
		cout << "<port>: The port to run the proxy on." << endl;
		return 1;
	}
	
	//cout << atoi(argc[1]) << endl;
	
	Proxy* proxy = new Proxy(atoi(argc[1]));
	proxy->startServer();
	
	return 0;
}