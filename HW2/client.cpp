#include <iostream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "huffmanTree.h"
using namespace std;
int port_no;
const char *hostname;
map<string, vector<int>> compressedData;
map<string, char> deCode;
void decompress(const string &it);
int main(int argc, char *argv[])
{
	// The user will execute this program using the following syntax:
	// ./ exec_filename hostname port_no < compressed_filename

	string compressed_filename;
	int max_pos = 0;
	if (argc != 3)
	{
		cout << "Usage: ./exec_filename hostname port_no < compressed_filename" << endl;
	}
	else
	{
		hostname = argv[1];
		port_no = atoi(argv[2]);
		string line;
		while (getline(cin, line))
		{
			istringstream iss(line);
			string code;
			int num;
			iss >> code;
			while (iss >> num)
			{
				compressedData[code].push_back(num);
				if (num > max_pos)
				{
					max_pos = num;
				}
			}
		}
		// creates m child threads
		int m = compressedData.size();
		thread threads[m];
		int i = 0;
		string binStr[m];
		for (auto it = compressedData.begin(); it != compressedData.end(); it++)
		{
			binStr[i++] = it->first;
		}
		for (i = 0; i < m; i++)
		{
			// create a thread to decompress the data
			threads[i] = thread(decompress, binStr[i]);
		}
		// wait for all threads to finish
		for (int i = 0; i < m; i++)
		{
			threads[i].join();
		}
		// get the decompressed data from buffer
		char buffer[1024];
		for (auto it = compressedData.begin(); it != compressedData.end(); it++)
		{
			char character = deCode[it->first];
			vector<int> position = it->second;
			for (int i = 0; i < position.size(); i++)
			{
				buffer[position[i]] = character;
			}
		}
		buffer[max_pos + 1] = '\0';
		// print the decompressed data
		// eg: Original message: CACACADBD
		cout << "Original message: " << buffer << endl;
	}
	return 0;
}

void decompress(const string &it)
{
	const string &binaryCode = it;
	vector<int> position = compressedData[binaryCode];
	// create a socket
	int client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1)
	{
		std::cerr << "Error: could not create socket\n";
		return;
	}

	// connect to the server
	struct sockaddr_in server_addr
	{
	};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_no);
	inet_pton(AF_INET, hostname, &server_addr.sin_addr);
	if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << "Error: could not connect to server\n";
		return;
	}
	// cout << "Connected to server" << endl;

	// send a message to the server
	if (send(client_socket, binaryCode.c_str(), binaryCode.length(), 0) == -1)
	{
		std::cerr << "Error: could not send data to server\n";
		close(client_socket);
		return;
	}

	// receive a response from the server
	char buffer[1024];
	int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
	if (bytes_received == -1)
	{
		std::cerr << "Error: could not receive data from server\n";
		close(client_socket);
		return;
	}
	buffer[bytes_received] = '\0'; // add null terminator to received data
								   // Write the character to the buffer
	// cout << buffer << endl;
	deCode[binaryCode] = buffer[0];

	// close the socket and exit
	close(client_socket);
}