#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

const float _wrappedHeight = 960;
const float _wrappedWidth = 720;

cv::Mat wrap(cv::Mat inputImage, std::vector<cv::Point> controlPoints);
