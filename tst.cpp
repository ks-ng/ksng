#include "ksng/util/node.h"

int main() {
	node::Node<int> a("a", 1, 2);
	node::Node<int> b("b", 2);
	node::Node<int> c("c", 3);
	a.addChild(b);
	a.addChild(c);
	cout << a.repr() << endl;
	cout << a.getChild("b").repr() <<endl;
}