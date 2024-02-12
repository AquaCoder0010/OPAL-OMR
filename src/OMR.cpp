#include "OMR.hpp"

OMR::OMR()
    : _cameraPointer(nullptr)
{
    _baseImage = cv::imread("rsrc/base.jpg");
}

OMR::OMR(cv::VideoCapture *camera)
    : _cameraPointer(nullptr)
{
    _cameraPointer = camera;
    _baseImage = cv::imread("rsrc/base.jpg");
}

void OMR::getImage()
{
    Preprocessor preprocessor_;
    Answer answer;
    InfoGrabber infoGrabber;

    answer.loadAnswer("rsrc\\answer-text.txt") ? std::cout << "Answer file loaded" << '\n' : std::cout << "Error answer file not loaded";
    cv::Mat custBaseImage = cv::imread("rsrc/sim/baseImage.jpg");
    preprocessor_.setBaseImage(custBaseImage);

    // cv::Size displaySize(640, 640);

    // bool testdummy = true;

    if (_cameraPointer != nullptr)
    {
        cv::Mat transImage = cv::Mat();
        while (true)
        {
            _cameraPointer->read(_currentImage);

            cv::rotate(_currentImage, _currentImage, cv::ROTATE_90_CLOCKWISE);
            cv::Mat transformedImage = preprocessor_.getTransformedImage(_currentImage);

            cv::imshow("_currentImage", _currentImage);
            cv::imshow("transformedImage", transformedImage);
            if (cv::waitKey(25) == 10)
            {
                if (cv::countNonZero(transformedImage) != 0)
                {
                    transImage = transformedImage;
                    break;
                }
            }
        }
        infoGrabber.setCurrentImage(transImage);
        infoGrabber.getInfo();

        answer.setImage(transImage);
        answer.preprocess();
        answer.extractAnswerBounds();
        float evaluation = answer.evaluateAnswer();
        std::cout << "Evaluation : " << evaluation << '\n';

        cv::Scalar evaluationColor(0, 255 * (evaluation / 100), 255 * (1 - (evaluation / 100)));
        cv::Scalar color(0, 255, 0);
        cv::putText(transImage, std::to_string(static_cast<int>(evaluation)), cv::Point(transImage.cols / 2, transImage.rows / 0.5), cv::FONT_HERSHEY_PLAIN, 5, evaluationColor, 4);

        cv::putText(transImage, ("BC:" + infoGrabber.getBookletCode()), cv::Point(transImage.cols / 4 - 25, transImage.rows / 5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);
        cv::putText(transImage, ("SC:" + infoGrabber.getSubjectCode()), cv::Point(transImage.cols / 4 - 50, transImage.rows / 5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

        cv::putText(transImage, ("RN : " + infoGrabber.getRegisterNumber()), cv::Point(infoGrabber._registerImagePoints[0].x, infoGrabber._registerImagePoints[0].y + 30), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

        cv::imshow("currentImage", _currentImage);
        cv::imshow("transImage", transImage);
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    // if(_cameraPointer != nullptr)
    // {
    //     while(true)
    //     {
    //         _cameraPointer->read(_currentImage);
    //         cv::rotate(_currentImage, _currentImage, cv::ROTATE_90_CLOCKWISE);

    //         preprocesser.setInputImage(_currentImage);
    //         preprocesser.edgeDetect();
    //         int returnValue = preprocesser.findPaper();
    //         if(returnValue == 0)
    //         {
    //             cv::imshow("_transformedImage", preprocesser._transformedImage);

    //             preprocesser.drawPoints(_currentImage);
    //             answer.setImage(preprocesser._transformedImage);
    //             answer.preprocess();
    //             if(answer._transformedAnswerBlock.empty() == false)
    //             {
    //                 answer.extractAnswerBounds();
    //                 float evaluation = answer.evaluateAnswer();

    //                 cv::Scalar evaluationColor(0, 255 * (evaluation/100), 255*(1-(evaluation/100)));
    //                 cv::Scalar color(0, 255, 0);
    //                 cv::putText(preprocesser._transformedImage, std::to_string(static_cast<int>(evaluation)), cv::Point(preprocesser._transformedImage.rows/2 - 50, preprocesser._transformedImage.cols*0.8), cv::FONT_HERSHEY_PLAIN, 5, evaluationColor, 4);

    //                 cv::putText(preprocesser._transformedImage, ("BC:" + infoGrabber.getBookletCode()), cv::Point(preprocesser._transformedImage.rows/4 - 25, preprocesser._transformedImage.cols/5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);
    //                 cv::putText(preprocesser._transformedImage, ("SC:"  + infoGrabber.getSubjectCode()), cv::Point(preprocesser._transformedImage.rows/2 - 30, preprocesser._transformedImage.cols/5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

    //                 cv::putText(preprocesser._transformedImage, ("RN : " + infoGrabber.getRegisterNumber()), cv::Point(infoGrabber._registerImagePoints[0].x, infoGrabber._registerImagePoints[0].y + 30), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

    //                 cv::imshow("currentImage", _currentImage);
    //                 cv::imshow("preprocesser._transformedImage", preprocesser._transformedImage);
    //                 cv::waitKey(25);
    //                 cv::destroyAllWindows();
    //             }
    //             if(answer._transformedAnswerBlock.empty() == true)
    //             {
    //                 cv::imshow("_transformedImage", preprocesser._transformedImage);
    //                 cv::imshow("currentImage", _currentImage);
    //                 cv::waitKey(25);
    //             }
    //         }
    //         if(returnValue == -1)
    //         {
    //             cv::imshow("image", _currentImage);
    //             cv::waitKey(25);
    //         }

    //     }
    // }

    // created for testing purposes .. will remove this section after finishing
    // else if (testdummy == true)
    // {
    //     cv::Mat image = cv::imread("rsrc/sim/image-1.jpg");
    //     cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);

    //     preprocessor_.setBaseImage(custBaseImage);
    //     cv::Mat transformedImage = preprocessor_.getTransformedImage(image);
    //     std::cout << transformedImage.empty() << '\n';

    //     cv::imshow("image", image);
    //     if (transformedImage.empty() == false)
    //         cv::imshow("transformedImage", transformedImage);
    //     cv::waitKey(0);
    //     cv::destroyAllWindows();
    // }

    // else if(testdummy == false)
    // {
    //     cv::Mat image = cv::imread("rsrc/data/01sc03bc00.jpg");
    //     cv::Mat transformedImage = cv::Mat();

    //     cv::rotate(image, image, cv::ROTATE_90_CLOCKWISE);

    //     _currentImage = image;

    //     ///// Preprocesser * no i won't fix the spelling.
    //     // get the image points
    //     preprocesser.setInputImage(_currentImage);
    //     preprocesser.edgeDetect();

    //     // cv::imshow("edgeImage", preprocesser._edgeImage);
    //     // cv::imshow("currentImage", _currentImage);
    //     // cv::waitKey(0);
    //     // cv::destroyAllWindows();

    //     // to do list,
    //     // instead of using integers for Error checking, add either exception classes or Enums for better code readability.
    //     // will do It later past me :-p

    //     int returnValue = preprocesser.findPaper();
    //     if(returnValue == 0)
    //     {
    //         preprocesser.drawPoints(_currentImage);
    //     }
    //     // cv::imshow("transformedImage", preprocesser._transformedImage);
    //     // cv::waitKey(0);
    //     // cv::destroyAllWindows();

    //     // cv::imwrite("transformedImage.png", preprocesser._transformedImage);
    //     /////

    //     /////
    //     infoGrabber.setCurrentImage(preprocesser._transformedImage);
    //     infoGrabber.getInfo();
    //     /////

    //     /////// get answer code
    //     answer.setImage(preprocesser._transformedImage);
    //     answer.preprocess();
    //     answer.extractAnswerBounds();
    //     float evaluation = answer.evaluateAnswer();
    //     std::cout << "Evaluation : " << evaluation << '\n';
    //     // cv::imshow("answerBlock", answer._transformedAnswerBlock);
    //     // cv::waitKey(0);
    //     // cv::destroyAllWindows();
    //     ///////

    //     cv::Scalar evaluationColor(0, 255 * (evaluation/100), 255*(1-(evaluation/100)));
    //     cv::Scalar color(0, 255, 0);
    //     cv::putText(preprocesser._transformedImage, std::to_string(static_cast<int>(evaluation)), cv::Point(preprocesser._transformedImage.rows/2 - 50, preprocesser._transformedImage.cols*0.8), cv::FONT_HERSHEY_PLAIN, 5, evaluationColor, 4);

    //     cv::putText(preprocesser._transformedImage, ("BC:" + infoGrabber.getBookletCode()), cv::Point(preprocesser._transformedImage.rows/4 - 25, preprocesser._transformedImage.cols/5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);
    //     cv::putText(preprocesser._transformedImage, ("SC:"  + infoGrabber.getSubjectCode()), cv::Point(preprocesser._transformedImage.rows/2 - 30, preprocesser._transformedImage.cols/5), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

    //     cv::putText(preprocesser._transformedImage, ("RN : " + infoGrabber.getRegisterNumber()), cv::Point(infoGrabber._registerImagePoints[0].x, infoGrabber._registerImagePoints[0].y + 30), cv::FONT_HERSHEY_PLAIN, 5, color, 4);

    //     cv::imshow("currentImage", _currentImage);
    //     cv::imshow("preprocesser._transformedImage", preprocesser._transformedImage);
    //     cv::waitKey(0);
    //     cv::destroyAllWindows();
    // }
}

int OMR::run()
{
    getImage();

    return 0;
}