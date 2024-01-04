#include "ksng/util/node.h"

int main() {
	node::Node<int> a("a", 1, 3);
	node::Node<int> b("b", 2, 1);
	node::Node<int> c("c", 3);
	node::Node<int> d("d", 4);
	a.addChild(b);
	a.addChild(c);
	cout << a.getChild("b").repr() << endl;
	a.getChild("b").addChild(d);
	cout << a.repr();
}