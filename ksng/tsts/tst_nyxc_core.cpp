#include "../nyxc/core.h"

int main() {
	core::Entity e = {1, 2, 3, 4, 5};
	core::Entity f = {2, 3, 4, 5, 6};
	core::Entity g = {3, 4, 5, 6, 7};
	core::Event evt1 = {e, f, g};
	core::Event evt2 = {f, g, e};
	core::Memory mem = {evt1, evt2};
	cout << e.entityRepr() << endl;
	cout << f.entityRepr() << endl;
	cout << core::distance(e, f) << endl;
	cout << evt1.repr() << endl;
	cout << evt2.repr() << endl;
	cout << mem.repr() << endl;
}