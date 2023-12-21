#include "ksng/util/fileops.h"

int main() {
	datacls::Data d = fileops::readBytes("/dev/urandom", 5);
	cout << d.hex() << endl;
}