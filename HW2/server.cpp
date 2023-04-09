#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include <signal.h>
#include <sys/wait.h>
#include "huffmanTree.h"
using namespace std;

std::mutex my_mutex; 

void sigchld_handler(int signum) //SIGCHLD
{
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;
}

void process(int sockfd, HuffmanTree huffmanTree)
{
	// Receive and process data sent from the client
	char buffer[256];
	bzero(buffer, sizeof(buffer));
	int n = recv(sockfd, buffer, sizeof(buffer), 0);
	if (n < 0)
	{
		cerr << "Failed to receive data" << endl;
		return;
	}
	string str(buffer);
	// Send processing results back to the client
	char decode = huffmanTree.decode(str);
	send(sockfd, &decode, sizeof(char), 0);
}

// The user will execute this program using the following syntax:
// ./exec_filename port_no < input_filename
int main(int argc, char *argv[])
{
	int port_no;
	string input_filename;
	HuffmanTree huffmanTree;
	if (argc == 2)
	{
		port_no = atoi(argv[1]);
		huffmanTree.readFreqFromInput(cin);
		huffmanTree.createTree();
		huffmanTree.printCodes();
	}
	else
	{
		cout << "Usage: ./exec_filename port_no < input_filename" << endl;
		return 0;
	}
	// Create socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cerr << "Failed to create socket" << endl;
		return 1;
	}
	// SO_REUSEADDR
	int reuseaddr = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1)
	{
		cerr << "Failed to set SO_REUSEADDR option" << endl;
		return 1;
	}
	// Bind port
	struct sockaddr_in serv_addr;
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (::bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		cerr << "Failed to bind port" << endl;
		return 1;
	}

	// Listen on port
	if (listen(sockfd, SOMAXCONN) == -1)
	{
		cerr << "Failed to listen on socket" << endl;
		return 1;
	}

	// Accept connection requests
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	// SIGCHLD
	signal(SIGCHLD, sigchld_handler);

	while (true)
	{
		int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
		if (clientfd == -1)
		{
			cerr << "Failed to accept connection request" << endl;
			return 1;
		}

		// Create child process to handle connection request
		pid_t pid = fork();
		if (pid == 0)
		{
			// Child process handles connection request
			close(sockfd); // Close listening socket
			process(clientfd, huffmanTree);
			close(clientfd); // Processing complete, close client socket
			_exit(0);		 // Child process exit
		}
		else if (pid < 0)
		{
			cerr << "Failed to create child process" << endl;
			return 1;
		}
		else
		{
			// Parent process closes client socket and continues to listen on port
			// use mutex to avoid race condition
			my_mutex.lock();
			close(clientfd);
			my_mutex.unlock();
		}
	}
	return 0;
}