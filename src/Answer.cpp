#include "Answer.hpp"

Answer::Answer(cv::Mat& image)
:_answerBlock()
{
    _answerBlock = image;
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