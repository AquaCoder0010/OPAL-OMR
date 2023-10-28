#include "SimilarityChecker.hpp"

SimilarityChecker::SimilarityChecker()
{
    _baseImage = nullptr;
}

void SimilarityChecker::getBaseImage(cv::Mat* baseImagePtr)
{
    _baseImage = baseImagePtr;
}

float SimilarityChecker::calculateSimilarityIndex(cv::Mat& image)
{
    cv::Mat baseHist = cv::Mat();
    cv::Mat imageHist = cv::Mat();

    cv::Mat baseHSV = *_baseImage;
    cv::Mat imageHSV = image;
    cv::cvtColor((*_baseImage), baseHSV, cv::COLOR_BGR2HSV);        
    cv::cvtColor(image, image, cv::COLOR_BGR2HSV);        

    // took source code from here : https://docs.opencv.org/4.5.5/d8/dc8/tutorial_histogram_comparison.html

    int histSize[] = { 50 , 60 }; 
    float hueRanges[] = { 0, 180 };
    float saturationRanges[] = { 0, 256 };

    const float* ranges[] = { hueRanges, saturationRanges };
    int channels[] = { 0, 1 };


    cv::calcHist(&baseHSV, 1, channels, cv::Mat(),  baseHist, 2, histSize, ranges);
    cv::normalize(baseHSV, baseHSV, 0, 1, cv::NORM_MINMAX);

    cv::calcHist(&imageHSV, 1, channels, cv::Mat(), imageHist, 2, histSize, ranges);
    cv::normalize(imageHSV, imageHSV, 0, 1, cv::NORM_MINMAX);

    double similarityIndex = cv::compareHist(baseHist, imageHist, cv::HISTCMP_CORREL);
    std::cout << similarityIndex << '\n'; 
    return (float)similarityIndex;
}