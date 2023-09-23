#pragma once
#include <opencv2/core.hpp>

struct SimilarityChecker
{
    cv::Mat* baseImage;

    void getBaseImage(cv::Mat* baseImagePtr)
    {
        baseImage = baseImagePtr;
    }

    float calculateSimilarityIndex(cv::Mat& image)
    {
        cv::Mat baseHSV = (*baseImage).clone();
        cv::Mat imageHSV = 
        cv::cvtColor(baseHSV, baseHSV, cv::COLOR_BGR2HSV);
        
    }   
};

