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
        void drawEvaluation(cv::Mat& image, std::string bookletCode, std::string subjectCode, std::string registerNumber, float evaluation);
    public:
        OMR();
        OMR(cv::VideoCapture* camera);
        int run();      
};