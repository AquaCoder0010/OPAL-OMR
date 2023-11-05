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
    cv::GaussianBlur(answerBlockClone, answerBlockClone, cv::Size(3, 3), 1, 1);
	cv::adaptiveThreshold(answerBlockClone, answerBlockClone, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 15, 3);

    
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

    std::cout << answerBlockPoly.size() << '\n'; 
    
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

    _transformedAnswerBlock = wrap(_answerBlock, answerBlockPoints, _transformedAnswerHeight, _transformedAnswerWidth);
}

void Answer::extractAnswerBounds()
{
    cv::Mat transformedAnswerBinary = _transformedAnswerBlock.clone(); 
    cv::Mat transformedGridRemoved = _transformedAnswerBlock.clone();

    cv::cvtColor(transformedAnswerBinary, transformedAnswerBinary, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(transformedAnswerBinary, transformedAnswerBinary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);

    cv::Mat horizontalLines = transformedAnswerBinary.clone();
    cv::Mat verticalLines = transformedAnswerBinary.clone();

    
	int horizontalLength = horizontalLines.cols / 30;
	cv::Mat morphKernelHorizontal = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontalLength, 1));
	cv::erode(horizontalLines, horizontalLines, morphKernelHorizontal);
	cv::dilate(horizontalLines, horizontalLines, morphKernelHorizontal);


	int verticalLength = verticalLines.cols / 30;
	cv::Mat morphKernelVertical = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, verticalLength));
	cv::erode(verticalLines, verticalLines, morphKernelVertical);
	cv::dilate(verticalLines, verticalLines, morphKernelVertical);

	cv::Mat dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(horizontalLines, horizontalLines, dilationKernel);
	cv::dilate(verticalLines, verticalLines, dilationKernel);

    cv::HoughLinesP(horizontalLines, _horizontalLineList, 1, CV_PI/180, 50, 10, 50);
	cv::HoughLinesP(verticalLines, _verticalLineList, 1, CV_PI/180, 50, 10, 50);


    _lineList.resize(_horizontalLineList.size());
	_lineList.insert(_lineList.begin(), _horizontalLineList.begin(), _horizontalLineList.end()); 
    _lineList.insert(_lineList.end(), _verticalLineList.begin(), _verticalLineList.end());

    
    for(auto& line : _lineList)
	{
		cv::Point point1 = cv::Point(line[0], line[1]);
		cv::Point point2 = cv::Point(line[2], line[3]);

		if(point1.x == point2.x)
		{
			point2.y -= 60;
			point1.y += 60;
		}

		if(point1.y == point2.y && (point1.x < point2.x))
		{
			point1.x -= 60;
			point2.x += 60;
		}

		if(point1.y == point2.y && (point1.x > point2.x))
		{
			point1.x += 60;
			point2.x -= 60;
		}
		//
		cv::line(transformedGridRemoved, point1, point2, cv::Scalar(255, 255, 255), 4);
	}
    cv::cvtColor(transformedGridRemoved, transformedGridRemoved, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(transformedGridRemoved, transformedGridRemoved, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);
    cv::Mat openingKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(transformedGridRemoved, transformedGridRemoved, cv::MORPH_CLOSE, openingKernel);

    
    cv::imshow("binary", transformedGridRemoved);
    cv::waitKey(0);
    cv::destroyAllWindows();

    std::vector<std::vector<cv::Point>> answerBoundContours;

    cv::findContours(transformedGridRemoved, answerBoundContours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_L1);
    
    for(auto& contour : answerBoundContours)
        _answerBounds.emplace_back(cv::boundingRect(contour));
    

    std::vector<cv::Rect>::iterator enditer = std::remove_if(_answerBounds.begin(), _answerBounds.end(), [](cv::Rect rect_1) 
	{
        int smallAreaLimit = 20;
        return (rect_1.width * rect_1.height) <= smallAreaLimit;    // remove small rectangles numbers
    });

	_answerBounds.erase(enditer, _answerBounds.end());
    for(auto& bound : _answerBounds)
        cv::rectangle(_transformedAnswerBlock, bound, cv::Scalar(255, 0, 0), 2);
    
    cv::imshow("transformedAnswerBlock", _transformedAnswerBlock);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
