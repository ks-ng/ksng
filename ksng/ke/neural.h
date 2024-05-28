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
			ss << x / 1000000000000 << "T";
		} else if (x >= 1000000000) {
			ss << x / 1000000000 << "B";
		} else if (x >= 1000000) {
			ss << x / 1000000 << "M";
		} else if (x >= 1000) {
			ss << x / 1000 << "k";
		} else {
			ss << x;
		}
		return ss.str();
	}

	namespace core {

		double LEARNING_RATE = 0.001;

		random_device rd;
		mt19937 gen(rd());
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
					hideCursor();
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
					if (verbose) { 
						prediction = forward(question);
						cout << "\rFinal training report: " << question.repr() << " -> [expected: " << answer.repr() << ", predicted: " << prediction.repr() << "]            " << endl;
					}
					showCursor();
				}

				void train(sda::Array<Arr> questions, sda::Array<Arr> answers, int iterations, bool verbose=false) {
					hideCursor();
					Arr question;
					Arr prediction;
					Arr correctAnswer;
					Arr error;
					for (int i = 0; i < iterations; i++) {
						for (int j = 0; j < questions.getLength(); j++) {
							question = questions.get(j); 
							correctAnswer = answers.get(j);
							prediction = forward(question);
							error = Arr(prediction.getLength());
							for (int k = 0; k < error.getLength(); k++) {
								error.set(k, tanh(prediction.get(k) - correctAnswer.get(k)));
							}
							backward(error);
							if (verbose) { cout << "\r" << intrepr(i + 1) << "/" << intrepr(iterations) << ": " << question.repr() << " -> [expected: " << correctAnswer.repr() << ", predicted: " << prediction.repr() << "]            "; }
						}
					}
					if (verbose) {
						cout << "\rFinal training report:                                                                                                                              " << endl;
						for (int i = 0; i < questions.getLength(); i++) {
							question = questions.get(i); 
							correctAnswer = answers.get(i);
							prediction = forward(question);
							cout << "\t" << question.repr() << " -> [expected: " << correctAnswer.repr() << ", predicted: " << prediction.repr() << "]            " << endl;
						}
					}
					showCursor();
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

				double& bias(int index) { return biases.get(index); }
				double& weight(int i, int j) { return weights.get(i, j); }

				Arr forward(Arr input) override {
					lastInput = input;
					lastOutput = core::dot(input, weights);

					for (int i = 0; i < outputCount; i++) {
						lastOutput.set(i, lastOutput.get(i) + biases.get(i));
					}

					return lastOutput;
				}

				Arr backward(Arr error) override {
					// i hate linear algebra SO MUCH, so don't ask me how any of this works, i just know it does.

					// Gradient of the biases ??
					Arr biasGradients = error;

					// Gradient of the weights?
					Mat weightGradients = core::dot(lastInput, error);

					// Gradient of the input?
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

	namespace pr {

		class PseudorecurrentNeuralNetwork {
			
		}

	};

};