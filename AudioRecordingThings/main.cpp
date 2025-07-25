
#include "glad.h"
#include "RtAudio.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "kissfft/kiss_fft.h"
#include "implot/implot.h"

//export CPATH="/opt/homebrew/include";
//export LIBRARY_PATH="/opt/homebrew/lib";

//#include <GLFW/glfw3.h>
#include "glfw3.h"
//#include <OpenGL/gl3.h>
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
//#include "../../../../addons/ofxAubio/src/ofxAubio.h"
//#include "/Users/calvi/Documents/openFrameworks/of_v0.12.1_osx_release/addons/ofxAubio/src/ofxAubio.h"

int BUFFER_SIZE = 1024;
int numCallbackCalls = 0;
float firstBuffer[1024] = {0.0f};
float magBuffer[1024] = {0.0f};

void hannWindow (std::vector<float> &buffer);
kiss_fft_cfg FFT(std::vector<float> &buffer);

// callback function for input and output
int recordCallback(void *outputBuffer, void *inputBuffer,
                   unsigned int nFrames, double streamTime,
                   RtAudioStreamStatus status, void *userData) {
    // cast the input buffer to a double *
    float* buffer = (float *)inputBuffer;

    // clears console
    std::cout << "\033[2J\033[H";
    std::cout << "Value of inputBuffer[0]: " << buffer[0] << std::endl;

    float sumSquares = 0.0f;
    for (unsigned int i = 0; i < nFrames; i++) {
        float sample = buffer[i];
        sumSquares += sample * sample;
    }

    // ChatGPT generated rms and decibel calculation
    ////////
    float rms = std::sqrt(sumSquares / (nFrames));
    float volumeDb = 20 * std::log10(rms + 1e-6f);
    ////////

    float linearVolume = std::pow(10.0f, volumeDb / 20.0f);
    float stretchedSqrt = std::sqrt(linearVolume);
    float stretchedPow = std::pow(linearVolume, 0.3f);

    std::cout << "Decibels: " << volumeDb << std::endl;
    std::cout << "RMS: " << rms << std::endl;
    std::cout << "linear volume: " << linearVolume << std::endl;
    std::cout << "square rooted linear volume: " << linearVolume << std::endl;
    std::cout << "powered linear volume: " << stretchedPow << std::endl;

    std::cout << std::endl << std::endl;

    std::cout << "Linear volume: " << std::endl;
    for (double i = 0; i < linearVolume; i+=.01) {
        std::cout << "█";
    }
    std::cout << std::endl;

    std::cout << "Square rooted linear volume: " << std::endl;
    // Display volume
    for (double i = 0; i < stretchedSqrt; i+=.01) {
        std::cout << "█";
    }
    std::cout << std::endl;

    std::cout << "Powered linear volume: " << std::endl;
    // Display volume
    for (double i = 0; i < stretchedPow; i+=.01) {
        std::cout << "█";
    }

    std::vector<float> bufferVector(nFrames);
    // store audio buffer in vector
    for (int i = 0; i < nFrames; i++) {
        bufferVector[i] = buffer[i];
    }

    hannWindow(bufferVector);
    FFT(bufferVector);

    if (numCallbackCalls == 20) {
        // store the first buffer
        for (int i = 0; i < nFrames; i++) {
            firstBuffer[i] = buffer[i];
        }
    }

    std::cout << std::endl << std::endl;

    numCallbackCalls++;

    return 0;
}

// applies Hann window on the buffer
void hannWindow (std::vector<float> &buffer) {
    int sizeOfInput = buffer.size();

    // code refactored from Joonas Pulakka at https://stackoverflow.com/questions/3555318/implement-hann-window
    for (int i = 0; i < sizeOfInput; i++) {
        buffer[i] *= 0.5 * (1 - cos(2 * M_PI * i / (sizeOfInput - 1)));
    }
}

