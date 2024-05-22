#pragma once
#include <cmath>
#include <random>
#include "../util/dstruct/sda.h"
#include "../util/dstruct/sll.h"

namespace neural {

	using Arr = sda::SecureDataArray<double>;
	using Mat = sda::SecureDataMatrix<double>;

	string intrepr(int x) {
		stringstream ss;
		if (x >= 1000000000000) {
			ss << x / 1000000000000 << " trillion";
		} else if (x >= 1000000000) {
			ss << x / 1000000000 << " billion";
		} else if (x >= 1000000) {
			ss << x / 1000000 << " million";
		} else if (x >= 1000) {
			ss << x / 1000 << " thousand";
		} else {
			ss << x;
		}
		return ss.str();
	}

	namespace core {

		double LEARNING_RATE = 0.001;

		random_device rd;

		// Step 2: Initialize a random number generator (Mersenne Twister engine)
		mt19937 gen(rd());

		// Step 3: Define a distribution to generate numbers in the range [-1.0, 1.0]
		uniform_real_distribution<> dis(-1.0, 1.0);

		double getRandom() {
			return (double)(dis(rd));
		}

		class Layer {

			public:

				Layer() {}

				virtual Arr forward(Arr input) = 0;
				virtual Arr backward(Arr error) = 0;

		};

		template <typename T>
		class NeuralNetwork: public sda::SecureDataArray<T> {

			public:

				NeuralNetwork() {}

				virtual Arr forward(Arr input) = 0;
				virtual Arr backward(Arr input) = 0;

				void train(Arr question, Arr answer, int iterations, bool verbose=false) {
					Arr prediction;
					Arr error;
					for (int i = 0; i < iterations; i++) {
						prediction = forward(question);
						error = Arr(prediction.getLength());
						for (int j = 0; j < error.getLength(); j++) {
							error.set(j, tanh(prediction.get(j) - answer.get(j)));
						}
						backward(error);
						if (verbose) { cout << "\r" << intrepr(i + 1) << "/" << intrepr(iterations) << ": " << question.repr() << " -> [expected: " << answer.repr() << ", predicted: " << prediction.repr() << "]            "; }
					}
					if (verbose) { cout << endl; }
				}

				void train(sda::Array<Arr> questions, sda::Array<Arr> answers, int iterations, bool verbose=false) {
					Arr question;
					Arr prediction;
					Arr correctAnswer;
					Arr error;
					for (int i = 0; i < iterations; i++) {
						for (int j = 0; j < questions.getLength(); j++) {
							question = questions.get(j);
							prediction = forward(question);
							correctAnswer = answers.get(j);
							error = Arr(correctAnswer.getLength());
							for (int k = 0; k < correctAnswer.getLength(); k++) {
								error.set(k, correctAnswer.get(k) - prediction.get(k));
							}
							
							backward(error);
						}
					}
				}

		};

		Arr dot(Arr v, Mat m) {
			Arr r(m.getCols());

			double s;
			for (int j = 0; j < m.getCols(); j++) {
				s = 0;
				for (int i = 0; i < m.getRows(); i++) {
					s += v.get(i) * m.get(i, j);
				}
				r.set(j, s);
			}

			return r;
		}

		Mat dot(Arr a, Arr b) {
			Mat r(a.getLength(), b.getLength());
			for (int i = 0; i < a.getLength(); i++) {
				for (int j = 0; j < b.getLength(); j++) { r.set(i, j, a.get(i) * b.get(j)); } 
			}
			return r;
		}

	};

	namespace ff {

		class FullyConnectedLayer: public core::Layer {

			private:

				Arr biases;
				Mat weights;

				Arr lastInput;
				Arr lastOutput;

				int inputCount;
				int outputCount;
			
			public:
		
				FullyConnectedLayer() {}
				FullyConnectedLayer(int ic, int oc): inputCount(ic), outputCount(oc), biases(Arr(oc)), weights(Mat(ic, oc)) {
					// Randomize everything
					for (int i = 0; i < outputCount; i++) {
						biases.set(i, core::getRandom());
						for (int j = 0; j < inputCount; j++) {
							weights.set(j, i, core::getRandom());
						}
					}
				}

				Arr forward(Arr input) override {
					lastInput = input;
					lastOutput = core::dot(input, weights);

					for (int i = 0; i < outputCount; i++) {
						lastOutput.set(i, lastOutput.get(i) + biases.get(i));
					}

					return lastOutput;
				}

				Arr backward(Arr error) override {
					// Gradient of the biases
					Arr biasGradients = error;

					// Gradient of the weights
					Mat weightGradients = core::dot(lastInput, error);

					// Gradient of the input
					Arr inputGradients(inputCount);
					double s;
					for (int i = 0; i < inputCount; i++) {
						s = 0;
						for (int j = 0; j < outputCount; j++) {
							s += weights.get(i, j) * error.get(j);
						}
						inputGradients.set(i, s);
					}

					// Update weights and biases
					for (int i = 0; i < outputCount; i++) {
						biases.set(i, biases.get(i) - core::LEARNING_RATE * biasGradients.get(i));
						for (int j = 0; j < inputCount; j++) {
							weights.set(j, i, weights.get(j, i) - core::LEARNING_RATE * weightGradients.get(j, i));
						}
					}

					return inputGradients;
				}

				string reportString() {
					stringstream ss;
					ss << "<Layer with biases [" << biases.repr(", ") << "]: " << endl << weights.repr(", ") << ">";
					return ss.str();
				}

				void report() {
					cout << reportString() << endl;
				}

		};

		using FCL = FullyConnectedLayer;

		class FeedforwardNeuralNetwork: public core::NeuralNetwork<FCL> {
		
			public:

				FeedforwardNeuralNetwork(initializer_list<int> initList) {
					length = initList.size() - 1;
					elements = new FullyConnectedLayer[length];
					locked = false; // By default, not locked when initialized with a list
					securityLevel = ALERT; // Default security level
					int i = 0;
					int last = -1;
					for (const auto& val : initList) {
						if (last == -1) { last = val; continue; }
						elements[i++] = FullyConnectedLayer(last, val);
						last = val;
					}
				}

				Arr forward(Arr input) override {
					for (int i = 0; i < getLength(); i++) {
						input = get(i).forward(input);
					}
					return input;
				}

				Arr backward(Arr error) override {
					for (int i = getLength() - 1; i >= 0; i--) {
						error = get(i).backward(error);
					}
					return error;
				}

		};

		using FNN = FeedforwardNeuralNetwork;

	};

};