
struct MlpParameters {

    float weight_input[INPUT_SIZE*HIDDEN_SIZE];
    float bias_input[HIDDEN_SIZE];

    float weight_hidden[NUM_LAYERS-2][HIDDEN_SIZE][HIDDEN_SIZE];
    float bias_hidden[NUM_LAYERS-2][HIDDEN_SIZE];

    float weight_output[OUTPUT_SIZE][HIDDEN_SIZE];
    float bias_output[OUTPUT_SIZE];

}