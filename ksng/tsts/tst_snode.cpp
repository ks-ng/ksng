#include "../util/dstruct/snode.h"

using namespace snode;

int main() {
	SN<int> a("a", 1);
	SN<int> b("b", 2);
	SN<int> c("c", 3);
	SN<int> d("d", 4);
	SN<int> e("e", 5);
	b.attach(c);
	b.attach(d);
	a.attach(b);
	a.attach(e);
	cout << a.repr() << endl;
}