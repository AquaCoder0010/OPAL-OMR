#pragma once
#include <opencv2/opencv.hpp>

#include "WrapFunction.hpp"

class Answer
{
    private:
        const int _xPos = 55;
        const int _yPos = 395;
        const int _height = 390;
        const int _width = 640;
        cv::Rect _answerBlockRect = cv::Rect(x_pos, y_pos, width, height);

        cv::Mat _imageCopy = cv::Mat();
        cv::Mat _answerBlock = cv::Mat();

        std::vector<cv::Rect> _answerBounds;
        float evaluation = 0.f; 
    private:
        void cropAnswerBlock();
        void sharpen();
        void preprocess();
        
    public:
        Answer();
        void extractAnswerBounds();
        float evaluateAnswer();
        
        std::vector<cv::Rect> getAnswerBound(){ return _answerBound }; 
};