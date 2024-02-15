#include "../util/csprng.h"
#include <cmath>

using namespace std;

namespace nn {

	double sigmoid(double x) {
		return 1 / (1 + exp(-x));
	}

	class Neuron {

		private:

			sda::SDA<double> weights;
			double bias;
			int inputCount;
			double learningRate;

		public:

			Neuron() {}

			Neuron(int inputCount, double learningRate): inputCount(inputCount), learningRate(learningRate) {
				weights = sda::SDA<double>(inputCount);
				for (int i = 0; i < inputCount; i++) { weights.set(i, (double)(csprng::bytes(1).get(0))); }
				bias = (double)(csprng::bytes(1).get(0));
			}

			int getLength() { return inputCount; }
			void setWeight(int index, double value) { weights.set(index, value); }
			double getWeight(int index) { return weights.get(index); }
			void setBias(double value) { bias = value; }
			double getBias() { return bias; }

			double activate(sda::SDA<double> inputs) {
				double x = 0;
				for (int i = 0; i < inputCount; i++) {
					x += inputs.get(i) * weights.get(i);
				}
				x += bias;
				return sigmoid(x);
			}

	};

	using N = Neuron;

	class FeedforwardLayer {

		private:

			sda::SDA<Neuron> neurons;

		public:

			FeedforwardLayer(int neuronCount, int inputCount, double learningRate): neurons(sda::SDA<Neuron>(neuronCount)) {
				for (int i = 0; i < inputCount; i++) {
					neurons.set(i, Neuron(inputCount, learningRate));
				}
			}

			int getLength() { return neurons.getLength(); }

			sda::SDA<double> activate(sda::SDA<double> inputs) {
				sda::SDA<double> results;
				for (int i = 0; i < getLength(); i++) {
					results.set(i, neurons.get(i).activate(inputs));
				}
				return results;
			}

	};

	using FL = FeedforwardLayer;

	class FeedforwardNeuralNetwork {

		private:

			sda::SDA<FL> layers;

		public:

			FeedforwardNeuralNetwork(sda::SDA<int> layerSizes, double learningRate) {
				layers = sda::SDA<FL>(layerSizes.getLength());
				for (int i = 0; i < layerSizes.getLength(); i++) {
					if (i == 0) {
						layers.set(i, FeedforwardLayer(layerSizes.get(i), 1, learningRate));
					} else {
						layers.set(i, FeedforwardLayer(layerSizes.get(i), layerSizes.get(i - 1), learningRate));
					}
				}
			}

			int getLength() { return layers.getLength(); }

	};

	using FNN = FeedforwardNeuralNetwork;

};