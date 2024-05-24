#include "../../ke/neural.h"

using namespace neural;

int main() {
	ff::FNN net = {3, 5, 5, 5, 3};
	
	Arr input({1, 2, 3});

	Arr output({2, 4, 6});

	cout << net.forward(input).repr() << endl;

	net.train(input, output, 1000, true);

	cout << net.forward(input).repr() << endl;
}