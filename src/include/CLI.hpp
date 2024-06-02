#pragma once 
#include <opencv2/videoio.hpp>
#include <iostream>


enum OPTIONS
{
    VIDEO = 0,
    IMAGE = 1,
    IMAGE_LIST = 2
};

class CLI
{
    private:
        std::string _ipAddress;
        std::string _port;
        std::string _path;
    public:
        CLI();
        bool startCLI(cv::VideoCapture& camera);
        
        // Add for the next version *which is..... some day in the future 
        // OPTIONS startCLI(cv::VideoCapture& camera);
    private:
        void enterPath();
    public:
        // for debugging purposes
        bool autoStart(cv::VideoCapture& camera);
};