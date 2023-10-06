#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

struct WrapFunction
{
    const float wrappedHeight = 720;
    const float wrappedWidth = 920;

    cv::Mat wrap(cv::Mat inputImage, std::vector<cv::Point> controlPoints)
    {
        cv::Mat outputImage;
        cv::Point2f sourcePoints[4];
	    cv::Point2f distPoints[4] = { {0.f, 0.f} , {wrappedWidth, 0.f} , {0.f, wrappedHeight} , {wrappedWidth, wrappedHeight} };
	
	    for(int i = 0; i < controlPoints.size(); i++)
		    sourcePoints[i] = controlPoints[i];
	
	    cv::Mat transMatrix = cv::getPerspectiveTransform(sourcePoints, distPoints);
	    cv::warpPerspective(inputImage, outputImage, transMatrix, cv::Size(wrappedWidth, wrappedHeight));

        return outputImage;
    }

};