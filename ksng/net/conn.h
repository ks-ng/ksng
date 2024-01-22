#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../util/data.h"

using namespace std;

namespace conn {

	class Connection {

		protected:

			sockaddr_in serverAddress;

		public:

			virtual ~Connection() {}

			virtual void establish() = 0;
			virtual void transmit(data::Bytes msg) = 0;
			virtual data::Bytes receive() = 0;

	};

	class TCPConnection: public Connection {

		private:

			int sock = socket(AF_INET, SOCK_STREAM, 0);

		public:

			TCPConnection(const char* ip, int port) {
				if (sock < 0) {
					notif::fatal("error building socket");
				}

				memset(&serverAddress, 0, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				inet_pton(AF_INET, ip, &serverAddress.sin_addr);
				serverAddress.sin_port = htons(port);
			}

			void establish() override {
				if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { notif::fatal("could not establish connection"); }
			}

			void transmit(data::Bytes msg) override {
				unsigned char data[msg.getLength()];
				for (int i = 0; i < msg.getLength(); i++) { data[i] = msg.get(i); }
				send(sock, data, (size_t)(msg.getLength()), 0);
			}

			data::Bytes receive() override {
				unsigned char* buffer = new unsigned char[1500];
				socklen_t addrLen = sizeof(serverAddress);
				ssize_t bytesRead = recvfrom(sock, buffer, 1500, 0, (struct sockaddr*)(&serverAddress), &addrLen);
				if (bytesRead < 0) {
					notif::error("error receiving data");
					return data::Bytes(0);
				}
				data::Bytes result(1500);
				for (int i = 0; i < 1500; i++) {
					result.set(i, buffer[i]);
				}
				return result;
			}

	};

	class UDPConnection: public Connection {

		private:

			int sock = socket(AF_INET, SOCK_DGRAM, 0);

		public:

			UDPConnection(const char* ip, int port) {
				if (sock < 0) {
					notif::fatal("error building socket");
				}

				memset(&serverAddress, 0, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				inet_pton(AF_INET, ip, &serverAddress.sin_addr);
				serverAddress.sin_port = htons(port);
			}

			void establish() override {}

			void transmit(data::Bytes msg) override {
				unsigned char data[msg.getLength()];
				for (int i = 0; i < msg.getLength(); i++) { data[i] = msg.get(i); }
				sendto(sock, data, (size_t)(msg.getLength()), 0, (struct sockaddr*)(&serverAddress), sizeof(serverAddress));
			}

			data::Bytes receive() override {
				unsigned char* buffer = new unsigned char[1500];
				socklen_t addrLen = sizeof(serverAddress);
				ssize_t bytesRead = recvfrom(sock, buffer, 1500, 0, (struct sockaddr*)(&serverAddress), &addrLen);
				if (bytesRead < 0) {
					notif::error("error receiving data");
					return data::Bytes(0);
				}
				data::Bytes result(1500);
				for (int i = 0; i < 1500; i++) {
					result.set(i, buffer[i]);
				}
				return result;
			}

	};	

};
