#include <opencv2/opencv.hpp>


static float calculateShade(cv::Mat& image, cv::Rect bound)
{
    float currentIndex = 0.f;
    for(int y = bound.tl().y; y < bound.br().y; y++)
        for(int x = bound.tl().x; x < bound.br().x; x++)
        {
			cv::Vec3b pixel = image.at<cv::Vec3b>(y, x);
            int gray = (pixel[0] + pixel[1] + pixel[2]) /3;
            currentIndex += (gray / 255.f);
        }
    currentIndex /= (bound.width * bound.height);
    return currentIndex;
}