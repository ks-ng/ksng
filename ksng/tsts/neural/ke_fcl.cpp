#include "../../ke/neural.h"

using namespace neural;

int main() {
	srand(0);

	ff::FCL l(3, 3);

	Arr input({1, 2, 3});

	for (int i = 0; i < 100; i++) {
		Arr output = l.forward(input);
		cout << output.repr() << endl;
		l.backward(Arr({output[0] - 1, output[1] - 2, output[2] - 3}));
	}
}