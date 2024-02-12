#include "InfoGrabber.hpp"

InfoGrabber::InfoGrabber()
{

}

void InfoGrabber::setCurrentImage(cv::Mat& image)
{
    _currentImage = image(_studentInfoBlock).clone();
}


cv::Mat InfoGrabber::edgeDetect(cv::Mat& image)
{
    // make a utility function instead of rewriting the same code
    float meanValue = 0.f;
    cv::Scalar meanPixelValue = cv::Scalar();
    cv::Mat edgeImage = cv::Mat();

    edgeImage = image.clone();
    meanPixelValue = cv::mean(edgeImage);

    for (int i = 0; i < meanPixelValue.channels; i++)
        meanValue += meanPixelValue[i];

    meanValue /= meanPixelValue.channels;

    cv::cvtColor(edgeImage, edgeImage, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(edgeImage, edgeImage);
    cv::GaussianBlur(edgeImage, edgeImage, cv::Size(7, 7), 0, 0);
    cv::Canny(edgeImage, edgeImage, meanValue, meanValue * 3, 5);    
    return edgeImage;
}

cv::Mat InfoGrabber::removeOutline(cv::Mat& image)
{
    cv::Mat removed = image.clone();

    cv::cvtColor(removed, removed, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(removed, removed, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);
    
    cv::Mat verticalLines = removed.clone();
    cv::Mat horizontalLines = removed.clone(); 

	cv::Mat morphKernelHorizontal = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontalLines.cols, 1));
	cv::Mat morphKernelVertical = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, verticalLines.cols / 2));
    
    cv::Mat dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

    cv::erode(horizontalLines, horizontalLines, morphKernelHorizontal);
	cv::dilate(horizontalLines, horizontalLines, morphKernelHorizontal);

	cv::erode(verticalLines, verticalLines, morphKernelVertical);
	cv::dilate(verticalLines, verticalLines, morphKernelVertical);

    cv::dilate(horizontalLines, horizontalLines, dilationKernel); cv::dilate(verticalLines, verticalLines, dilationKernel);
    
    removed = removed - horizontalLines;
    removed = removed - verticalLines;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(removed, removed, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(removed, removed, cv::MORPH_OPEN, kernel);

    return removed;
}

cv::Mat InfoGrabber::removeGrid(cv::Mat& image)
{
    std::vector<cv::Vec4i> horizontalLineList;
    std::vector<cv::Vec4i> verticalLineList;
    std::vector<cv::Vec4i> lineList;

    cv::Mat gridRemoved = image.clone();
    cv::Mat binary = image.clone();
    cv::Mat horizontalLines = cv::Mat();
    cv::Mat verticalLines = cv::Mat();


    cv::cvtColor(binary, binary, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(binary, binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);

    horizontalLines = binary.clone();
    verticalLines = binary.clone();

	int horizontalLength = horizontalLines.cols / 10;
	cv::Mat morphKernelHorizontal = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontalLength, 1));
	cv::erode(horizontalLines, horizontalLines, morphKernelHorizontal);
	cv::dilate(horizontalLines, horizontalLines, morphKernelHorizontal);


	int verticalLength = verticalLines.cols / 10;
	cv::Mat morphKernelVertical = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, verticalLength));
	cv::erode(verticalLines, verticalLines, morphKernelVertical);
	cv::dilate(verticalLines, verticalLines, morphKernelVertical);

	cv::Mat dilationKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(horizontalLines, horizontalLines, dilationKernel);
	cv::dilate(verticalLines, verticalLines, dilationKernel);

    cv::HoughLinesP(horizontalLines, horizontalLineList, 1, CV_PI/180, image.cols * 0.3, image.cols * 0.3, image.cols * 0.6);
	cv::HoughLinesP(verticalLines, verticalLineList, 1, CV_PI/180, image.rows * 0.3, image.rows *0.3, image.rows * 0.6);


    lineList.resize(horizontalLineList.size());
	lineList.insert(lineList.begin(), horizontalLineList.begin(), horizontalLineList.end()); 
    lineList.insert(lineList.end(), verticalLineList.begin(), verticalLineList.end());

    for(auto& line : lineList)
	{
		cv::Point point1 = cv::Point(line[0], line[1]);
		cv::Point point2 = cv::Point(line[2], line[3]);

        int extrudeValue = sign(point2.x - point1.x)*10;

		float slope = 0.f;
        if(point1.x != point2.x)
        {
            slope = (point2.y - point1.y) / (point2.x - point1.x);
            float x_1 = point1.x - extrudeValue;
            float x_2 = point2.x + extrudeValue;
            float y_1 = slope * (x_1 - point1.x) + point1.y;
            float y_2 = slope * (x_2 - point2.x) + point2.y;

            cv::Point point1Changed = cv::Point(x_1, y_1);
            cv::Point point2Changed = cv::Point(x_2, y_2);
            point1 = point1Changed;
            point2 = point2Changed;
        }
        
        else if(point1.x == point2.x)
        {
            point1.x = point1.x - extrudeValue;
            point2.x = point2.x + extrudeValue;
        }
		cv::line(gridRemoved, point1, point2, cv::Scalar(255, 255, 255), 2.5);        
    }
    cv::cvtColor(gridRemoved, gridRemoved, cv::COLOR_BGR2GRAY);
    cv::adaptiveThreshold(gridRemoved, gridRemoved, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 15, 2);
    cv::Mat openingKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(gridRemoved, gridRemoved, cv::MORPH_CLOSE, openingKernel);

    return gridRemoved;
}
// only works if the image is binary
std::vector<cv::Rect> InfoGrabber::getBounds(cv::Mat& image, int lowAreaThreshold)
{
    std::vector<cv::Rect> bounds;
    std::vector<std::vector<cv::Point>> imageContours;
    cv::findContours(image, imageContours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_L1);
        
    for(auto& contour : imageContours)
        bounds.emplace_back(cv::boundingRect(contour));

    std::vector<cv::Rect>::iterator enditer = std::remove_if(bounds.begin(), bounds.end(), [&](cv::Rect rect_1) {
        return (rect_1.width * rect_1.height) <= lowAreaThreshold;
    });
    bounds.erase(enditer, bounds.end());
    return bounds;
}

