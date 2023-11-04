#include "Answer.hpp"

Answer::Answer()
:_answerBlock(), _transformedAnswerBlock()
{

}

void Answer::setImage(cv::Mat& image)
{
    _answerBlock = image;
}

void Answer::cropAnswerBlock()
{
    cv::Mat answerBlockClone = _answerBlock.clone(); 
    _answerBlock = answerBlockClone(_answerBlockRect).clone();
}

void Answer::sharpen()
{
    cv::Mat shaprenKernel( 3 ,3 , CV_32F , cv::Scalar(0));

	shaprenKernel.at<float>(1,1)= 5.2;
	shaprenKernel.at<float>(0,1)= -1.0;
	shaprenKernel.at<float>(2,1)= -1.0;
	shaprenKernel.at<float>(1,0)= -1.0;
	shaprenKernel.at<float>(1,2)= -1.0;

	cv::filter2D(_answerBlock, _answerBlock, _answerBlock.depth(), shaprenKernel);
}

void Answer::preprocess()
{
    cropAnswerBlock(); 
    sharpen();

    cv::Mat answerBlockClone = _answerBlock; 
	

    cv::cvtColor(answerBlockClone, answerBlockClone, cv::COLOR_BGR2GRAY);
	cv::adaptiveThreshold(answerBlockClone, answerBlockClone, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 3);

    cv::imshow("answerBlock-clone", answerBlockClone);
    cv::waitKey(0);
    
    cv::findContours(answerBlockClone, _answerContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::sort(_answerContours.begin(), _answerContours.end(), [&](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
    {
        int area1 = cv::contourArea(contour1);
        int area2 = cv::contourArea(contour2);
        return area1 < area2; 
    });

    std::vector<cv::Point> answerBlockPoly;
    std::vector<cv::Point> answerBlockPoints;
    float arcLength = 0.f;

    arcLength = cv::arcLength(_answerContours[_answerContours.size() - 1], true);

    cv::approxPolyDP(_answerContours[_answerContours.size() - 1], answerBlockPoly, 0.02 * arcLength, true);
    if(answerBlockPoly.size() == 4)
    {
        std::vector<cv::Point> orderedPoints;
        std::vector<PointOperations> pointOperationsList(answerBlockPoly.size());

        for (int i = 0; i < answerBlockPoly.size(); i++)
        {
            pointOperationsList[i].index = i;
            pointOperationsList[i].sum_value = answerBlockPoly[i].x + answerBlockPoly[i].y;
            pointOperationsList[i].sub_value = answerBlockPoly[i].x - answerBlockPoly[i].y;
        }
        orderedPoints.reserve(4);
        orderedPoints.emplace_back(answerBlockPoly[getMinSum(pointOperationsList)]);
        orderedPoints.emplace_back(answerBlockPoly[getMaxSub(pointOperationsList)]);
        orderedPoints.emplace_back(answerBlockPoly[getMinSub(pointOperationsList)]);
        orderedPoints.emplace_back(answerBlockPoly[getMaxSum(pointOperationsList)]);
        
        answerBlockPoints = orderedPoints;
    }

    _transformedAnswerBlock = wrap(_answerBlock, answerBlockPoints);
}