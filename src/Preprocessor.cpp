#include "Preprocessor.hpp"


Preprocessor::Preprocessor()
:_similarityChecker(), _transformedImage()
{
    _clahe = cv::createCLAHE();

}

void Preprocessor::setBaseImage(cv::Mat& baseImage)
{
    _similarityChecker.setBaseImage(baseImage);
    _baseImage = baseImage.clone();
}

cv::Mat Preprocessor::edgeDetect(cv::Mat& image)
{
    if(image.empty() == true)
        return cv::Mat();
    float meanValue = 0.f;
    cv::Scalar meanPixelValue = cv::Scalar();
    cv::Mat edge = image.clone();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    
    meanPixelValue = cv::mean(image);
    
    for (int i = 0; i < meanPixelValue.channels; i++)
        meanValue += meanPixelValue[i];

    meanValue /= meanPixelValue.channels;
    meanValue *= 1.5;

    
    cv::cvtColor(image, edge, cv::COLOR_BGR2GRAY);
    cv::medianBlur(edge, edge, 5);
    cv::dilate(edge, edge, kernel);
    cv::Canny(edge, edge, meanValue, meanValue * 3, 3);
    return edge;
}


std::vector<cv::Point> Preprocessor::orderPoints(std::vector<cv::Point>& points)
{
    std::vector<cv::Point> orderedPoints;     
    std::vector<PointOperations> pointOperationsList(points.size());

    for (int i = 0; i < points.size(); i++)
    {
        pointOperationsList[i].index = i;
        pointOperationsList[i].sum_value = points[i].x + points[i].y;
        pointOperationsList[i].sub_value = points[i].x - points[i].y;
    }
    orderedPoints.reserve(4);
    orderedPoints.emplace_back(points[getMinSum(pointOperationsList)]);
    orderedPoints.emplace_back(points[getMaxSub(pointOperationsList)]);
    orderedPoints.emplace_back(points[getMinSub(pointOperationsList)]);
    orderedPoints.emplace_back(points[getMaxSum(pointOperationsList)]);

    return orderedPoints;  
}

