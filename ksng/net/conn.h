#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../util/data.h"
#include "../crypto/core/cipher.h"
#include "../crypto/core/key.h"

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

			TCPConnection() {}
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

			UDPConnection() {}
			UDPConnection(string ip, int port) {
				if (sock < 0) {
					notif::fatal("error building socket");
				}

				memset(&serverAddress, 0, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr);
				setPort(port);
			}

			void establish() override { notif::warning("UDP connection established; not necessary, operation ignored"); }

			void transmit(data::Bytes msg) override {
				unsigned char data[msg.getLength()];
				for (int i = 0; i < msg.getLength(); i++) { data[i] = msg.get(i); }
				sendto(sock, data, (size_t)(msg.getLength()), 0, (struct sockaddr*)(&serverAddress), sizeof(serverAddress));
			}

			inline void setPort(int port) {
				serverAddress.sin_port = htons(port);
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

	template <typename ConnectionType, typename CipherType>
	class Encrypted: public Connection {

		// for example:
		//
		//   Encrypted<TCPConnection, VOX> myEncryptedConnection("1.2.3.4");
		//
		// works with any Connection class

		private:

			ConnectionType conn;
			CipherType cipher;
			key::Key k;
			bool keyAcquired;

		public:

			Encrypted() {}
			Encrypted(const char* ip, int port): conn(ConnectionType(ip, port)), keyAcquired(true) {}
			Encrypted(const char* ip, int port, key::Key k): conn(ConnectionType(ip, port)), k(k), keyAcquired(true) {}

			void loadKey(key::Key k_) { k = k_; keyAcquired = true; }
			void loadKey(data::Bytes rawData) { k = key::Key(rawData); keyAcquired = true; }
			void loadKey(string filename) { k = key::Key(filename); keyAcquired = true; }

			void establish() { conn.establish(); }

			void transmitRaw(data::Bytes plaintext) { conn.transmit(plaintext); }
			void transmit(data::Bytes plaintext) override { conn.transmit(cipher.encrypt(plaintext, k)); }
			data::Bytes receiveRaw() { return conn.receive(); }
			data::Bytes receive() override { return cipher.decrypt(conn.receive(), k); }

	};

};
