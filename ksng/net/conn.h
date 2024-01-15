#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../util/data.h"

using namespace std;

namespace conn {

	class Connection {

		public:

			virtual ~Connection() {}

			virtual void establish() = 0;
			virtual void send(data::Bytes msg) = 0;
			virtual data::Bytes receive() = 0;

	};

	class TCPConnection: public Connection {

		private:

			int sock = socket(AF_INET, SOCK_STREAM, 0);

		public:

			...

	};

};