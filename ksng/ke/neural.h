#pragma once

#include <random>
#include <cmath>
#include "../util/dstruct/sda.h"
#include "../util/dstruct/sll.h"

using namespace sda;
using namespace sll;

namespace neural {

	double LEARNING_RATE = 0.01;

	double getRandom() {
		return (rand() / RAND_MAX) - 0.5;
	}

	class Layer {

		public:

			virtual Array<double> forward(Array<double> input) = 0;
			virtual Array<double> backward(Array<double> outputError) = 0;

	};

	class FullyConnectedLayer: public Layer {

		private:

			Matrix<double> weights;
			Array<double> biases;

			Array<double> lastInput;
			Array<double> lastOutput;

			double inputCount; 
			double outputCount; 

		public:

			FullyConnectedLayer(int ic, int oc): inputCount(ic), outputCount(oc); {
				// Randomize everything
				weights = Matrix<double>(inputCount, outputCount);
				biases = Array<double>(outputCount);
				for (int i = 0; i < outputCount; i++) {
					biases.set(i, getRandom());
					for (int j = 0; j < inputCount; j++) {
						weights.set(j, i, getRandom());
					}
				}
			}

			Array<double> forward(Array<double> input) override {
				lastInput = input; 
				lastOutput = Array<double>(outputCount);
				
				double s;
				double x;
				for (int i = 0; i < inputCount; i++) {
					s = 0;
					x = input.get(i);
					for (int j = 0; j < outputCount; j++) {
						s += (x * weights.get(i, j)) + biases.get(j);
					}
					lastOutput.set(i, s);
				}

				// i think that's right? maybe?
				// i'll check later

				return lastOutput;
			}

			double backward(Array<double> outputError) override {
				Array<double> inputError(inputCount);
				double s;
				for (int i = 0; i < inputCount; i++) {
					s = 0;
					for (int j = 0; j < outputCount; j++) {
						s += outputError.get(i) * weights.get(i, j);
					}
					inputError.set(i, s);
				}

				// ...
				// i honestly can't tell if i'm doing this right
				// i'm just hacking off of this https://towardsdatascience.com/math-neural-network-from-scratch-in-python-d6da9f29ce65
				// gonna have to see how to do these products without numpy

			}

	};

	using FCL = FullyConnectedLayer;

	class NeuralNetwork {

		public:

	}

};