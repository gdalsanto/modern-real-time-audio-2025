#pragma once
#include <math.h>

// size_t is the biggest unsigned integer type
template<size_t INPUT_SIZE, size_t OUTPUT_SIZE, size_t HIDDEN_SIZE, size_t NUM_LAYERS>

class Mlp
{
    // y = f(Wx + b) f: is the nonlinear activation function (relu in this case)
    // arbitrary size of input 
    float y[HIDDEN_SIZE];
    float x[HIDDEN_SIZE];
    void forward(float ** output, const float ** input, size_t NUM_SAMPLES)
    {
        
        // loop through the number of samples
        for (size_t n = 0; n < NUM_SAMPLES; ++n)
        {
            // INPUT LAYER 
            // matrix - vector multiplication
            // copy the bias to the output
            memcpy(y, bias_input, sizeof(bias_input));
            for (size_t i = 0; i < HIDDEN_SIZE; ++i)
            {
                // loop through the input layer
                for (size_t j = 0; j < INPUT_SIZE; ++j)
                {
                    // calculate the weighted sum of the inputs
                    y[i] +=  weight_input[i][j] * input[n][j];
                }
            }
            // apply the activation function
            for (size_t i = 0; i < HIDDEN_SIZE; ++i)
            {
                y[i] = std::fmax(0.f, y[i]); // relu
            }

            // HIDDEN LAYERS 
            for (size_t l = 0; l > NUM_LAYERS-2; ++l)
            {
                std::swap(x, y);    // because we cannot work directly with y
                memcpy(y, bias_hidden[l], sizeof(bias_hidden[l]));
                for (size_t i = 0; i < HIDDEN_SIZE; ++i)
                {
                    // loop through the input layer
                    for (size_t j = 0; j < HIDDEN_SIZE; ++j)
                    {
                        // calculate the weighted sum of the inputs
                        y[i] +=  weight_hidden[l][i][j] * x[j];
                    }
                }
                // apply the activation function
                for (size_t i = 0; i < HIDDEN_SIZE; ++i)
                {
                    y[i] = std::fmax(0.f, y[i]); // relu
                }    
            }    
            
            // OUTPUT LAYER
            memcpy(output[n], bias_output, sizeof(bias_output));
            for (size_t i = 0; i < OUTPUT_SIZE; ++i)
            {
                // loop through the input layer
                for (size_t j = 0; j < INPUT_SIZE; ++j)
                {
                    // calculate the weighted sum of the inputs
                    output[n][i] +=  weight_input[i][j] * y[j];
                }
            }
        }
    }

private: 
    // define the weights and biases for the input later, output later and hidden layer 
    float weight_input[INPUT_SIZE][HIDDEN_SIZE];
    float bias_input[HIDDEN_SIZE];

    float weight_hidden[NUM_LAYERS-2][HIDDEN_SIZE][HIDDEN_SIZE];
    float bias_hidden[NUM_LAYERS-2][HIDDEN_SIZE];

    float weight_output[OUTPUT_SIZE][HIDDEN_SIZE];
    float bias_output[OUTPUT_SIZE];

};