#include "../util/dstruct/ssq.h"

int main() {
	ssq::SQ<int> sq;
	sq.enq(1);
	sq.enq(2);
	sq.enq(3);
	cout << sq.deq() << sq.deq() << sq.deq() << endl;
}