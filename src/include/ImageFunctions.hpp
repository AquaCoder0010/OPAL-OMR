#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>


struct PreProcessImage
{
    cv::Mat edgeImage;
    std::vector<std::vector<cv::Point>> contourList;
    std::vector<cv::Point> pointList;

    void edgeDetect(cv::Mat &image)
    {
        float meanValue = 0.f;
        cv::Scalar meanPixelValue = cv::Scalar();
        cv::Mat dilationKernel = cv::Mat();

        dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        edgeImage = image.clone();
        meanPixelValue = cv::mean(edgeImage);

        for (int i = 0; i < meanPixelValue.channels; i++)
            meanValue += meanPixelValue[i];

        meanValue /= meanPixelValue.channels;

        cv::cvtColor(edgeImage, edgeImage, cv::COLOR_BGR2GRAY);
        cv::Canny(edgeImage, edgeImage, meanValue, meanValue * 3, 3);
        cv::dilate(edgeImage, edgeImage, dilationKernel);
    }

    void approxPoly()
    {
        cv::findContours(edgeImage, contourList, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Rect> boundRect(contourList.size());
        std::sort(contourList.begin(), contourList.end(), [&](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
                  {
            int area1 = cv::contourArea(contour1);
            int area2 = cv::contourArea(contour2);
            return area1 < area2; });

        int estimatedFindings = floor(contourList.size() * 0.2);
        
    }

    void drawPoints(cv::Mat &image)
    {
        cv::drawContours(image, contourList, contourList.size() - 1, cv::Scalar(0, 0, 255));
        // for (int i = 0; i < pointList.size(); i++)
        // {
        //     cv::circle(image, pointList[i], 3, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
        //     cv::putText(image, std::to_string(i), cv::Point(pointList[i].x + 10, pointList[i].y + 10), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
        // }
        // cv::line(image, pointList[0], pointList[1], cv::Scalar(0, 255, 0));
        // cv::line(image, pointList[0], pointList[2], cv::Scalar(0, 255, 0));
        // cv::line(image, pointList[1], pointList[3], cv::Scalar(0, 255, 0));
        // cv::line(image, pointList[2], pointList[3], cv::Scalar(0, 255, 0));
    }

    void reorder()
    {
    }

    std::vector<cv::Point> getPointList()
    {
        return pointList;
    }

    void wrapToPointList()
    {
    }
};