// does a fast fourier transform on the buffer using kissfft
kiss_fft_cfg FFT(std::vector<float> &buffer) {
    int sizeOfInput = buffer.size();

    kiss_fft_cpx input[sizeOfInput], output[sizeOfInput];
    kiss_fft_cfg cfg = kiss_fft_alloc(sizeOfInput, 0, nullptr, nullptr);

    // fill input with buffer info
    for (int i = 0; i < sizeOfInput; i++) {
        input[i].r = buffer[i];
        input[i].i = 0;
    }

    // perform FFT
    kiss_fft(cfg, input, output);

    // // print the results
    // printf("FFT Results:\n");
    // for (int k = 0; k < sizeOfInput; ++k) {
    //     printf("Bin %d: Real = %f, Imaginary = %f\n", k, output[k].r, output[k].i);
    // }

    for (int i = 0; i < BUFFER_SIZE; i++) {
        float real = output[i].r;
        float imag = output[i].i;
        magBuffer[i] = sqrtf(real * real + imag * imag);
    }

    return cfg;
}

int main() {
    RtAudio audio;

    if (audio.getDeviceCount() < 1) {
        std::cout << "No audio devices found" << std::endl;
        return 1;
    }


    ///////////////////
    ///////////////////

    RtAudio::StreamParameters inputParams;
    inputParams.deviceId = audio.getDefaultInputDevice();

    // get the device IDs and names of every available device
    std::vector<unsigned int> deviceIDs = audio.getDeviceIds();
    for (std::vector<unsigned int>::iterator it = deviceIDs.begin(); it != deviceIDs.end(); ++it) {
        std::cout << "Device ID: " << *it << " " << audio.getDeviceInfo(*it).name;
        std::cout << " Inpt cnls: " << audio.getDeviceInfo(*it).inputChannels << std::endl;

    }
    
    //user input to select device
    int choice = 0;
    while (choice <= 128 || choice > 128 + audio.getDeviceCount()) {
        std::cout << "Enter device ID for the device you want: ";
        std::cin >> choice;
    }
    std::cout << "Sample Rate: " << audio.getDeviceInfo(choice).currentSampleRate << std::endl;
    std::cout << "Channels: " << audio.getDeviceInfo(choice).inputChannels << std::endl;

    // using mono input
    inputParams.nChannels = 1;
    inputParams.firstChannel = 0;
    inputParams.deviceId = choice;

    // set sample rate to the sample rate of the device being used
    unsigned int sampleRate = audio.getDeviceInfo(inputParams.deviceId).currentSampleRate;
    unsigned int bufferFrames = BUFFER_SIZE;

    try {
        audio.openStream(nullptr, &inputParams, RTAUDIO_FLOAT32,
                         sampleRate, &bufferFrames, &recordCallback);
        audio.startStream();

        // list the default input device
        unsigned int defaultInputDevice = audio.getDefaultInputDevice();
        RtAudio::DeviceInfo info = audio.getDeviceInfo(defaultInputDevice);
        std::cout << "Default input device: " << info.name << " ID " << defaultInputDevice;
        std::cout << "Inpt cnls: " << audio.getDeviceInfo(inputParams.deviceId).inputChannels << std::endl;

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();

        audio.stopStream();
    }
    catch (std::runtime_error &e) {
        std::cerr << "Error encountered: " << e.what() << std::endl;
        return 1;
    }

    if (audio.isStreamOpen()) audio.closeStream();

        // Initialize GLFW
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "ImGui Example", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Initialize GLAD (add this!)
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSwapInterval(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
    ImGui::StyleColorsDark();

    ImPlot::CreateContext();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Your ImGui code here
        ImGui::Begin("My Window");
        ImGui::Text("Hello, world %d", 123);
        ImGui::End();

        

        static float x[1024], y[100];
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            x[i] = i;
            y[i] = sin(x[i]);
        }

        if (ImPlot::BeginPlot("Sound Wave cross section")) {
            ImPlot::PlotLine("", x, firstBuffer, 1024);
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("FFT Result")) {
            ImPlot::PlotLine("", x, magBuffer, 1024);
            ImPlot::EndPlot();
        }


        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}