std::vector<std::vector<cv::Rect>> InfoGrabber::createSections(std::vector<cv::Rect>& boundList)
{
    std::vector<std::vector<cv::Rect>> sectionList;
    std::sort(boundList.begin(), boundList.end(), [](const cv::Rect rect1, const cv::Rect rect2)
	{
		return rect1.x < rect2.x;
	});

    int tolerance = 10;
    std::vector<cv::Rect> currentCol;
    currentCol.emplace_back(boundList[0]);

    for(int i = 1; i < boundList.size(); i++)
    {
		if( abs(boundList[i].x - boundList[i - 1].x) <= tolerance )
            currentCol.emplace_back(boundList[i]);

		if( abs(boundList[i].x - boundList[i - 1].x) > tolerance )
        {
            sectionList.emplace_back(currentCol);
            currentCol.clear();
            currentCol.emplace_back(boundList[i]);
        }
    }
    // this is called for the last col
    if(currentCol.size() != 0)
        sectionList.emplace_back(currentCol);
        
    for(auto& section : sectionList)
        std::sort(section.begin(), section.end(), [](const cv::Rect rect1, const cv::Rect rect2){
            return rect1.y < rect2.y;});
    return sectionList;
}



std::string InfoGrabber::evaluateCode(cv::Mat& image, std::vector<std::vector<cv::Rect>>& sectionList)
{
    const float differenceConstant = 0.1f;
    std::string evaluatedStr = ""; 
    
    struct SectionChoice
    {
        float value = 0.f;
        int id;
    };


    // for(int j = 0; j < sectionList.size(); j++)
    // {
    //     std::cout << "( ";      
    //     for(int i = 0; i < sectionList[j].size(); i++)
    //         std::cout << " " << calculateShade(image, sectionList[j][i]) << ",";
    //     std::cout << ")" << '\n';
    // }

    for(int i = 0; i < sectionList.size(); i++)
    {
        std::vector<SectionChoice> sectionChoiceList;
        for(int j = 0; j < sectionList[i].size(); j++)
        {   
            SectionChoice currentChoice;
            currentChoice.value = calculateShade(image, sectionList[i][j]);
            currentChoice.id = j;
            sectionChoiceList.emplace_back(currentChoice);
        }
        std::sort(sectionChoiceList.begin(), sectionChoiceList.end(), [](SectionChoice choice1, SectionChoice choice2){
            return choice1.value < choice2.value;
        });

        evaluatedStr += std::to_string(sectionChoiceList[0].id); 
    }
    
    return evaluatedStr;
}

