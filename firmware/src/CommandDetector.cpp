#include <Arduino.h>
#include "I2SSampler.h"
#include "AudioProcessor.h"
#include "NeuralNetwork.h"
#include "RingBuffer.h"
#include "CommandDetector.h"
#include "CommandProcessor.h"

#define WINDOW_SIZE 320
#define STEP_SIZE 160
#define POOLING_SIZE 6
#define AUDIO_LENGTH 16000
#define DETECTION_THRESHOLD -3

CommandDetector::CommandDetector(I2SSampler *sample_provider, CommandProcessor *command_procesor)
{
    m_last_detection = 0;
    m_scores_index = 0;
    m_command_processor = command_procesor;
    // save the sample provider for use later
    m_sample_provider = sample_provider;
    // some stats on performance
    m_average_detect_time = 0;
    m_number_of_runs = 0;
    // Create our neural network
    m_nn = new NeuralNetwork();
    Serial.println("Created Neral Net");
    // create our audio processor
    m_audio_processor = new AudioProcessor(AUDIO_LENGTH, WINDOW_SIZE, STEP_SIZE, POOLING_SIZE);
    // clear down the window
    for (int i = 0; i < COMMAND_WINDOW; i++)
    {
        for (int j = 0; j < NUMBER_COMMANDS; j++)
        {
            m_scores[i][j] = 0;
        }
    }
    m_scores_index = 0;

    Serial.println("Created audio processor");
}

CommandDetector::~CommandDetector()
{
    delete m_nn;
    m_nn = NULL;
    delete m_audio_processor;
    m_audio_processor = NULL;
    uint32_t free_ram = esp_get_free_heap_size();
    Serial.printf("Free ram after DetectWakeWord cleanup %d\n", free_ram);
}

void CommandDetector::run()
{
    // time how long this takes for stats
    long start = millis();
    // get access to the samples that have been read in
    RingBufferAccessor *reader = m_sample_provider->getRingBufferReader();
    // rewind by 1 second
    reader->rewind(16000);
    // get hold of the input buffer for the neural network so we can feed it data
    float *input_buffer = m_nn->getInputBuffer();
    // process the samples to get the spectrogram
    bool is_valid = m_audio_processor->get_spectrogram(reader, input_buffer);
    // finished with the sample reader
    delete reader;
    // get the prediction for the spectrogram
    m_nn->predict();
    // keep track of the previous 5 scores - about 0.5 seconds given current processing speed
    for (int i = 0; i < NUMBER_COMMANDS; i++)
    {
        float prediction = std::max(m_nn->getOutputBuffer()[i], 1e-6f);
        m_scores[m_scores_index][i] = log(is_valid ? prediction : 1e-6);
    }
    m_scores_index = (m_scores_index + 1) % COMMAND_WINDOW;
    // get the best score
    float scores[NUMBER_COMMANDS] = {0, 0, 0, 0, 0};
    for (int i = 0; i < COMMAND_WINDOW; i++)
    {
        for (int j = 0; j < NUMBER_COMMANDS; j++)
        {
            scores[j] += m_scores[i][j];
        }
    }
    // get the best score
    float best_score = scores[0];
    int best_index = 0;
    for (int i = 1; i < NUMBER_COMMANDS; i++)
    {
        if (scores[i] > best_score)
        {
            best_index = i;
            best_score = scores[i];
        }
    }
    long end = millis();
    // sanity check best score and check the cool down period
    if (best_score > DETECTION_THRESHOLD && best_index != NUMBER_COMMANDS - 1 && start - m_last_detection > 1000)
    {
        m_last_detection = start;
        m_command_processor->queueCommand(best_index, best_score);
    }
    // compute the stats
    m_average_detect_time = (end - start) * 0.1 + m_average_detect_time * 0.9;
    m_number_of_runs++;
    // log out some timing info
    if (m_number_of_runs == 100)
    {
        m_number_of_runs = 0;
        Serial.printf("Average detection time %.fms\n", m_average_detect_time);
    }
}
