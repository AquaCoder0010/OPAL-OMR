#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

#include "ImageFunctions.hpp"

class OMR
{
    private:
        cv::Mat _currentImage;
        cv::Mat _baseImage;
        cv::VideoCapture* _cameraPointer;
    private:
        void getImage();
    public:
        OMR();
        OMR(cv::VideoCapture* camera);
        int run();      
};