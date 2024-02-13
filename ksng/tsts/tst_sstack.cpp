#include "../util/dstruct/ssq.h"

int main() {
	ssq::SS<int> sq;
	sq.push(1);
	sq.push(2);
	sq.push(3);
	cout << sq.pop() << sq.pop() << sq.pop() << endl;
}