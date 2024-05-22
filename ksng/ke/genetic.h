#pragma once
#include <cmath>
#include "../util/dstruct/sda.h"

namespace genetic {

	double MUTATION_RATE = 0.01;

	using Arr = sda::SecureDataArray<double>;

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

		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(-1.0, 1.0);

		double getRandom() {
			return (double)(dis(rd));
		}

		class Agent {

			private:

				double mistakes = 0;

			public:

				virtual Arr forward() = 0;
				virtual void mutate() = 0;

				double getMistakes() { return mistakes; }
				void makeMistake(double x) { mistakes += x; }

		};

		template <typename AgentType>
		class GeneticAlgorithm {

			protected:

				sda::Array<AgentType> agentPool;
			
			public:

				virtual void regenerate() = 0;
				virtual void forward() = 0;

				void test(Arr question, Arr answer) {
					Arr prediction;
					double error;
					for (int i = 0; i < agentPool.getLength(); i++) {
						error = 0;
						prediction = agentPool.get(i).forward(question);
						for (int j = 0; j < prediction.getLength(); j++) {
							error += pow(prediction.get(j) - answer.get(j), 2);
						}
						agentPool.get(i).makeMistake(error);
					}
				}

				void train(sda::Array<Arr> questions, sda::Array<Arr> answers, int iterations) {
					Arr question;
					Arr answer;
					for (int i = 0; i < iterations; i++) {
						for (int j = 0; j < questions.getLength() && j < answers.getLength(); j++) {
							question = questions.get(j);
							answer = answers.get(j);
							test(question, answer);
						}
						regenerate();
					}
				}

		};

	};

	namespace impl {

		template <typename AgentType>
		class HighlanderGeneticAlgorithm: core::GeneticAlgorithm<AgentType> {

			public:

				void regenerate() {
					AgentType best = agentPool.get(0);
					for (int i = 1; i < agentPool.getLength(); i++) {
						
					}
				}

		};

	};

};