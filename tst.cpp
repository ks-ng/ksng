#include "ksng/util/fileops.h"

using namespace std;

int main() {
	data::Bytes a = fileops::readFile("t.dat");
	cout << a.repr() << endl;
	cout << a.hex() << endl;
	cout << data::bytesToBits(a).bin() << endl;
}