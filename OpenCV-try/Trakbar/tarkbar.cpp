#include<iostream>
#include<opencv2/opencv.hpp>
cv::Mat img = cv::imread("/home/ad/OpenCV-try/test.jpeg");
cv::Mat gaussian_img;
int ksize = 5;
void GAUSSIANBLUR(int,void*)
{
    int auc_size = ksize * 2 + 1;
    cv::GaussianBlur(img,gaussian_img,cv::Size(auc_size,auc_size),0);
    cv::imshow("DISPLAY",gaussian_img);
}
int main(int argc,char** argv)
{
    if(img.empty())return 1;
    cv::namedWindow("DISPLAY",cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("GAUSSIAN","DISPLAY",&ksize,15,GAUSSIANBLUR);
    GAUSSIANBLUR(0,0);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}