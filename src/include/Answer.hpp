#pragma once
#include <opencv2/opencv.hpp>

#include "WrapFunction.hpp"
#include "PointOperations.hpp"

class Answer
{
    private:
        const int _xPos = 55;
        const int _yPos = 395;
        const int _height = 390;
        const int _width = 640;
        cv::Rect _answerBlockRect = cv::Rect(_xPos, _yPos, _width, _height);

        std::vector<cv::Rect> _answerBounds;
        std::vector<std::vector<cv::Point>> _answerContours;
        float _evaluation = 0.f; 
    private:
        void cropAnswerBlock();
        void sharpen();        
    public:
        cv::Mat _answerBlock = cv::Mat();
        cv::Mat _transformedAnswerBlock = cv::Mat();
    public:
        Answer();
        void setImage(cv::Mat& image);
        void preprocess();
        void extractAnswerBounds();
        float evaluateAnswer();        
};