#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include "datacls.h"
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

	datacls::Data readBytes(const string& filename, int byteCount) {
		datacls::Data result(byteCount);
		ifstream file(filename);

		char chara = 0;
		int i = 0;
		if (file.is_open()) {
			while (!file.eof() && !(i == byteCount)) {
				file.get(chara);
				result.setByte(i, static_cast<unsigned char>(chara));
				i++;
			}
		}

		return result;
	}

	inline datacls::Data readFile(const string& filename) {
		int fileSize = getFileSize(filename);
		return readBytes(filename, fileSize);
	}

	int writeFile(string filename, datacls::Data data) {
		ofstream file(filename, ios::binary | ios::trunc);
		if (!file.is_open()) {
			notif::fatal("could not open file");
			return -1;
		}

		if (!file.good()) {
			notif::fatal("fatal error: could not write to file");
			return -2;
		}

		for (int i = 0; i < data.length(datacls::bytes); i++) {
			file << static_cast<char>(data.getByte(i));
		}

		file.close();
		return 0;
	}

};