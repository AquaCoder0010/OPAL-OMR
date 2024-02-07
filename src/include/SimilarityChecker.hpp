#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

class SimilarityChecker
{
    private:
        cv::Mat* _baseImage;
        cv::Mat _baseHist;
        
        int histSize[2] = { 50 , 60 }; 
        float hueRanges[2] = { 0, 180 };
        float saturationRanges[2] = { 0, 256 };

        const float* ranges[2] = { hueRanges, saturationRanges };
        int channels[2] = { 0, 1 };
    
    public:
        SimilarityChecker();
        void getBaseImage(cv::Mat* baseImagePtr);
        float calculateSimilarityIndex(cv::Mat& image);   
};

