#include "../util/dstruct/sll.h"

int main() {
	sll::SLL<int> x = {1, 2, 3, 4, 5};
	for (int i = 0; i < 5; i++) {
		cout << x.get(i) << endl;
	}
}