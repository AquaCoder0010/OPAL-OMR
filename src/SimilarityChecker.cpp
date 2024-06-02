#include "SimilarityChecker.hpp"

SimilarityChecker::SimilarityChecker()
:_baseImageHash()
{

}

void SimilarityChecker::setBaseImage(cv::Mat& baseImage)
{
    _baseImageHash = aHash(baseImage);
    std::cout << _baseImageHash << '\n';
}


float SimilarityChecker::calSimilarity(cv::Mat& image)
{
    return 1 - compare(aHash(image), _baseImageHash);
}


std::string SimilarityChecker::aHash(cv::Mat& image)
{
    cv::Mat imageClone = image.clone();
    if(imageClone.channels() == 3)
        cv::cvtColor(imageClone, imageClone, cv::COLOR_BGR2GRAY);
    
    cv::resize(imageClone, imageClone, cv::Size(8, 8));
    cv::normalize(imageClone, imageClone, 0, 1, cv::NORM_MINMAX, CV_32F);
    int averageValue = cv::mean(imageClone)[0];

    std::bitset<64> hashBitSet;
    for (int y =  0; y < imageClone.rows; ++y) {
        for (int x =  0; x < imageClone.cols; ++x) {
            hashBitSet[(y *  8) + x] = (imageClone.at<float>(y, x) > averageValue);
        }
    }
    return hashBitSet.to_string(); 
}

float SimilarityChecker::compare(std::string hash1, std::string hash2)
{
    if(hash1.length() != hash2.length())
        return -1.f;
    int dist =  0;
    for (size_t i =  0; i < hash1.length(); ++i) {
        dist += (int)(hash1[i] != hash2[i]);
    }
    return (float)(dist) / hash1.length();
}
