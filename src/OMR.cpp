#include "OMR.hpp"


OMR::OMR(cv::VideoCapture* camera)
:cameraPointer(nullptr)
{
    cameraPointer = camera;
    baseImage = cv::imread("rsrc/base.jpg");
}

void OMR::getImage()
{
    PreProcessImage preprocesser;
    while(true)
    {
        cameraPointer->read(currentImage);
        cv::rotate(currentImage, currentImage, cv::ROTATE_90_CLOCKWISE);

        // get the image points
        preprocesser.edgeDetect(currentImage);
        preprocesser.approxPoly();
        preprocesser.drawPoints(currentImage);


        cv::imshow("window", currentImage);
        cv::imshow("edgeImage", preprocesser.edgeImage);
        cv::waitKey(25); 
    }
}

int OMR::run()
{
    getImage();
    return 0;
}