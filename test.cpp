#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Activation function (sigmoid)
double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

// Derivative of the activation function (sigmoid)
double sigmoidDerivative(double x) {
    return sigmoid(x) * (1 - sigmoid(x));
}

class NeuralNetwork {
private:
    int inputNodes;
    int hiddenNodes;
    int outputNodes;
    double learningRate;

    // Weights
    vector<vector<double>> weightsInputHidden;
    vector<vector<double>> weightsHiddenOutput;

    // Biases
    vector<double> biasesHidden;
    vector<double> biasesOutput;

public:
    NeuralNetwork(int input, int hidden, int output, double rate) {
        inputNodes = input;
        hiddenNodes = hidden;
        outputNodes = output;
        learningRate = rate;

        // Initialize weights and biases with random values
        weightsInputHidden.resize(inputNodes, vector<double>(hiddenNodes));
        weightsHiddenOutput.resize(hiddenNodes, vector<double>(outputNodes));
        biasesHidden.resize(hiddenNodes);
        biasesOutput.resize(outputNodes);

        for (int i = 0; i < inputNodes; ++i) {
            for (int j = 0; j < hiddenNodes; ++j) {
                weightsInputHidden[i][j] = (double) rand() / RAND_MAX - 0.5; // random weight between -0.5 and 0.5
            }
        }

        for (int i = 0; i < hiddenNodes; ++i) {
            for (int j = 0; j < outputNodes; ++j) {
                weightsHiddenOutput[i][j] = (double) rand() / RAND_MAX - 0.5; // random weight between -0.5 and 0.5
            }
        }

        for (int i = 0; i < hiddenNodes; ++i) {
            biasesHidden[i] = (double) rand() / RAND_MAX - 0.5; // random bias between -0.5 and 0.5
        }

        for (int i = 0; i < outputNodes; ++i) {
            biasesOutput[i] = (double) rand() / RAND_MAX - 0.5; // random bias between -0.5 and 0.5
        }
    }

    // Feedforward function
    vector<double> feedforward(const vector<double>& inputs) {
        // Hidden layer
        vector<double> hidden(hiddenNodes);
        for (int i = 0; i < hiddenNodes; ++i) {
            double sum = 0;
            for (int j = 0; j < inputNodes; ++j) {
                sum += inputs[j] * weightsInputHidden[j][i];
            }
            hidden[i] = sigmoid(sum + biasesHidden[i]);
        }

        // Output layer
        vector<double> outputs(outputNodes);
        for (int i = 0; i < outputNodes; ++i) {
            double sum = 0;
            for (int j = 0; j < hiddenNodes; ++j) {
                sum += hidden[j] * weightsHiddenOutput[j][i];
            }
            outputs[i] = sigmoid(sum + biasesOutput[i]);
        }

        return outputs;
    }

    // Train the neural network using backpropagation
    void train(const vector<double>& inputs, const vector<double>& targets) {
        // Feedforward
        vector<double> hidden(hiddenNodes);
        for (int i = 0; i < hiddenNodes; ++i) {
            double sum = 0;
            for (int j = 0; j < inputNodes; ++j) {
                sum += inputs[j] * weightsInputHidden[j][i];
            }
            hidden[i] = sigmoid(sum + biasesHidden[i]);
        }

        vector<double> outputs(outputNodes);
        for (int i = 0; i < outputNodes; ++i) {
            double sum = 0;
            for (int j = 0; j < hiddenNodes; ++j) {
                sum += hidden[j] * weightsHiddenOutput[j][i];
            }
            outputs[i] = sigmoid(sum + biasesOutput[i]);
        }

        // Backpropagation
        // Calculate output layer errors
        vector<double> outputErrors(outputNodes);
        for (int i = 0; i < outputNodes; ++i) {
            outputErrors[i] = (targets[i] - outputs[i]) * sigmoidDerivative(outputs[i]);
        }

        // Calculate hidden layer errors
        vector<double> hiddenErrors(hiddenNodes);
        for (int i = 0; i < hiddenNodes; ++i) {
            double error = 0;
            for (int j = 0; j < outputNodes; ++j) {
                error += outputErrors[j] * weightsHiddenOutput[i][j];
            }
            hiddenErrors[i] = error * sigmoidDerivative(hidden[i]);
        }

        // Update weights and biases
        for (int i = 0; i < hiddenNodes; ++i) {
            for (int j = 0; j < outputNodes; ++j) {
                weightsHiddenOutput[i][j] += learningRate * outputErrors[j] * hidden[i];
            }
        }

        for (int i = 0; i < inputNodes; ++i) {
            for (int j = 0; j < hiddenNodes; ++j) {
                weightsInputHidden[i][j] += learningRate * hiddenErrors[j] * inputs[i];
            }
        }

        for (int i = 0; i < outputNodes; ++i) {
            biasesOutput[i] += learningRate * outputErrors[i];
        }

        for (int i = 0; i < hiddenNodes; ++i) {
            biasesHidden[i] += learningRate * hiddenErrors[i];
        }
    }
};

int main() {
    // Example usage of the neural network
    srand(time(NULL)); // seed random number generator

    NeuralNetwork neuralNetwork(2, 3, 1, 0.1); // 2 input nodes, 3 hidden nodes, 1 output node, learning rate 0.1

    // Training data (XOR function)
    vector<vector<double>> trainingDataInputs = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<vector<double>> trainingDataTargets = {{0}, {1}, {1}, {0}};

    // Training
    for (int epoch = 0; epoch < 1000000; ++epoch) {
        for (int i = 0; i < trainingDataInputs.size(); ++i) {
            neuralNetwork.train(trainingDataInputs[i], trainingDataTargets[i]);
        }
    }

    // Testing
    for (int i = 0; i < trainingDataInputs.size(); ++i) {
        vector<double> inputs = trainingDataInputs[i];
        vector<double> outputs = neuralNetwork.feedforward(inputs);

        cout << "Input: " << inputs[0] << ", " << inputs[1] << " -> Output: " << outputs[0] << endl;
    }

    return 0;
}
