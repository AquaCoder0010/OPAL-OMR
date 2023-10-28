#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

class SimilarityChecker
{
    private:
        cv::Mat* _baseImage;
    public:
        SimilarityChecker();
        void getBaseImage(cv::Mat* baseImagePtr);
        float calculateSimilarityIndex(cv::Mat& image);   
};

