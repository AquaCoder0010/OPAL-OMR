#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>
#include <cmath>

#include "WrapFunction.hpp"
#include "PointOperations.hpp"

class Answer
{
    private:
        const int _transformedAnswerWidth = 565;
        const int _transformedAnswerHeight = 360;

        
        const int _xPos = 55;
        const int _yPos = 395;
        const int _height = 390;
        const int _width = 640;
        cv::Rect _answerBlockRect = cv::Rect(_xPos, _yPos, _width, _height);

        std::vector<cv::Rect> _answerBounds;
        std::vector<std::vector<cv::Point>> _answerContours;
    
        std::vector<cv::Vec4i> _horizontalLineList;
        std::vector<cv::Vec4i> _verticalLineList;
        std::vector<cv::Vec4i> _lineList;

        std::vector<std::vector<cv::Rect>> _extractedAnswerList;
        std::vector<unsigned char> _answerList;
        float _evaluation = 0.f; 
    private:
        void cropAnswerBlock();
        void sharpen();
        void reorderAnswerBounds();
        float getShadedIndex(cv::Rect bound);
    public:
        cv::Mat _answerBlock = cv::Mat();
        cv::Mat _transformedAnswerBlock = cv::Mat();
    public:
        Answer();
        void setImage(cv::Mat& image);
        bool loadAnswer(const std::string& filePath);
        void preprocess();
        void extractAnswerBounds();
        float evaluateAnswer();
};