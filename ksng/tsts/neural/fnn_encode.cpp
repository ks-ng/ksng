#include "../../ke/neural.h"

int main() {
	neural::ff::FeedforwardNeuralNetwork n = {3, 5, 5, 5, 3};
	n.encode("testnet");
	neural::ff::FNN nn = neural::ff::FNN::decode("testnet");
}