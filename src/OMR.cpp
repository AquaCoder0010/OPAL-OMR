#include "OMR.hpp"



OMR::OMR()
:cameraPointer(nullptr)
{

}

OMR::OMR(cv::VideoCapture* camera)
:cameraPointer(nullptr)
{
    cameraPointer = camera;
    baseImage = cv::imread("rsrc/base.jpg");
}

void OMR::getImage()
{
    PreProcessImage preprocesser(&baseImage);

    if(cameraPointer != nullptr)
    {
        while(true)
        {
            cameraPointer->read(currentImage);
            cv::rotate(currentImage, currentImage, cv::ROTATE_90_CLOCKWISE);

            cv::imshow("window", currentImage);
            cv::imshow("edgeImage", preprocesser.edgeImage);
            cv::waitKey(25); 
        }
    }

    // created for testing purposes .. will remove this section after finishing 
    else
    {
        cv::Mat image = cv::imread("rsrc/image.jpg");
        cv::Mat baseImage = cv::imread("rsrc/base.jpg");

        cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
        
        currentImage = image;

        // get the image points
        preprocesser.edgeDetect(currentImage);

        cv::imshow("edgedetected", preprocesser.edgeImage);
        cv::waitKey(0);

        preprocesser.approxPoly();
        preprocesser.drawPoints(currentImage);

        cv::imshow("window", currentImage); 
        cv::waitKey(0);
    }
}

int OMR::run()
{
    getImage();
    return 0;
}