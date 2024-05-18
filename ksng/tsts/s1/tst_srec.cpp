#include "../util/dstruct/srec.h"
#include <iostream>

using namespace std;

int main() {
	sda::SDA<string> names(3);
	names.set(0, (string)("hello"));
	names.set(1, (string)("world"));
	names.set(2, (string)("fubar"));
	srec::SecureRecord<int> sr(names);
	sr.set((string)("hello"), 1);
	sr.set((string)("world"), 2);
	sr.set((string)("fubar"), 3);
	cout << sr.repr() << endl;
}