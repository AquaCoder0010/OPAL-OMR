#pragma once 

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <vector>

#include "WrapFunction.hpp"
#include "PointOperations.hpp"
#include "sign.hpp"
#include "CalculateShade.hpp"


struct ImageBounds
{  
    cv::Mat image = cv::Mat();
    std::vector<cv::Rect> bounds;
    std::vector<std::vector<cv::Rect>> sectionList;
};

struct SectionChoice
{
    float value = 0.f;
    int id;
};

class InfoGrabber
{
    private:
        const cv::Rect _studentInfoBlock = cv::Rect(200, 200, 520, 265);
        const int _codeImageWidth = 100;
        const int _codeImageHeight = 215; 
        
        const int _registerImageWidth = 130; 
        const int _registerImageHeight = 220; 

        std::string _subjectCodeStr;
        std::string _bookletCodeStr;
        std::string _registerNumberStr;
    public:
        cv::Mat _currentImage;

        std::vector<cv::Point> _codeImagePoints;
        std::vector<cv::Point> _registerImagePoints;

        ImageBounds _registerBounds;
        ImageBounds _subjectCodeBounds;
        ImageBounds _bookletCodeBounds;

        cv::Mat _codeImage;
        cv::Mat _registerImage;
    public:
        InfoGrabber();
        void setCurrentImage(cv::Mat& image);
        int getInfo();
        std::string getRegisterNumber();
        std::string getBookletCode();
        std::string getSubjectCode();
    private:
        cv::Mat edgeDetect(cv::Mat& image);
        cv::Mat removeOutline(cv::Mat& image);
        cv::Mat removeGrid(cv::Mat& image);
        std::vector<cv::Rect> getBounds(cv::Mat& image, int lowAreaThreshold=30); // works if the image is binary
        std::vector<std::vector<cv::Rect>> createSections(std::vector<cv::Rect>& boundList);
        std::string evaluateCode(cv::Mat& image, std::vector<std::vector<cv::Rect>>& sectionList);        
        // std::string evaluateRegisterNumber(cv::Mat& image, std::vector<std::vector<cv::Rect>>& sectionList);
        
        void preprocesser();
        void processStudentCode();
        void processRegisterNumber();

    private:
        // delete this later
        void drawSection(cv::Mat& image, std::vector<cv::Rect>& boundList)
        {   
            int index = 0;
            for(auto& bound : boundList)
            {
                cv::rectangle(image, bound, cv::Scalar(255, 0, 0));
                cv::putText(image, std::to_string(index), cv::Point(bound.tl().x + 1, bound.tl().y - 1), cv::FONT_HERSHEY_PLAIN, 0.5, cv::Scalar(0, 0, 255)); 
                index++;
            }
        }

        void drawSection(cv::Mat& image, std::vector<cv::Rect>& boundList, cv::Scalar color)
        {   
            int index = 0;
            for(auto& bound : boundList)
            {
                cv::rectangle(image, bound, color);
                cv::putText(image, std::to_string(index), cv::Point(bound.tl().x + 1, bound.tl().y - 1), cv::FONT_HERSHEY_PLAIN, 0.5, color); 
                index++;
            }
        }
};

