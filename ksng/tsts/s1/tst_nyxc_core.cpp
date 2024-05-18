#include "../nyxc/core.h"

int main() {
	nyxcore::Entity e = {1, 2, 3, 4, 5};
	nyxcore::Entity f = {2, 3, 4, 5, 6};
	nyxcore::Entity g = {3, 4, 5, 6, 7};
	nyxcore::Event evt1 = {e, f, g};
	nyxcore::Event evt2 = {f, g, e};
	nyxcore::Memory mem = {evt1, evt2};
	cout << e.entityRepr() << endl;
	cout << f.entityRepr() << endl;
	cout << nyxcore::distance(e, f) << endl;
	cout << evt1.repr() << endl;
	cout << evt2.repr() << endl;
	cout << mem.repr() << endl;
}