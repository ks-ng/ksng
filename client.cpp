#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main() {
	// Create a socket
	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
		perror("Error creating socket");
		exit(EXIT_FAILURE);
	}

	// Connect to the server
	sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
	serverAddress.sin_port = htons(PORT);

	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		perror("Error connecting to server");
		close(clientSocket);
		exit(EXIT_FAILURE);
	}

	std::cout << "Connected to server" << std::endl;

	// Communication loop
	char buffer[BUFFER_SIZE];
	while (true) {
		// Send a message to the server
		std::cout << "Client: ";
		std::cin.getline(buffer, sizeof(buffer));
		send(clientSocket, buffer, strlen(buffer), 0);

		// Receive response from the server
		ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0) {
			std::cout << "Connection closed by server" << std::endl;
			break;
		}

		// Display server's response
		buffer[bytesRead] = '\0';
		std::cout << "Server: " << buffer << std::endl;
	}

	// Close the socket
	close(clientSocket);

	return 0;
}
