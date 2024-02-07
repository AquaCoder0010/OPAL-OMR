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
    Answer answer;
    InfoGrabber infoGrabber;

    answer.loadAnswer("rsrc\\answer-text.txt") ? std::cout << "Answer file loaded" << '\n' : std::cout << "Error answer file not loaded";

    if(_cameraPointer != nullptr)
    {
        while(true)
        {
            _cameraPointer->read(_currentImage);
            cv::rotate(_currentImage, _currentImage, cv::ROTATE_90_CLOCKWISE);

            preprocesser.setInputImage(_currentImage);
            preprocesser.edgeDetect();
            int returnValue = preprocesser.findPaper();
            
            if(returnValue == 0)
            {
                preprocesser.drawPoints(_currentImage);
                answer.setImage(preprocesser._transformedImage);
                answer.preprocess();
                if(answer._transformedAnswerBlock.empty() == false)
                    answer.extractAnswerBounds();
                if(answer._transformedAnswerBlock.empty() == true)
                    cv::imshow("_transformedImage", preprocesser._transformedImage);
   
            }
            if(returnValue == -1)
            {
                cv::imshow("image", _currentImage);
                cv::waitKey(25);    
            }
            
        }
    }

    // created for testing purposes .. will remove this section after finishing 
    else
    {
        cv::Mat image = cv::imread("rsrc/data/01sc03bc00.jpg");
        cv::Mat transformedImage = cv::Mat();

        cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);
        
        _currentImage = image;

        ///// Preprocesser * no i won't fix the spelling.
        // get the image points
        preprocesser.setInputImage(_currentImage);
        preprocesser.edgeDetect();
    
        // cv::imshow("edgeImage", preprocesser._edgeImage);
        // cv::imshow("currentImage", _currentImage);
        // cv::waitKey(0);
        // cv::destroyAllWindows();

        // to do list, 
        // instead of using integers for Error checking, add either exception classes or Enums for better code readability.
        // will do It later past me :-p

        int returnValue = preprocesser.findPaper();
        if(returnValue == 0)
        {
            preprocesser.drawPoints(_currentImage);        
        }
        // cv::imshow("transformedImage", preprocesser._transformedImage);
        // cv::waitKey(0);
        // cv::destroyAllWindows();

        // cv::imwrite("transformedImage.png", preprocesser._transformedImage);
        ///// 

        /////
        infoGrabber.setCurrentImage(preprocesser._transformedImage);
        infoGrabber.getInfo();
        /////


        /////// get answer code
        answer.setImage(preprocesser._transformedImage);   
        answer.preprocess();                
        answer.extractAnswerBounds();
        float evaluation = answer.evaluateAnswer();
        std::cout << "Evaluation : " << evaluation << '\n';
        // cv::imshow("answerBlock", answer._transformedAnswerBlock);
        // cv::waitKey(0);
        // cv::destroyAllWindows();
        /////// 
        
        
        cv::Scalar evaluationColor(0, 255 * evaluation, 255*(1-evaluation));
        cv::Scalar color(0, 255, 0);
        cv::putText(preprocesser._transformedImage, std::to_string(round(evaluation)), cv::Point(preprocesser._transformedImage.rows/2 - 50, preprocesser._transformedImage.cols*0.8), cv::FONT_HERSHEY_PLAIN, 5, evaluationColor, 4);
        
        cv::putText(preprocesser._transformedImage, ("BC:" + infoGrabber.getBookletCode()), cv::Point(preprocesser._transformedImage.rows/4 - 25, preprocesser._transformedImage.cols/5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);
        cv::putText(preprocesser._transformedImage, ("SC:"  + infoGrabber.getSubjectCode()), cv::Point(preprocesser._transformedImage.rows/2 - 30, preprocesser._transformedImage.cols/5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);
        
        cv::putText(preprocesser._transformedImage, ("RN : " + infoGrabber.getRegisterNumber()), cv::Point(infoGrabber._registerImagePoints[0].x, infoGrabber._registerImagePoints[0].y + 30), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

        cv::imshow("currentImage", _currentImage);
        cv::imshow("preprocesser._transformedImage", preprocesser._transformedImage);
        cv::waitKey(0);
        cv::destroyAllWindows();
       
    }
}

int OMR::run()
{
    getImage();
    
    return 0;
}