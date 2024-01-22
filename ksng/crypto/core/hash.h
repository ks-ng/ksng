#pragma once
#include "key.h"
#include "permute.h"
#include <string>

using namespace std;

namespace hash {

	class HashFunction {

		public:

			virtual data::Bytes hash(data::Bytes plaintext) = 0;

			data::Bytes fileHash(string filename, string store=(string)("NONE")) {
				data::Bytes h = hash(fileops::readFileBytes(filename));
				if (store != (string)("NONE")) { fileops::writeFile(store, h); }
				return h;
			}

	};

	class KeyedHashFunction {

		public:

			virtual data::Bytes hash(data::Bytes plaintext, key::Key k) = 0;

			data::Bytes fileHash(string filename, key::Key k, string store=(string)("NONE")) {
				data::Bytes h = hash(fileops::readFileBytes(filename), k);
				if (store != (string)("NONE")) { fileops::writeFile(store, h); }
				return h;
			}

	};

};
