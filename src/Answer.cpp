#include "Answer.hpp"

Answer::Answer()
:_answerBlock(), _transformedAnswerBlock()
{

}

bool Answer::loadAnswer(const std::string& filePath)
{
    std::vector<std::string> stringList;
    std::ifstream file(filePath);

    std::string currentString;

    while(std::getline(file, currentString))
        stringList.emplace_back(currentString);
    
    file.close();
    for(std::string string : stringList)
    {
        if(string.length() > 1)
            return false;
        _answerList.emplace_back(static_cast<unsigned char>(string[0]));
    }
    return true;
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

    reorderAnswerBounds(); 
}

void Answer::reorderAnswerBounds()
{
    std::vector<std::vector<cv::Rect>> rowList;
	std::vector<cv::Rect> currentRow;
	std::vector<cv::Rect> sortedAnswerList = _answerBounds;
	
	int tolerance = 8;
	std::sort(sortedAnswerList.begin(), sortedAnswerList.end(), [](const cv::Rect rect1, const cv::Rect rect2)
	{
		return rect1.y < rect2.y;
	});


	currentRow.emplace_back(sortedAnswerList[0]);
	for(int i = 1; i < sortedAnswerList.size(); i++)
	{
		if( abs(sortedAnswerList[i].y - sortedAnswerList[i - 1].y) <= tolerance )
			currentRow.emplace_back(sortedAnswerList[i]);

		if( abs(sortedAnswerList[i].y - sortedAnswerList[i - 1].y > tolerance) )
		{
			rowList.emplace_back(currentRow);
			currentRow.clear();
			currentRow.emplace_back(sortedAnswerList[i]);	
		}
	}

	if(currentRow.empty() == false)
		rowList.emplace_back(currentRow);


	std::vector<cv::Rect> sorted;	
	for(auto& row : rowList)
	{
		std::sort(row.begin(), row.end(), [](const cv::Rect rect_1, const cv::Rect rect_2)
		{	
			return rect_1.x < rect_2.x;
		});
    	sorted.insert(sorted.end(), row.begin(), row.end());
	}

    int answerBoundSize = sorted.size();
	int answerYCount = 20;
	int answerXCount = answerBoundSize / answerYCount;
	
	std::vector<int> deletionList;
	for(int i = 0; i < answerXCount; i++)
	{
		if(i % 6 == 0)
		{	
			deletionList.emplace_back(i);
			for(int y = 0; y < answerYCount; y++)
			{
				int yIndex = (answerXCount +  y * answerXCount) + i;
				if(yIndex < answerBoundSize)
					deletionList.emplace_back(yIndex);
			}
		}
	}

	std::sort(deletionList.begin(), deletionList.end());

	// delete the vectors from the list
	for(auto i = deletionList.rbegin(); i != deletionList.rend(); i++)
		sorted.erase(sorted.begin() + *i);

    _answerBounds = sorted;    


    // converting the answerbounds and then changing them into list of answerLists
    int boundSize = _answerBounds.size();
	int yCount = 20;
	int xCount = boundSize / yCount;

	for(int i = 0; i < 6; i++)
	{
		std::vector<cv::Rect> currentQuestion;
		for(int j = 0; j < yCount; j++)
		{
			int questionIndex = j * xCount + (i * 5);
			currentQuestion.emplace_back(_answerBounds[questionIndex]);

			if(i != 5)	
				for(int k = questionIndex + 1; k < (questionIndex + 4) + 1; k++)
					currentQuestion.emplace_back(_answerBounds[k]);
			
			if(i == 5)
				for(int k = questionIndex + 1; k < (questionIndex + 4); k++)
					currentQuestion.emplace_back(_answerBounds[k]);

			_extractedAnswerList.emplace_back(currentQuestion);
			currentQuestion.clear();
		}
	}
}


float Answer::getShadedIndex(cv::Rect bound)
{
    float currentIndex = 0.f;
    for(int y = bound.tl().y; y < bound.br().y; y++)
        for(int x = bound.tl().x; x < bound.br().x; x++)
        {
			cv::Vec3b pixel = _transformedAnswerBlock.at<cv::Vec3b>(y, x);
            int gray = (pixel[0] + pixel[1] + pixel[2]) /3;
            currentIndex += gray;
        }

    currentIndex /= (bound.width * bound.height);
    return currentIndex;
}


float Answer::evaluateAnswer()
{   
    struct Choice
    {
        float value = 0.f;
        unsigned char choiceValue; 
    };

    for(int i = 0; i < _answerList.size(); i++)
    {
        std::vector<Choice> choiceList;
        choiceList.reserve(_extractedAnswerList[i].size());

        for(int j = 0; j < _extractedAnswerList[i].size(); j++)
        {
            choiceList.emplace_back();
            if(i < 100)
                choiceList.back().value = getShadedIndex(_extractedAnswerList[i][j]);
            
            if(i >= 100)
            {
                float shadedIndex = 0.f;
                if(j == 2)
                    shadedIndex = getShadedIndex(_extractedAnswerList[i][j]) - 0.2;
                if(j != 2)
                    shadedIndex = getShadedIndex(_extractedAnswerList[i][j]);

                choiceList.back().value = shadedIndex;
            }

            choiceList.back().choiceValue = static_cast<unsigned char>(65 + j);
            std::cout << static_cast<unsigned char>(65 + j) << "  " << i << '\n';
        }

        std::sort(choiceList.begin(), choiceList.end(), [](Choice choice1, Choice choice2)
		{	
			return choice1.value < choice2.value; 
		});

        if(choiceList[0].choiceValue == _answerList[i])
        {
            int answerIndex = std::abs(65 - choiceList[0].choiceValue);
            cv::rectangle(_transformedAnswerBlock, _extractedAnswerList[i][answerIndex].tl(), _extractedAnswerList[i][answerIndex].br(), cv::Scalar(40, 255, 0), 2);
            _evaluation++;
        }

        if(choiceList[0].choiceValue != _answerList[i])
        {
            int answerIndex = std::abs(65 - _answerList[i]);
            cv::rectangle(_transformedAnswerBlock, _extractedAnswerList[i][answerIndex].tl(), _extractedAnswerList[i][answerIndex].br(), cv::Scalar(0, 0, 255), 2);
        }
    }
    
    return (_evaluation / _answerList.size()) * 100;
}