#pragma once
#include <bitset>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class SimilarityChecker
{
    private:
        std::string _baseImageHash;
    public:
        SimilarityChecker();
        void setBaseImage(cv::Mat& baseImage);
        float calSimilarity(cv::Mat& image);
    private:
        std::string aHash(cv::Mat& image);
        float compare(std::string hash1, std::string hash2);
};