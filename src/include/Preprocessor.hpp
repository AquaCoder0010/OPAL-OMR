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
    public:
        Preprocessor();
        void setBaseImage(cv::Mat& baseImage);
        cv::Mat getTransformedImage(cv::Mat& image);
        int findPaper();
    private:
        cv::Mat edgeDetect(cv::Mat& image);
        std::vector<cv::Point> orderPoints(std::vector<cv::Point>& points);
        void drawPoints(cv::Mat& image, std::vector<cv::Point>& points, float similarityValue);
};