#include "../util/node.h"

int main() {
	node::N<int> a1("a", 1, 2);
	node::N<int> b1("b", 2, 1);
	node::N<int> c1("c", 3);
	node::N<int> d1("d", 4);
	a1.addChild(b1);
	a1.addChild(c1);
	a1.getChild("b").addChild(d1);
	cout << "a1: " << a1.repr() << endl;

	node::N<int> a2("a", 1, 2);
	node::N<int> b2("b", 2, 1);
	node::N<int> c2("c", 3);
	node::N<int> d2("d", 4);
	a2.addChild(b2);
	a2.addChild(c2);
	a2.getChild("b").addChild(d2);
	cout << "a2: " << a2.repr() << endl;

	node::Node<int> a3("w", 1, 2);
	node::Node<int> b3("x", 2, 1);
	node::Node<int> c3("y", 3);
	node::Node<int> d3("z", 4);
	a3.addChild(b3);
	a3.addChild(c3);
	a3.getChild("x").addChild(d3);
	cout << "a3: " << a3.repr() << endl;

	cout << "a1 and a2 iso: " << node::isomorphic(a1, a2) << endl;
	cout << "a1 and a3 iso: " << node::isomorphic(a1, a3) << endl << endl;
}