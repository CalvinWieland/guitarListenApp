#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

// callback function for input and output
int recordCallback(void *outputBuffer, void *inputBuffer,
                   unsigned int nFrames, double streamTime,
                   RtAudioStreamStatus status, void *userData) {
    // cast the input buffer to a double *
    float* buffer = (float *)inputBuffer;

    std::cout << "Value of inputBuffer[0]: " << buffer[0] << std::endl;

    float sumSquares = 0.0f;
    for (unsigned int i = 0; i < nFrames; ++i) {
        float sample = buffer[i];
        sumSquares += sample * sample;
    }

    // ChatGPT generated rms and decibel calculation
    float rms = std::sqrt(sumSquares / (nFrames));
    float volumeDb = 20 * std::log10(rms + 1e-6f) + 80;

    std::cout << "Decibels: " << volumeDb << std::endl;
    std::cout << "RMS: " << rms << std::endl;

    // clears console
    //std::cout << "\033[2J\033[H";
    std::cout << std::endl << std::endl;

    // Display volume
    for (int i = 0; i < volumeDb / 5.0; i++) {
        std::cout << "█";
    }

    std::cout << std::endl << std::endl;

    return 0;
}

int main() {
    RtAudio audio;

    if (audio.getDeviceCount() < 1) {
        std::cout << "No audio devices found" << std::endl;
        return 1;
    }

    RtAudio::StreamParameters inputParams;
    inputParams.deviceId = audio.getDefaultInputDevice();
    inputParams.deviceId = 132;
    // using mono input
    inputParams.nChannels = 1;
    inputParams.firstChannel = 0;

    // Are 44,100 samples per second, 256 samples in a buffer
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256;

    try {
        audio.openStream(nullptr, &inputParams, RTAUDIO_FLOAT32,
                         sampleRate, &bufferFrames, &recordCallback);
        audio.startStream();

        // get the device IDs and names of every available device
        std::vector<unsigned int> deviceIDs = audio.getDeviceIds();
        for (std::vector<unsigned int>::iterator it = deviceIDs.begin(); it != deviceIDs.end(); ++it) {
            std::cout << "Device ID: " << *it << " " << audio.getDeviceInfo(*it).name << std::endl;
        }

        // list the default input device
        unsigned int defaultInputDevice = audio.getDefaultInputDevice();
        RtAudio::DeviceInfo info = audio.getDeviceInfo(defaultInputDevice);
        std::cout << "Default input device: " << info.name << " ID " << defaultInputDevice << std::endl;

        std::cout << "Press enter to stop recording." << std::endl;
        std::cin.get();

        audio.stopStream();
    }
    catch (std::runtime_error &e) {
        std::cerr << "Error encountered: " << e.what() << std::endl;
        return 1;
    }

    if (audio.isStreamOpen()) audio.closeStream();
    return 0;
}
