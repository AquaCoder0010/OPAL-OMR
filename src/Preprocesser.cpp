#include "Preprocesser.hpp"

PreProcessImage::PreProcessImage(cv::Mat* baseImagePtr)
{
  if(baseImagePtr->empty() == true)
    std::cout << "the shit transfered here" << std::endl;
  
  _similarityChecker.getBaseImage(baseImagePtr);
}

void PreProcessImage::edgeDetect(cv::Mat &image)
{
  _inputImage = image;
  float meanValue = 0.f;
  cv::Scalar meanPixelValue = cv::Scalar();
  cv::Mat dilationKernel = cv::Mat();

  dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  _edgeImage = image.clone();
  meanPixelValue = cv::mean(_edgeImage);

  for (int i = 0; i < meanPixelValue.channels; i++)
    meanValue += meanPixelValue[i];

  meanValue /= meanPixelValue.channels;

  cv::cvtColor(_edgeImage, _edgeImage, cv::COLOR_BGR2GRAY);
  cv::Canny(_edgeImage, _edgeImage, meanValue, meanValue * 3, 3);
  cv::dilate(_edgeImage, _edgeImage, dilationKernel);
}

int PreProcessImage::findPaper()
{
  cv::findContours(_edgeImage, _contourList, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  std::vector<cv::Rect> boundRect(_contourList.size());
  std::sort(_contourList.begin(), _contourList.end(), [&](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
            {
      int area1 = cv::contourArea(contour1);
      int area2 = cv::contourArea(contour2);
      return area1 < area2; });

  int estimatedFindings = floor(_contourList.size() * 0.05);

  if (estimatedFindings == 0)
    estimatedFindings = 1;

  std::vector<std::vector<cv::Point>> _possiblePoints;
  float minSimilarityIndexValue = 0.7; 

  for (int i = _contourList.size() - 1; i > (_contourList.size() - 1 - estimatedFindings); i--)
  {
    std::vector<cv::Point> currentPoly;
    float arcLength = 0.f;

    arcLength = cv::arcLength(_contourList[i], true);

    // copies the approximated contour to the current poly cv::Point
    cv::approxPolyDP(_contourList[i], currentPoly, 0.02 * arcLength, true);
    if (currentPoly.size() == 4)
    {
      // put the points in the right order
      std::vector<cv::Point> orderedPoints;
      std::vector<PointOperations> pointOperationsList(currentPoly.size());

      for (int i = 0; i < currentPoly.size(); i++)
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
      
      _possiblePoints.emplace_back(orderedPoints);
    }
  }
  // minus one implies that it is an error
  if(_possiblePoints.size() == 0)
    return -1;
  
  std::vector<std::pair<int, float>> similarityList;
  for(int i = 0; i < _possiblePoints.size(); i++)
  {
    cv::Mat currentWrappedImage = wrap(_inputImage, _possiblePoints[i]);

    float currentSimiarityIndex = _similarityChecker.calculateSimilarityIndex(currentWrappedImage);

    similarityList.emplace_back();
    similarityList.back().first = i;
    similarityList.back().second = currentSimiarityIndex;
  }
  std::sort(similarityList.begin(), similarityList.end(), [&](std::pair<int, float> element1, std::pair<int, float> element2) {
    return element1.second < element2.second;
  });

  if(similarityList[similarityList.size() - 1].second > minSimilarityIndexValue)
  {
    int paperIndex = similarityList[similarityList.size() - 1].first;
    _imagePoints = _possiblePoints[paperIndex];
    _transformedImage = wrap(_inputImage, _imagePoints);
  }

  if(similarityList[similarityList.size() - 1].second < minSimilarityIndexValue)
    return -1;

  return 0;
}

void PreProcessImage::drawPoints(cv::Mat &image)
{
  for (int i = 0; i < _imagePoints.size(); i++)
  {
    cv::circle(image, _imagePoints[i], 3, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    cv::putText(image, std::to_string(i), cv::Point(_imagePoints[i].x + 10, _imagePoints[i].y + 10), cv::FONT_HERSHEY_PLAIN, 3, cv::Scalar(0, 255, 0), 2);
  }
  cv::line(image, _imagePoints[0], _imagePoints[1], cv::Scalar(0, 255, 0));
  cv::line(image, _imagePoints[0], _imagePoints[2], cv::Scalar(0, 255, 0));
  cv::line(image, _imagePoints[1], _imagePoints[3], cv::Scalar(0, 255, 0));
  cv::line(image, _imagePoints[2], _imagePoints[3], cv::Scalar(0, 255, 0));
}

std::vector<cv::Point> PreProcessImage::getPointList()
{
  return _imagePoints;
}
