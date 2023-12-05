#include "ksng/utils.cpp"

int main() {
	Bytestring data = readFile("t.dat");
	std::cout << data.hexadecimal();
}