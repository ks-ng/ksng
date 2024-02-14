#include "../util/dstruct/sll.h"
#include <iostream>

using namespace std;

int main() {
	sll::SLL<int> l;
	l.append(1);
	l.append(2);
	l.append(3);
	cout << l.get(0) << l.get(1) << l.get(2) << endl;
	l.remove(1);
	l.append(4);
	cout << l.get(0) << l.get(1) << l.get(2) << endl;
}
