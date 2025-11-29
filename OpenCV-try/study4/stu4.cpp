#include<iostream>
#include<opencv2/opencv.hpp>
int main(int argc,char** argv)
{
    cv::namedWindow("choise",cv::WINDOW_NORMAL);
    cv::resizeWindow("choise",800,800);
    cv::namedWindow("display",cv::WINDOW_NORMAL);
    cv::resizeWindow("display",800,800);
    cv::Mat img = cv::imread("/home/ad/OpenCV-try/study4.png");
    cv::Mat img_gray;
    cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
    cv::Mat img_bina,result;
    cv::threshold(img_gray, img_bina, 127, 255, cv::THRESH_BINARY);
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img_bina,contours,cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    cv::Mat display;
    cv::cvtColor(img_bina,display,cv::COLOR_GRAY2BGR);

    cv::drawContours(display,contours,-1,cv::Scalar(0,255,0),1);
    for(int i = 0;i < contours.size();i++)
    {
        cv::Rect rect = cv::boundingRect(contours[i]);
        cv::putText(display,std::to_string(i+1),cv::Point(rect.x,rect.y-5),cv::FONT_HERSHEY_SIMPLEX,0.6,cv::Scalar(255,0,0),1);

    }
    cv::imshow("choise",display);
    cv::imshow("display", img); 
    while(true)
    {
        int key = cv::waitKey(0);
        if(key==27)
        {
            break;
        }
        if(key >= '1'&& key <= '9' )
        {
            int index = key - '1';
            if(index < contours.size())
            {
                cv::Mat temp_display;
                display.copyTo(temp_display);
                
                
                cv::Rect rect = cv::boundingRect(contours[index]);
                cv::rectangle(temp_display, rect, cv::Scalar(0, 0, 255), 3);

                 cv::imshow("chiose", temp_display);
            }
        }
    }
    cv::destroyAllWindows();
    return 0;
}