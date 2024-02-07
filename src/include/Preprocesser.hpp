#pragma once


#include "WrapFunction.hpp"
#include "SimilarityChecker.hpp"
#include "PointOperations.hpp"

class PreProcessImage
{
    public:
        cv::Mat _edgeImage;
        cv::Mat _inputImage;
        cv::Mat _transformedImage;
        cv::Mat _baseImageCopy;   
    private:
        std::vector<std::vector<cv::Point>> _contourList;
        std::vector<std::vector<cv::Point>> _possiblePoints;
        std::vector<cv::Point> _imagePoints;

        SimilarityChecker _similarityChecker;
    public:
        PreProcessImage(cv::Mat* baseImagePtr);
        void setInputImage(cv::Mat& image);
        void edgeDetect();
        int findPaper();
        void drawPoints(cv::Mat& image);
        std::vector<cv::Point> getPointList();
    private:
        void edgeDetect(cv::Mat& image);
};