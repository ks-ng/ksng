#include "ksng/util/fileops.h"

using namespace std;

int main() {
	data::Bytes a = fileops::readFileBytes("t.dat");
	cout << a.hex() << endl << a.truncated(2).hex() << endl;
}