void InfoGrabber::preprocesser()
{
    if(_currentImage.empty() == true)
        std::cout << "Image is empty" << '\n';

    std::vector<std::vector<cv::Point>> currentContours;
    cv::Mat binaryImage = edgeDetect(_currentImage);


    cv::findContours(binaryImage, currentContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::sort(currentContours.begin(), currentContours.end(), [&](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
    {
        int area1 = cv::contourArea(contour1);
        int area2 = cv::contourArea(contour2);
        return area1 < area2; 
    }); 

    // find the last three largest contours.    
    currentContours.erase(currentContours.begin(), currentContours.begin() + (currentContours.size() - 3));

    std::sort(currentContours.begin(), currentContours.end(), [](std::vector<cv::Point> contour1, std::vector<cv::Point> contour2){
        return cv::boundingRect(contour1).tl().x < cv::boundingRect(contour2).tl().x;
    });
    currentContours.erase(currentContours.begin() + 1); // removes the second element



    for(int i = 0; i < currentContours.size(); i++)
    {
        std::vector<cv::Point> currentPoly;
        float arcLength = cv::arcLength(currentContours[i], true);
        cv::approxPolyDP(currentContours[i], currentPoly, arcLength * 0.02, true);
        
        // TODO convert this function to a utility function.

        if(currentPoly.size() != 4)
        {
            std::cout << "Qualilateral not detected, error within the studdent Info image... " << '\n';
            std::cin.get();
            exit(-1);
        }

        else if(currentPoly.size() == 4 && i == 0)
        {
            std::vector<PointOperations> pointOperationsList(currentPoly.size());

            for (int i = 0; i < currentPoly.size(); i++)
            {
                pointOperationsList[i].index = i;
                pointOperationsList[i].sum_value = currentPoly[i].x + currentPoly[i].y;
                pointOperationsList[i].sub_value = currentPoly[i].x - currentPoly[i].y;
            }
            _registerImagePoints.reserve(4);
            _registerImagePoints.emplace_back(currentPoly[getMinSum(pointOperationsList)]);
            _registerImagePoints.emplace_back(currentPoly[getMaxSub(pointOperationsList)]);
            _registerImagePoints.emplace_back(currentPoly[getMinSub(pointOperationsList)]);
            _registerImagePoints.emplace_back(currentPoly[getMaxSum(pointOperationsList)]);
        }
        
        else if(currentPoly.size() == 4 && i == 1)
        {
            std::vector<PointOperations> pointOperationsList(currentPoly.size());

            for (int i = 0; i < currentPoly.size(); i++)
            {
                pointOperationsList[i].index = i;
                pointOperationsList[i].sum_value = currentPoly[i].x + currentPoly[i].y;
                pointOperationsList[i].sub_value = currentPoly[i].x - currentPoly[i].y;
            }
            _codeImagePoints.reserve(4);
            _codeImagePoints.emplace_back(currentPoly[getMinSum(pointOperationsList)]);
            _codeImagePoints.emplace_back(currentPoly[getMaxSub(pointOperationsList)]);
            _codeImagePoints.emplace_back(currentPoly[getMinSub(pointOperationsList)]);
            _codeImagePoints.emplace_back(currentPoly[getMaxSum(pointOperationsList)]);            
        }
        // TO DO add an exception handler here. or find another more relaiable method for conversion,
    }
    _codeImage = wrap(_currentImage, _codeImagePoints, _codeImageHeight, _codeImageWidth);
    _registerImage = wrap(_currentImage, _registerImagePoints, _registerImageHeight, _registerImageWidth);

    processStudentCode();
    processRegisterNumber();
}

void InfoGrabber::processStudentCode()
{
    cv::Rect bookletCodeRegion = cv::Rect(0, 50, 50, 165);
    cv::Rect subjectCodeRegion = cv::Rect(50, 50, 50, 165);
    
    _subjectCodeBounds.image = _codeImage(subjectCodeRegion).clone();
    _bookletCodeBounds.image = _codeImage(bookletCodeRegion).clone(); 

    cv::Mat subjectCodeBinary = removeOutline(_subjectCodeBounds.image);
    cv::Mat bookletCodeBinary = removeOutline(_bookletCodeBounds.image);
    
    _subjectCodeBounds.bounds = getBounds(subjectCodeBinary);
    _bookletCodeBounds.bounds = getBounds(bookletCodeBinary);
}


void InfoGrabber::processRegisterNumber()
{
    cv::Rect roi = cv::Rect(0, 60, 130, 160);

    _registerBounds.image = _registerImage(roi).clone();
    cv::Mat registerImageBinary = removeGrid(_registerBounds.image);
    _registerBounds.bounds = getBounds(registerImageBinary);
}

int InfoGrabber::getInfo()
{
    preprocesser();
    _subjectCodeBounds.sectionList = createSections(_subjectCodeBounds.bounds);
    _subjectCodeStr = evaluateCode(_subjectCodeBounds.image, _subjectCodeBounds.sectionList);

    _bookletCodeBounds.sectionList = createSections(_bookletCodeBounds.bounds);
    _bookletCodeStr = evaluateCode(_bookletCodeBounds.image, _bookletCodeBounds.sectionList);

    _registerBounds.sectionList = createSections(_registerBounds.bounds);
    _registerNumberStr = evaluateCode(_registerBounds.image ,_registerBounds.sectionList);

    std::cout << _subjectCodeStr << "  " << _bookletCodeStr << "  " << _registerNumberStr << '\n';

    if(_bookletCodeStr == "-1" || _subjectCodeStr == "-1" || _registerNumberStr == "-1")
        return -1;
    return 0;
}

std::string InfoGrabber::getSubjectCode()
{
    return _subjectCodeStr;
}

std::string InfoGrabber::getBookletCode()
{
    return _bookletCodeStr;
}

std::string InfoGrabber::getRegisterNumber()
{
    return _registerNumberStr;
}