void Preprocessor::drawPoints(cv::Mat& image, std::vector<cv::Point>& points, float similarityValue)
{
    cv::putText(image, std::to_string(similarityValue * 100), cv::Point((points[0].x + points[1].x)*0.5, points[0].y), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
    for (int i = 0; i < points.size(); i++)
    {
        cv::circle(image, points[i], 3, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
        cv::putText(image, std::to_string(i), cv::Point(points[i].x + 10, points[i].y + 10), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
    }
    if(points.size() == 4)
    {
        cv::line(image, points[0], points[1], cv::Scalar(0, 255, 0));
        cv::line(image, points[0], points[2], cv::Scalar(0, 255, 0));
        cv::line(image, points[1], points[3], cv::Scalar(0, 255, 0));
        cv::line(image, points[2], points[3], cv::Scalar(0, 255, 0));
    }
}


void Preprocessor::drawPoints(cv::Mat& image, std::vector<cv::Point>& points)
{
    for (int i = 0; i < points.size(); i++)
    {
        cv::circle(image, points[i], 3, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
        cv::putText(image, std::to_string(i), cv::Point(points[i].x + 10, points[i].y + 10), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
    }
    if(points.size() == 4)
    {
        cv::line(image, points[0], points[1], cv::Scalar(0, 255, 0));
        cv::line(image, points[0], points[2], cv::Scalar(0, 255, 0));
        cv::line(image, points[1], points[3], cv::Scalar(0, 255, 0));
        cv::line(image, points[2], points[3], cv::Scalar(0, 255, 0));
    }
}

cv::Mat Preprocessor::getTransformedImageHC()
{
    if(_transformedImage.empty() == true)
        return cv::Mat();
    
    cv::Mat transformedHC = _transformedImage.clone(); 
    // enhanceImage(transformedHC);
    return transformedHC;
}


cv::Mat Preprocessor::getTransformedImage(cv::Mat& image)
{
    float threshold = 0.5f;
    cv::Mat transformedImage = cv::Mat();
    cv::Mat darkImage = cv::Mat::zeros(cv::Size(image.cols, image.rows), CV_8UC3);

    std::vector<std::vector<cv::Point>> currentContours;
    std::vector<std::vector<cv::Point>> convexHull;


    cv::Mat edgeImage = edgeDetect(image);
    cv::Mat resizedEdgeImage = cv::Mat(); 

    // cv::resize(edgeImage, resizedEdgeImage, cv::Size(640, 640));
    // cv::imshow("resizedEdgeImage", resizedEdgeImage);

    cv::findContours(edgeImage, currentContours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_L1);
    std::sort(currentContours.begin(), currentContours.end(), [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2){
        cv::Rect rect1 = cv::boundingRect(contour1);
        cv::Rect rect2 = cv::boundingRect(contour2); 

        return rect1.width * rect1.height < rect2.width * rect2.height;
    });
    std::vector<std::vector<cv::Point>>::iterator enditer = std::remove_if(currentContours.begin(), currentContours.end(), [&](std::vector<cv::Point> contour) 
	{
        int smallAreaLimit = 2000;
        cv::Rect boundingRect = cv::boundingRect(contour); 
        return (boundingRect.width * boundingRect.height) <= smallAreaLimit;    // remove small rectangles numbers
    });
	currentContours.erase(enditer, currentContours.end());
    if(currentContours.size() == 0)
        return darkImage;
    
    std::vector<cv::Point> points;
    float arcLength = cv::arcLength(currentContours[currentContours.size() - 1], true);
    cv::approxPolyDP(currentContours[currentContours.size() - 1], points, 0.02 * arcLength, true);   
    
    if(points.size() == 4 && cv::isContourConvex(points) == true)
    {
        std::vector<cv::Point> orderedPoints = orderPoints(points);
        cv::Mat possibleWrap = wrap(image, orderedPoints);

        float similarityValue = _similarityChecker.calSimilarity(possibleWrap);
        drawPoints(image, orderedPoints);

        // _transformedImage = possibleWrap.clone();
        // enhanceImage(possibleWrap);
        return possibleWrap;
    }
    return darkImage;
}

void Preprocessor::enhanceImage(cv::Mat& image)
{
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat horizontalKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(20, 1));
    cv::Mat verticalKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 20));

    cv::Mat backgroundImage = image.clone();
    cv::Mat backgroundRemoved = cv::Mat();

    cv::dilate(backgroundImage, backgroundImage, kernel);
    cv::medianBlur(backgroundImage, backgroundImage, 3);

    backgroundImage *= 0.90;
    image = image - backgroundImage;

    // cv::GaussianBlur(image, image, cv::Size(3, 3), 0, 0);
    cv::medianBlur(backgroundImage, backgroundImage, 3);
    cv::normalize(image, image, 0, 255, cv::NORM_MINMAX);
    
    float contrastValue = 12.5;
    float correctionFactor = 259*(255 + contrastValue)/(259 - contrastValue);

    int channels = image.channels();
    for (int y = 0; y < image.rows; ++y) {
        for (int x = 0; x < image.cols; ++x) {
            cv::Vec3b &pixel = image.at<cv::Vec3b>(y, x);
            pixel[0] = clamp(correctionFactor*(128 - pixel[0]) + 128.f, 0, 255); // blue channel
            pixel[1] = clamp(correctionFactor*(128 - pixel[1]) + 128.f, 0, 255); // green channel
            pixel[2] = clamp(correctionFactor*(128 - pixel[2]) + 128.f, 0, 255); // red channel
        }
    }
    cv::bitwise_not(image, image);
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

} 
float Preprocessor::clamp(float value, float max, float min)
{
    if(value >= max)
        return max;
    if(value <= min)
        return min;
    return value;
} 

