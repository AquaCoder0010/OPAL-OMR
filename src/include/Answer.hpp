#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <fstream>
#include <cmath>
#include <limits>


#include "WrapFunction.hpp"
#include "PointOperations.hpp"
#include "sign.hpp"
#include  "CalculateShade.hpp"

class Answer
{
    private:
        const int _transformedAnswerWidth = 565;
        const int _transformedAnswerHeight = 360;

        const int _xPos = 30;
        const int _yPos = 425;
        const int _height = 405;
        const int _width = 675;
        cv::Rect _answerBlockRect = cv::Rect(_xPos, _yPos, _width, _height);

        std::vector<cv::Rect> _answerBounds;
        std::vector<std::vector<cv::Point>> _answerContours;
    
        std::vector<cv::Vec4i> _horizontalLineList;
        std::vector<cv::Vec4i> _verticalLineList;
        std::vector<cv::Vec4i> _lineList;


        std::vector<cv::Vec4i> _gridList;
        std::vector<std::vector<cv::Rect>> _extractedAnswerList;
        std::vector<unsigned char> _answerList;
        float _evaluation = 0.f; 
    private:
        void cropAnswerBlock();
        void reorderAnswerBounds();
        float getShadedIndex(cv::Rect bound);
        cv::Mat getEdge(cv::Mat& image);
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
