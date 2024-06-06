#include "../../ke/neural.h"

using namespace neural;

int main() {

	ff::FNN net = {4, 10, 10, 10, 10, 10, 4};

	Arr i1 = {1, 2, 3, 4};
	Arr o1 = {2, 4, 6, 8};
	Arr i2 = {2, 4, 6, 8};
	Arr o2 = {4, 8, 12, 16};
	sda::Array<Arr> i = {i1, i2};
	sda::Array<Arr> o = {o1, o2};
	Arr ts = {3, 6, 9, 12};

	net.train(i, o, 100000, true);

	cout << net.forward(ts).repr() << endl;

}