#include "SimilarityChecker.hpp"

SimilarityChecker::SimilarityChecker()
{
    _baseImage = nullptr;
}

void SimilarityChecker::getBaseImage(cv::Mat* baseImagePtr)
{
    _baseImage = baseImagePtr;
    cv::Mat baseHSV = *(_baseImage);    
    cv::cvtColor((*_baseImage), baseHSV, cv::COLOR_BGR2HSV);        

    cv::calcHist(&baseHSV, 1, channels, cv::Mat(),  _baseHist, 2, histSize, ranges);
    cv::normalize(baseHSV, baseHSV, 0, 1, cv::NORM_MINMAX);    
}

float SimilarityChecker::calculateSimilarityIndex(cv::Mat& image)
{
    cv::Mat imageHist = cv::Mat();

    cv::Mat imageClone = image.clone();
    cv::cvtColor(imageClone, imageClone, cv::COLOR_BGR2GRAY); 
    cv::adaptiveThreshold(imageClone, imageClone, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 3);
    cv::cvtColor(imageClone, imageClone, cv::COLOR_GRAY2BGR); 

    cv::Mat imageHSV = imageClone;
    cv::cvtColor(imageHSV, imageHSV, cv::COLOR_BGR2HSV);        

    cv::calcHist(&imageHSV, 1, channels, cv::Mat(), imageHist, 2, histSize, ranges);
    cv::normalize(imageHSV, imageHSV, 0, 1, cv::NORM_MINMAX);

    double similarityIndex = cv::compareHist(_baseHist, imageHist, cv::HISTCMP_CORREL);
    return (float)similarityIndex;
}