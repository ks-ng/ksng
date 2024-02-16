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

		public:

			Neuron() {}

			Neuron(int inputCount): inputCount(inputCount) {
				weights = sda::SDA<double>(inputCount);
				for (int i = 0; i < inputCount; i++) { weights.set(i, ((double)(csprng::bytes(1).get(0)) / 128) - 1); }
				bias = ((double)(csprng::bytes(1).get(0)) / 256) - 1;
			}

			int getLength() { return inputCount; }
			void setWeight(int index, double value) { weights.set(index, value); }
			double getWeight(int index) { return weights.get(index); }
			void setBias(double value) { bias = value; }
			double getBias() { return bias; }

			double activate(sda::SDA<double> inputs) {
				if (inputs.getLength() != inputCount) {
					stringstream msg;
					msg << "invalid number of inputs to neuron - required " << inputCount << ", got " << inputs.getLength();
					notif::fatal(msg.str());
				}
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

			FeedforwardLayer() {}

			FeedforwardLayer(int neuronCount, int inputCount): neurons(sda::SDA<Neuron>(neuronCount)) {
				for (int i = 0; i < neuronCount; i++) {
					neurons.set(i, Neuron(inputCount));
				}
			}

			int getLength() { return neurons.getLength(); }

			sda::SDA<double> activate(sda::SDA<double> inputs) {
				sda::SDA<double> results(getLength());
				for (int i = 0; i < getLength(); i++) {
					results.set(i, neurons.get(i).activate(inputs));
				}
				return results;
			}

			Neuron& get(int index) { return neurons.get(index); }

	};

	using FL = FeedforwardLayer;

	class FeedforwardNeuralNetwork {

		private:

			sda::SDA<FL> layers;
			double learningRate;

		public:

			FeedforwardNeuralNetwork(sda::SDA<int> layerSizes, double learningRate): learningRate(learningRate) {
				//cout << "NyxNG: assembling feedforward neural network ..." << endl;
				//cout << "  setting up layer SDA ...";
				layers = sda::SDA<FL>(layerSizes.getLength());
				//cout << "done (length: " << layerSizes.getLength() << ")." << endl;
				//cout << "  populating layers ..." << endl;
				for (int i = 0; i < layerSizes.getLength(); i++) {
					if (i == 0) {
						//cout << "    populating input layer at index " << i << " ...";
						layers.set(i, FeedforwardLayer(layerSizes.get(i), 1));
						cout << "done." << endl;
					} else {
						//cout << "    populating hidden layer at index " << i << " ...";
						layers.set(i, FeedforwardLayer(layerSizes.get(i), layerSizes.get(i - 1)));
						//cout << "done." << endl;
					}
				}
			}

			int getLength() { return layers.getLength(); }
			int getInputCount() { return layers.get(0).getLength(); }

			sda::SDA<double> activate(sda::SDA<double> inputs) {
				sda::SDA<double> data(getInputCount());
				for (int i = 0; i < getInputCount(); i++) {
					sda::SDA<double> x(1);
					x.set(0, inputs.get(i));
					data.set(i, layers.get(0).activate(x).get(0));
				}
				for (int i = 1; i < layers.getLength(); i++) {
					data = layers.get(i).activate(data);
				}
				return data;
			}

			void backpropagate(sda::SDA<double> errors) {
				// Iterate through each layer in reverse order (starting from the output layer)
				for (int layerIndex = getLength() - 1; layerIndex >= 0; layerIndex--) {
					FL& currentLayer = layers.get(layerIndex);

					// Calculate the deltas for neurons in the current layer
					sda::SDA<double> deltas(currentLayer.getLength());
					if (layerIndex == getLength() - 1) {
						// Output layer
						for (int neuronIndex = 0; neuronIndex < currentLayer.getLength(); neuronIndex++) {
							Neuron& neuron = currentLayer.get(neuronIndex);
							double output = neuron.activate(sda::SDA<double>(1, errors.get(neuronIndex)));
							// Derivative of the sigmoid activation function
							double derivative = output * (1 - output);
							deltas.set(neuronIndex, errors.get(neuronIndex) * derivative);
						}
					} else {
						// Hidden layers
						FL& nextLayer = layers.get(layerIndex + 1);
						for (int neuronIndex = 0; neuronIndex < currentLayer.getLength(); neuronIndex++) {
							Neuron& neuron = currentLayer.get(neuronIndex);
							double error = 0;
							for (int nextNeuronIndex = 0; nextNeuronIndex < nextLayer.getLength(); nextNeuronIndex++) {
								error += nextLayer.get(nextNeuronIndex).getWeight(neuronIndex) * deltas.get(nextNeuronIndex);
							}
							double output = neuron.activate(sda::SDA<double>(1, error));
							// Derivative of the sigmoid activation function
							double derivative = output * (1 - output);
							deltas.set(neuronIndex, error * derivative);
						}
					}

					// Update weights and biases using the deltas
					for (int neuronIndex = 0; neuronIndex < currentLayer.getLength(); neuronIndex++) {
						Neuron& neuron = currentLayer.get(neuronIndex);
						for (int weightIndex = 0; weightIndex < neuron.getLength(); weightIndex++) {
							double input = (layerIndex == 0) ? errors.get(weightIndex) : layers.get(layerIndex - 1).get(weightIndex).activate(sda::SDA<double>(1, errors.get(weightIndex)));
							double weightDelta = learningRate * deltas.get(neuronIndex) * input;
							neuron.setWeight(weightIndex, neuron.getWeight(weightIndex) + weightDelta);
						}
						// Update bias
						double biasDelta = learningRate * deltas.get(neuronIndex);
						neuron.setBias(neuron.getBias() + biasDelta);
					}
				}
			}

			void train(sda::SDA<sda::SDA<double>> inputs, sda::SDA<sda::SDA<double>> outputs, int iterations) {
				sda::SDA<double> input;
				sda::SDA<double> output;
				sda::SDA<double> prediction;
				sda::SDA<double> errors;
				for (int i = 0; i < iterations; i++) {
					for (int j = 0; j < inputs.getLength(); j++) {
						input = inputs.get(j);
						output = outputs.get(j);
						prediction = activate(input); 
						errors = sda::SDA<double>(prediction.getLength());
						for (int k = 0; k < prediction.getLength(); k++) { errors.set(k, prediction.get(k)); }
						backpropagate(errors);
						cout << prediction.repr() << " " << errors.repr() << endl;
					}
				}
			}

	};

	using FNN = FeedforwardNeuralNetwork;

};