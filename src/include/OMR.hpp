#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

#include "ImageFunctions.hpp"

class OMR
{
    private:
        cv::Mat currentImage;
        cv::Mat baseImage;
        cv::VideoCapture* cameraPointer;
    private:
        void getImage();
    public:
        OMR();
        OMR(cv::VideoCapture* camera);
        int run();      
};