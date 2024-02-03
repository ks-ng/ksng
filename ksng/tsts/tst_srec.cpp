#include "../util/dstruct/srec.h"
#include <iostream>

using namespace std;

int main() {
	srec::SecureRecord<int, float, double> sr(1, 1.0, 1.0);
	cout << sr.get<0>() << " " << sr.get<1>() << " " << sr.get<2>() << endl;
}