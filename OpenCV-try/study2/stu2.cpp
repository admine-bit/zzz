#include<iostream>
#include<opencv2/opencv.hpp>
int main(int argc,char** argv)
{
    cv::Mat img = cv::imread("/home/ad/OpenCV-try/study2.jpg");
    cv::Mat img_gray,img_stel;
    std::vector<cv::Mat>img_ch;
    cv::split(img,img_ch);
    //cv::cvtColor(img_ch[2],img_gray,cv::COLOR_BGR2GRAY);
    cv::namedWindow("DISPLAY",cv::WINDOW_NORMAL);
    cv::resizeWindow("DISPLAY",800,800);
    cv::threshold(img_ch[2],img_stel,185,255,cv::THRESH_BINARY_INV);
    cv::imshow("DISPLAY",img_ch[2]);
    cv::waitKey(0);   
    cv::imshow("DISPLAY",img_stel);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}