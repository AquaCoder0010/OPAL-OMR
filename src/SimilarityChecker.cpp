#include "SimilarityChecker.hpp"

SimilarityChecker::SimilarityChecker()
:_baseImage()
{

}

void SimilarityChecker::setBaseImage(cv::Mat& baseImage)
{
    _baseImage = baseImage;
}

float SimilarityChecker::calSimilarity(cv::Mat& image)
{
    float similarityIndex = 0.f;
    float baseValue = 0.f;

    cv::Mat baseImageBinary = _baseImage.clone();    
    cv::Mat imageBinary = image.clone();

    // making the image binary
    cv::cvtColor(imageBinary, imageBinary, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(imageBinary, imageBinary);
    cv::GaussianBlur(imageBinary, imageBinary, cv::Size(11, 11), 0, 0);
    cv::adaptiveThreshold(imageBinary, imageBinary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 3); 
    
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(imageBinary, imageBinary, cv::MORPH_OPEN, kernel);
    
    // computing the baseImage binary
    cv::cvtColor(baseImageBinary, baseImageBinary, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(baseImageBinary, baseImageBinary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 3); 
    baseValue = (float)cv::countNonZero(baseImageBinary) / (baseImageBinary.cols * baseImageBinary.rows); 
    
    cv::Mat subtractedImage = baseImageBinary - imageBinary;
    similarityIndex = cv::countNonZero(subtractedImage) / (float)(subtractedImage.cols * subtractedImage.rows);
    similarityIndex /= baseValue;

    return (1 - similarityIndex);
}
