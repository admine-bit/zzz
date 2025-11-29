#include<iostream>
#include<opencv2/opencv.hpp>
cv::Mat img_hsv,img_bina,img_erod,img_Yello;
int h_low = 20;
int h_hig = 35;
void get_img(int,void*)
{
    cv::namedWindow("DISPLAY",cv::WINDOW_NORMAL);
    cv::resizeWindow("DISPLAY",800,800);
    cv::Mat img = cv::imread("/home/ad/OpenCV-try/stu3.png");
    cv::cvtColor(img,img_hsv,cv::COLOR_BGR2HSV);
    cv::Scalar yel_low = cv::Scalar(h_low,50,50);
    cv::Scalar yel_upp = cv::Scalar(h_hig,255,255);
    cv::inRange(img_hsv,yel_low,yel_upp,img_Yello);
   
    cv::imshow("DISPLAY",img_Yello);
}
int main(int argc,char** argv)
{
    cv::namedWindow("DISPLAY",cv::WINDOW_NORMAL);
    cv::resizeWindow("DISPLAY",800,800);
    cv::createTrackbar("h_low","DISPLAY",&h_low,179,get_img);
    cv::createTrackbar("h_hight","DISPLAY",&h_hig,179,get_img);

    
    get_img(0,0);
    cv::waitKey(0);

    return 0;
}