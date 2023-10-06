#pragma once
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

#include "WrapFunction.hpp"
#include "SimilarityChecker.hpp"
#include "PointOperations.hpp"

struct PreProcessImage
{
    cv::Mat edgeImage;
    cv::Mat inputImage;
    std::vector<std::vector<cv::Point>> contourList;
    std::vector<std::vector<cv::Point>> possiblePoints;
    std::vector<cv::Point> imagePoints;

    WrapFunction wrapFunction;
    SimilarityChecker similarityChecker;

public:

    PreProcessImage(cv::Mat* baseImagePtr)
    {
        similarityChecker.getBaseImage(baseImagePtr);
    }

    void edgeDetect(cv::Mat &image)
    {
        inputImage = image;
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

        int estimatedFindings = floor(contourList.size() * 0.05);
        
        if(estimatedFindings == 0)
            estimatedFindings = 1;

        std::vector<std::vector<cv::Point>> possiblePaperPoints;  
        
        for(int i = contourList.size() - 1; i > contourList.size() - 1 - estimatedFindings; i--)
        {
		    std::vector<cv::Point> currentPoly;
            float arcLength = 0.f;

            arcLength = cv::arcLength(contourList[i], true);
			cv::approxPolyDP(contourList[i], currentPoly, 0.02 * arcLength, true);
            if(currentPoly.size() == 4)
            {
                std::vector<cv::Point> orderedPoints;
                std::vector<PointOperations> pointOperationsList(currentPoly.size());

                for(int i = 0; i < currentPoly.size(); i++)
                {
                    pointOperationsList[i].index = i;
                    pointOperationsList[i].sum_value = currentPoly[i].x + currentPoly[i].y;
                    pointOperationsList[i].sub_value = currentPoly[i].x - currentPoly[i].y;
                }
                orderedPoints.reserve(4);
                orderedPoints.emplace_back(currentPoly[getMinSum(pointOperationsList)]); 
                orderedPoints.emplace_back(currentPoly[getMaxSub(pointOperationsList)]); 
                orderedPoints.emplace_back(currentPoly[getMinSub(pointOperationsList)]); 
                orderedPoints.emplace_back(currentPoly[getMaxSum(pointOperationsList)]); 


                possiblePaperPoints.emplace_back(currentPoly);
            }
        }

        if(possiblePaperPoints.size() == 1)
            imagePoints = possiblePaperPoints[0];
        else
        {
            std::vector<std::pair<int, float>> similarityList;

            for(int i = 0; i < possiblePaperPoints.size(); i++)
            {
                float currentSimilarityIndex = 0.f;
                
                cv::Mat currentWrappedImage = wrapFunction.wrap(inputImage, possiblePaperPoints[i]);
                currentSimilarityIndex = similarityChecker.calculateSimilarityIndex(currentWrappedImage);

                similarityList.emplace_back();
                similarityList.back().first = i;
                similarityList.back().second = currentSimilarityIndex;
            }
            std::sort(similarityList.begin(), similarityList.end(), [&](std::pair<int, float> element1, std::pair<int, float> element2){
                return element1.second < element2.second;
            });
            
            int paperImageIndex = similarityList[similarityList.size()-1].first;
            imagePoints = possiblePaperPoints[paperImageIndex];
        }

    }

    void drawPoints(cv::Mat &image)
    {
        for (int i = 0; i < imagePoints.size(); i++)
        {
            cv::circle(image, imagePoints[i], 3, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
            cv::putText(image, std::to_string(i), cv::Point(imagePoints[i].x + 10, imagePoints[i].y + 10), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
        }
        cv::line(image, imagePoints[0], imagePoints[1], cv::Scalar(0, 255, 0));
        cv::line(image, imagePoints[0], imagePoints[2], cv::Scalar(0, 255, 0));
        cv::line(image, imagePoints[1], imagePoints[3], cv::Scalar(0, 255, 0));
        cv::line(image, imagePoints[2], imagePoints[3], cv::Scalar(0, 255, 0));
    }


    std::vector<cv::Point> getPointList()
    {
        return imagePoints;
    }
};