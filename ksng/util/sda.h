#pragma once
#include <sstream>
#include <cstring>
#include "notif.h"

using namespace std;

namespace sda {

	bool DISALLOW_OUT_OF_RANGE = true;

	template <typename T>
	class SecureDataArray {

		private:

			T* elements;
			int length;
			bool locked = false;
			Severity securityLevel;

		public:

			// Constructors & destructors

			explicit SecureDataArray() {}
			explicit SecureDataArray(int length_, bool locked_=false, Severity securityLevel_=ALERT) { initialize(length_, locked_, securityLevel_); }

			void initialize(int length_, bool locked_=false, Severity securityLevel_=ALERT) { 
				length = length_;
				elements = new T[length];
				locked = locked_;
				securityLevel = securityLevel_;
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure data array", securityLevel);
				} 
			}

			// Access

			T& get(int index) {
				securityCheck();
				if ((index >= getLength() || index < 0) && DISALLOW_OUT_OF_RANGE) {
					notif::fatal("index out of range trying to get item from data array (segfault prevented)");
					return elements[0];
				}
				return elements[index];
			}

			void set(int index, T value) {
				securityCheck();
				if (index >= getLength() || index < 0) {
					notif::fatal("index out of range trying to set item of data array (segfault prevented)");
				}
				elements[index] = value;
			}

			int getLength() {
				securityCheck();
				return length;
			}

			// Utility

			string repr(string delimiter=(string)(" ")) {
				stringstream ss;
				for (int i = 0; i < getLength(); i++) {
					ss << get(i);
					if (i != getLength() - 1) {
						ss << delimiter;
					}
				}
				return ss.str();
			}

			SecureDataArray<T> subarray(int a, int b) {
				if (b < a) {
					return subarray(b, a);
				} else if (a == b) {
					SecureDataArray<T> result(1);
					result.set(0, get(a));
					return result;
				} else if (a < b) {
					SecureDataArray<T> result(b - a);
					for (int i = 0; i < b - a; i++) {
						result.set(i, get(a + i));
					}
					return result;
				} else {
					notif::fatal("impossible state: a < b and b > a; recommend system reboot");
				}
			}

			SecureDataArray<T> reverse() {
				int len = getLength();
				SecureDataArray<T> result(len);
				for (int i = 0; i < len; i++) { result.set((len - 1) - i, get(i)); }
				return result;
			}

			bool has(T value) {
				for (int i = 0; i < getLength(); i++) { if (get(i) == value) { return true; } }
				return false;
			}

	};

	template <typename T>
	using SDA = SecureDataArray<T>;

	template <typename T>
	class SecureDataMatrix {

		private:

			T** elements;
			int rows;
			int cols;
			bool locked = false;
			Severity securityLevel;

		public:

			// Constructors

			SecureDataMatrix() {}
			SecureDataMatrix(int rows, int cols, bool locked=false, Severity securityLevel=ALERT) { initialize(rows, cols, locked, securityLevel); }
			void initialize(int rows_, int cols_, bool locked_, Severity securityLevel_) {
				rows = rows_;
				cols = cols_;
				locked = locked_;
				securityLevel = securityLevel_;
				elements = new T*[rows];
				for (int i = 0; i < rows; i++) {
					elements[i] = new T[cols];
				}
			}

			// Security

			void reveal() { locked = false; }
			void hide() { locked = true;}
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure data matrix", securityLevel);
				} 
			}

			// Access

			T& getref(int row, int col) {
				securityCheck();
				if (row >= rows || col >= cols || row < 0 || col < 0) {
					notif::fatal("index out of range trying to get item from data matrix (segfault prevented)");
					return elements[0][0];
				}
				return elements[row][col];
			}

			T get(int row, int col) {
				securityCheck();
				if (row >= rows || col >= cols || row < 0 || col < 0) {
					notif::fatal("index out of range trying to get item from data matrix (segfault prevented)");
					return elements[0][0];
				}
				return elements[row][col];
			}

			void set(int row, int col, T value) {
				securityCheck();
				if (row >= rows || col >= cols || row < 0 || col < 0) {
					notif::fatal("index out of range trying to set item of data array (segfault prevented)");
				}
				elements[row][col] = value;
			}

			int getRows() { securityCheck(); return rows; }
			int getCols() { securityCheck(); return cols; }
			int getArea() { securityCheck(); return rows * cols; }

			SecureDataArray<T> getRow(int row) {
				SDA<T> result(cols);
				for (int col = 0; col < cols; col++) {
					result.set(col, get(row, col));
				}
				return result;
			}

			SecureDataArray<T> getCol(int col) {
				SDA<T> result(rows);
				for (int row = 0; row < rows; row++) {
					result.set(get(row, col));
				}
				return result;
			}

	};

	template <typename T>
	using SDM = SecureDataMatrix<T>;

};