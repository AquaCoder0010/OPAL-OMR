#include "WrapFunction.hpp"

cv::Mat wrap(cv::Mat& inputImage, std::vector<cv::Point>& controlPoints)
{
    // wrap function
    cv::Mat outputImage;
    cv::Point2f sourcePoints[4];
	cv::Point2f distPoints[4] = { {0.f, 0.f} , {_wrappedWidth, 0.f} , {0.f, _wrappedHeight} , {_wrappedWidth, _wrappedHeight} };
	
	for(int i = 0; i < controlPoints.size(); i++)
		sourcePoints[i] = controlPoints[i];
	
	cv::Mat transMatrix = cv::getPerspectiveTransform(sourcePoints, distPoints);
	cv::warpPerspective(inputImage, outputImage, transMatrix, cv::Size(_wrappedWidth, _wrappedHeight));

    return outputImage;
}


cv::Mat wrap(cv::Mat& inputImage, std::vector<cv::Point>& controlPoints, int p_wrappedHeight, int p_wrappedWidth)
{
    // wrap function
    cv::Mat outputImage = inputImage.clone();
    cv::Point2f sourcePoints[4];
	cv::Point2f distPoints[4] = { {0.f, 0.f} , {p_wrappedWidth, 0.f} , {0.f, p_wrappedHeight} , {p_wrappedWidth, p_wrappedHeight} };
	
	for(int i = 0; i < controlPoints.size(); i++)
		sourcePoints[i] = controlPoints[i];
	
	cv::Mat transMatrix = cv::getPerspectiveTransform(sourcePoints, distPoints);
	cv::warpPerspective(inputImage, outputImage, transMatrix, cv::Size(p_wrappedWidth, p_wrappedHeight));

    return outputImage;
}

