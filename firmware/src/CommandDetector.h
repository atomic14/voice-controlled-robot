#ifndef _detect_wake_word_state_h_
#define _detect_wake_word_state_h_

class I2SSampler;
class NeuralNetwork;
class AudioProcessor;
class CommandProcessor;

#define NUMBER_COMMANDS 5
#define COMMAND_WINDOW 3

class CommandDetector
{
private:
    CommandProcessor *m_command_processor;
    I2SSampler *m_sample_provider;
    NeuralNetwork *m_nn;
    AudioProcessor *m_audio_processor;
    float m_average_detect_time;
    int m_number_of_runs;
    float m_scores[COMMAND_WINDOW][NUMBER_COMMANDS];
    int m_scores_index;
    unsigned long m_last_detection;

public:
    CommandDetector(I2SSampler *sample_provider, CommandProcessor *command_processor);
    ~CommandDetector();
    void run();
};

#endif
