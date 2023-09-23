#pragma once 
#include <opencv2/videoio.hpp>
#include <iostream>

class CLI
{
    private:
        std::string ipAddress;
        std::string port;
    public:
        CLI();
        bool startCLI(cv::VideoCapture& camera);
    public:
        // for debugging purposes
        bool autoStart(cv::VideoCapture& camera);
};