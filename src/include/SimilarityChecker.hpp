#pragma once
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

struct SimilarityChecker
{
    cv::Mat* baseImage;

    void getBaseImage(cv::Mat* baseImagePtr)
    {
        baseImage = baseImagePtr;
    }

    float calculateSimilarityIndex(cv::Mat& image)
    {
        cv::Mat baseHSV = *baseImage;
        cv::Mat imageHSV = image;
        cv::cvtColor((*baseImage), baseHSV, cv::COLOR_BGR2HSV);        
        cv::cvtColor(image, image, cv::COLOR_BGR2HSV);        

        // took source code from here : https://docs.opencv.org/4.5.5/d8/dc8/tutorial_histogram_comparison.html

        // hue bins 50, saturation bins 60 
        int histSize[] = { 50 , 60 }; 
        float hueRanges[] = { 0, 180 };
        float saturationRanges[] = { 0, 256 };

        const float* ranges[] = { hueRanges, saturationRanges };
        int channels[] = { 0, 1 };

        cv::calcHist(&baseHSV, 1, channels, cv::Mat(), baseHSV, 2, histSize, ranges);
        cv::normalize(baseHSV, baseHSV, 0, 1, cv::NORM_MINMAX);

        cv::calcHist(&imageHSV, 1, channels, cv::Mat(), imageHSV, 2, histSize, ranges);
        cv::normalize(imageHSV, imageHSV, 0, 1, cv::NORM_MINMAX);

        double similarityIndex = cv::compareHist(baseHSV, imageHSV, cv::HISTCMP_CORREL);
        std::cout << similarityIndex << '\n'; 
        return (float)similarityIndex;
    }   
};

