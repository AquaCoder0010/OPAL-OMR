#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

#include "WrapFunction.hpp"
#include "SimilarityChecker.hpp"
#include "PointOperations.hpp"





class Preprocessor
{
    private:
        SimilarityChecker _similarityChecker;
        cv::Ptr<cv::CLAHE> _clahe;
        cv::Mat _baseImage;
        cv::Mat _transformedImage;
    public:
        Preprocessor();
        void setBaseImage(cv::Mat& baseImage);
        cv::Mat getTransformedImage(cv::Mat& image);
        cv::Mat getTransformedImageHC();
        int findPaper();
    private:
        cv::Mat edgeDetect(cv::Mat& image);
        std::vector<cv::Point> orderPoints(std::vector<cv::Point>& points);
        void enhanceImage(cv::Mat& image);
        void drawPoints(cv::Mat& image, std::vector<cv::Point>& points, float similarityValue);
        void drawPoints(cv::Mat& image, std::vector<cv::Point>& points);
    private:
        float clamp(float value, float min, float max);
};