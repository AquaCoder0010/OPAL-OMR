#include "OMR.hpp"



OMR::OMR()
:_cameraPointer(nullptr)
{
    _baseImage = cv::imread("rsrc/base.jpg");
}

OMR::OMR(cv::VideoCapture* camera)
:_cameraPointer(nullptr)
{
    _cameraPointer = camera;
    _baseImage = cv::imread("rsrc/base.jpg");
}

void OMR::getImage()
{
    PreProcessImage preprocesser(&_baseImage);

    if(_cameraPointer != nullptr)
    {
        while(true)
        {
            _cameraPointer->read(_currentImage);
            cv::rotate(_currentImage, _currentImage, cv::ROTATE_90_CLOCKWISE);

            cv::imshow("window", _currentImage);
            cv::imshow("edgeImage", preprocesser._edgeImage);
            cv::waitKey(25); 
        }
    }

    // created for testing purposes .. will remove this section after finishing 
    else
    {
        cv::Mat image = cv::imread("rsrc/image-2.jpg");
        cv::Mat transformedImage = cv::Mat();


        cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
        
        _currentImage = image;

        // get the image points
        preprocesser.edgeDetect(_currentImage);
        
        cv::imshow("edge_window", preprocesser._edgeImage);
        cv::waitKey(0);

        int returnValue = preprocesser.findPaper();
        if(returnValue == 0)
        {
            preprocesser.drawPoints(_currentImage);        
        }
        cv::imshow("window", _currentImage); 
        cv::imshow("transformed_image", preprocesser._transformedImage);
        cv::waitKey(0);
    
    
    }
}

int OMR::run()
{
    getImage();
    return 0;
}