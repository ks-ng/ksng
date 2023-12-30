#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
	// Create a socket
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		perror("Error creating socket");
		exit(EXIT_FAILURE);
	}

	// Bind the socket
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(PORT);

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		perror("Error binding socket");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if (listen(serverSocket, 5) == -1) {
		perror("Error listening for connections");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}

	std::cout << "Server is listening on port " << PORT << std::endl;

	// Accept a connection
	sockaddr_in clientAddress{};
	socklen_t clientAddrSize = sizeof(clientAddress);
	int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrSize);
	if (clientSocket == -1) {
		perror("Error accepting connection");
		close(serverSocket);
		exit(EXIT_FAILURE);
	}

	std::cout << "Client connected" << std::endl;

	// Communication loop
	char buffer[BUFFER_SIZE];
	while (true) {
		// Receive data from the client
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0) {
			std::cout << "Connection closed by client" << std::endl;
			break;
		}

		// Display received message
		buffer[bytesRead] = '\0';
		std::cout << "Client: " << buffer << std::endl;

		// Send a response
		std::cout << "Server: ";
		std::cin.getline(buffer, sizeof(buffer));
		send(clientSocket, buffer, strlen(buffer), 0);
	}

	// Close sockets
	close(clientSocket);
	close(serverSocket);

	return 0;
}
