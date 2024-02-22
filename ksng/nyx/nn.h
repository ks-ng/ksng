#include <random>
#include <climits>
#include <cmath>
#include <ctime>
#include "../util/csprng.h"

using namespace std;

namespace nn {

	double sigmoid(double x) { return 1 / (1 + exp(-x)); }
	double sigmoidDerivative(double x) { return sigmoid(x) * (1 - sigmoid(x)); }
	double randomNumber(int precision=4) {
		double sum = 0.0;
		int byte;
		for (int i = 0; i < precision; i++) {
			byte = csprng::bytes(1).get(0);
			if (byte < 128) {
				sum += byte;
			} else {
				sum -= byte - 128;
			}
		}
		return sum / (256.0 * (double)(precision));
	}

	class Neuron {

		private:

			sda::SDA<double> weights;
			double bias;
			double lastWeightedInput;

		public:

			Neuron() {}

			Neuron(int inputCount): weights(sda::SDA<double>(inputCount)), bias(randomNumber()) {
				for (int i = 0; i < getLength(); i++) { weights.set(i, randomNumber()); }
			}

			double getBias() { return bias; }
			void setBias(double value) { bias = value; }
			double getWeight(int index) { return weights.get(index); }
			void setWeight(int index, double value) { weights.set(index, value); }
			int getLength() { return weights.getLength(); }
			double getLastWeightedInput() { return lastWeightedInput; }

			double forward(sda::SDA<double> inputs) {
				double sum = 0;
				for (int i = 0; i < getLength(); i++) {
					sum += inputs.get(i) * weights.get(i);
				}
				lastWeightedInput = sum;
				return sigmoid(lastWeightedInput + bias);
			}

	};

	using N = Neuron;

	namespace feedforward {

		class FeedforwardLayer {

			public:

				sda::SDA<N> neurons;

				FeedforwardLayer() {}

				FeedforwardLayer(int neuronCount, int inputCount): neurons(sda::SDA<N>(neuronCount)) {
					for (int i = 0; i < neuronCount; i++) { neurons.set(i, Neuron(inputCount)); }
				}

				int getLength() { return neurons.getLength(); }

				Neuron get(int index) { return neurons.get(index); }

				sda::SDA<double> forward(sda::SDA<double> inputs) {
					sda::SDA<double> results(getLength());
					for (int i = 0; i < getLength(); i++) {
						results.set(i, neurons.get(i).forward(inputs));
					}
					return results;
				}

		};

		using FL = FeedforwardLayer;

		class FeedforwardNeuralNetwork {

			public:

				sda::SDA<FL> layers;
				double learningRate;

				FeedforwardNeuralNetwork(sda::SDA<int> layerSizes, double learningRate): layers(sda::SDA<FL>(layerSizes.getLength())), learningRate(learningRate) {
					for (int i = 0; i < getLength(); i++) {
						if (i == 0) {
							layers.set(i, FeedforwardLayer(layerSizes.get(0), 1));
						} else {
							layers.set(i, FeedforwardLayer(layerSizes.get(i), layerSizes.get(i - 1)));
						}
					}
				}

				int getLength() { return layers.getLength(); }

				FeedforwardLayer get(int index) { return layers.get(index); }

				sda::SDA<double> forward(sda::SDA<double> inputs) {
					for (int i = 0; i < getLength(); i++) { inputs = layers.get(i).forward(inputs); }
					return inputs;
				}

				void backward(sda::SDA<double> errors) {
					sda::SDA<double> newErrors;
					FL currentLayer, previousLayer;
					Neuron neuron;
					double neuronWeight, error, derivative;
					for (int i = getLength() - 1; i >= 1; i--) {
						currentLayer = get(i);
						previousLayer = get(i - 1);
						newErrors = sda::SDA<double>(previousLayer.getLength());
						for (int j = 0; j < previousLayer.getLength(); j++) { newErrors.set(j, 0.0); }
						for (int j = 0; j < currentLayer.getLength(); j++) {
							neuron = currentLayer.get(j);
							derivative = sigmoidDerivative(neuron.getLastWeightedInput());
							neuron.setBias(neuron.getBias() - (errors.get(j) * learningRate * derivative));
							for (int k = 0; k < previousLayer.getLength(); k++) {
								newErrors.set(k, newErrors.get(k) + (errors.get(j) * neuron.getWeight(k) * derivative));
								neuron.setWeight(k, neuron.getWeight(k) - (errors.get(j) * learningRate * derivative));
							}
						}
						errors = newErrors;
					}
				}

				void train(sda::SDA<sda::SDA<double>> inputs, sda::SDA<sda::SDA<double>> outputs, int iterations) {
					sda::SDA<double> input, output, prediction, errors;
					cout << "\e[?25l"; // hides the cursos
					for (int i = 0; i < iterations; i++) {
						for (int j = 0; j < inputs.getLength(); j++) {
							cout << "Training iteration " << (i + 1) << "/" << iterations << " (" << floor((i + 1) * 100 / iterations) << "%): ";
							cout << "loading training dataset " << j << " ...";
							input = inputs.get(j);
							output = outputs.get(j);
							cout << "successful, training ...";
							prediction = forward(input);
							cout << "successful, computing errors ...";
							errors = sda::SDA<double>(output.getLength());
							for (int k = 0; k < errors.getLength(); k++) {
								errors.set(k, prediction.get(k) - output.get(k));
							}
							backward(errors);
							cout << "\r";
						}
					}
					cout << "\e[?25h" << endl; // fixes the cursos
				}

		};

		using FNN = FeedforwardNeuralNetwork;

	};

};