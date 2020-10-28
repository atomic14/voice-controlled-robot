# Voice Command Recognition Firmware

This folder contains the firmware for the voice-controlled robot.

We are using Platform.io to build the firmware.

To understand the code the best place to start is `src/main.cpp`. This creates our `CommandDetector` and `CommandProcessor` objects and also creates a task to service the command detector as audio samples come in.

From there you can look at `src/CommandDetector.cpp` and `src/CommandProcessor.cpp`.

The code should be well commented and hopefully easy to understand.

## Config options

To set things up for yourself, edit the `config.h` file and fill in your WiFi details.

There are a number of options in this file that you can modify to suit your own setup.

If you want to use an analog microphone instead of I2S then you need to comment out this line:

```
// are you using an I2S microphone - comment this out if you want to use an analog mic and ADC input
#define USE_I2S_MIC_INPUT
```

And you will need to select the appropriate ADC channel to read data from:

```
// Analog Microphone Settings - ADC1_CHANNEL_7 is GPIO35
#define ADC_MIC_CHANNEL ADC1_CHANNEL_7
```

If you are using an I2S Microphone then you need to tell the system which channel you have configure the microphone on (left or right - generally these devices default to left).

```
// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or I2S_CHANNEL_FMT_ONLY_RIGHT
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_RIGHT
```

And you will need to tell it which pins you have connected to the microphone:

```
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_33
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_26
#define I2S_MIC_SERIAL_DATA GPIO_NUM_25
```
