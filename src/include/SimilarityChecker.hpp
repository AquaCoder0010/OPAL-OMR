#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

class SimilarityChecker
{
    private:
        cv::Mat _baseImage;
    public:
        SimilarityChecker();
        void setBaseImage(cv::Mat& baseImagePtr);
        float calSimilarity(cv::Mat& image);
};

