#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "data.h"
#include "notif.h"

using namespace std;

namespace fileops {

	int getFileSize(const std::string& filename) {
		ifstream file(filename, ios::binary | ios::ate);
		if (!file.is_open()) {
			// Handle error: unable to open the file
			return -1;
		}

		int size = static_cast<int>(file.tellg());
		file.close();

		return size;
	}

	data::Bytes readBytes(const string& filename, int byteCount) {
		data::Bytes result(byteCount);
		ifstream file(filename);

		char chara = 0;
		int i = 0;
		if (file.is_open()) {
			while (!file.eof() && !(i == byteCount)) {
				file.get(chara);
				result.set(i, static_cast<unsigned char>(chara));
				i++;
			}
		}

		return result;
	}

	data::Bits readBits(const string& filename, int byteCount) {
		return data::bytesToBits(readBytes(filename, byteCount));
	}

	inline data::Bytes readFileBytes(const string& filename) {
		int fileSize = getFileSize(filename);
		return readBytes(filename, fileSize);
	}

	inline data::Bits readFileBits(const string& filename) {
		return data::bytesToBits(readFileBytes(filename));
	}

	int writeFile(string filename, data::Bytes data) {
		ofstream file(filename, ios::binary | ios::trunc);
		if (!file.is_open()) {
			notif::fatal("could not open file");
			return -1;
		}

		if (!file.good()) {
			notif::fatal("fatal error: could not write to file");
			return -2;
		}

		for (int i = 0; i < data.getLength(); i++) {
			file << static_cast<char>(data.get(i));
		}

		file.close();
		return 0;
	}

};