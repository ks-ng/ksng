#include "../../ke/neural.h"

using namespace neural;

int main() {
	srand(0);

	ff::FNN net = {3, 5, 5, 5, 3};

	Arr input({1, 2, 3});

	for (int i = 0; i < 100; i++) {
		Arr output = net.forward(input);
		cout << output.repr() << endl;
		net.backward(Arr({output[0] - 4, output[1] - 5, output[2] - 6}));
	}
}