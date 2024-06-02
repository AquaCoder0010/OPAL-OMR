#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <vector>

const int _wrappedHeight = 960;
const int _wrappedWidth = 720;

cv::Mat wrap(cv::Mat& inputImage, std::vector<cv::Point>& controlPoints);
cv::Mat wrap(cv::Mat& inputImage, std::vector<cv::Point>& controlPoints, int p_wrappedHeight, int p_wrappedWidth);
