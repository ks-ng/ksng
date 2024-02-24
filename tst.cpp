#include <iostream>
#include <vector>
#include <cmath>

// Define activation functions
double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

// Define the Feedforward Neural Network class
class FeedforwardNeuralNetwork {
private:
    std::vector<std::vector<double>> weights;
    std::vector<double> biases;
    std::vector<int> layer_sizes;
    int num_layers;

public:
    FeedforwardNeuralNetwork(const std::vector<int>& sizes) : layer_sizes(sizes), num_layers(sizes.size()) {
        // Initialize weights randomly and biases to zeros
        for (int i = 1; i < num_layers; ++i) {
            std::vector<double> layer_weights;
            for (int j = 0; j < sizes[i] * sizes[i - 1]; ++j) {
                layer_weights.push_back(rand() / double(RAND_MAX));
            }
            weights.push_back(layer_weights);
            biases.push_back(0.0);
        }
    }

    std::vector<double> feedforward(const std::vector<double>& input) {
        std::vector<double> activations = input;
        for (int i = 0; i < num_layers - 1; ++i) {
            std::vector<double> new_activations(layer_sizes[i + 1], 0.0);
            for (int j = 0; j < layer_sizes[i + 1]; ++j) {
                double neuron_input = biases[i] + dot_product(weights[i], activations, j * layer_sizes[i]);
                new_activations[j] = sigmoid(neuron_input);
            }
            activations = new_activations;
        }
        return activations;
    }

private:
    double dot_product(const std::vector<double>& a, const std::vector<double>& b, int start_index) {
        double result = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            result += a[i] * b[start_index + i];
        }
        return result;
    }
};

int main() {
    std::vector<int> layer_sizes = {2, 3, 1}; // Input layer: 2 neurons, Hidden layer: 3 neurons, Output layer: 1 neuron
    FeedforwardNeuralNetwork nn(layer_sizes);

    std::vector<double> input = {0.5, 0.3};
    std::vector<double> output = nn.feedforward(input);

    std::cout << "Output: " << output[0] << std::endl;

    return 0;
}
