#ifndef _intent_processor_h_
#define _intent_processor_h_

#include <list>
class StepperMotor;
class Servo;

class CommandProcessor
{
private:
    QueueHandle_t m_command_queue_handle;
    void processCommand(uint16_t commandIndex);

public:
    CommandProcessor();
    void queueCommand(uint16_t commandIndex, float score);
    friend void commandQueueProcessorTask(void *param);
};

#endif
