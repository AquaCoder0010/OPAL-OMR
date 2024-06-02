#include <iostream>
#include <opencv2/opencv.hpp>

#include "CLI.hpp"
#include "OMR.hpp"

int main()
{
	cv::VideoCapture camera;
	CLI cli;

	if(cli.startCLI(camera) == false)
		return -1;
	
	OMR omr(&camera);
	return omr.run();	
}