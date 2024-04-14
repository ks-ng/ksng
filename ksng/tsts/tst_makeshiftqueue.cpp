#include "../util/dstruct/sll.h"

int main() {
	sll::SLL<int> l = {0, 1, 2, 3, 4, 5};
	cout << l.repr() << endl;
	l.remove(0);
	l.append(6);
	cout << l.repr() << endl;
}