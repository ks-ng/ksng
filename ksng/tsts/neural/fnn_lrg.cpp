#include "../../ke/neural.h"

using namespace neural;

int main() {

	ff::FNN net = {5, 10, 10, 10, 10, 10, 5};

	Arr i1 = {1, 2, 3, 4, 5};
	Arr o1 = {2, 4, 6, 8, 10};
	Arr i2 = {2, 4, 6, 8, 10};
	Arr o2 = {4, 8, 12, 16, 20};
	sda::Array<Arr> i = {i1, i2};
	sda::Array<Arr> o = {o1, o2};

	net.train(i, o, 25000, true);

}