// ... of the dead.

#pragma once
#include "../net/conn.h"
#include "../crypto/ciphers/vox.h"

namespace queen {

	class Queen {

		private:

			sll::SecureLinkedList<conn::TCPConnection> connections;
			key::Key encryptionKey;
			vox::VOX cryptosystem;

		public:

			Queen(key::Key encryptionKey): encryptionKey(encryptionKey) {}

			void setup(string ip, unsigned short port) {}

	};

};