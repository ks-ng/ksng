#include "../util/dstruct/ssql.h"

using namespace std;

int main() {
	ssql::SecureLog<int> l(3);
	l.log(1);
	l.log(2);
	l.log(3);
	l.log(4);

	cout << l.get(0) << l.get(1) << l.get(2) << endl;
}