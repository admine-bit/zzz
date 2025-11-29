#include<iostream>
#include<opencv2/opencv.hpp>

int main(int argc,char** argv)
{
    cv::Mat img = cv::imread("/home/ad/OpenCV-try/study.jpg");
    cv::namedWindow("DISPLAY",cv::WINDOW_NORMAL);
    cv::resizeWindow("DISPLAY",800,800);
    std::vector<cv::Mat> chals;
    cv::split(img.clone(),chals);
    cv::Mat blue = cv::Mat::zeros(img.size(),img.type());
    std::vector<cv::Mat> Bluechal;
    Bluechal.push_back(chals[0]);
    Bluechal.push_back(cv::Mat::zeros(img.size(),CV_8UC1));
    Bluechal.push_back(cv::Mat::zeros(img.size(),CV_8UC1));
    cv::merge(Bluechal,blue);

    cv::Mat green = cv::Mat::zeros(img.size(),img.type());
    std::vector<cv::Mat> Greenchal;
    Greenchal.push_back(cv::Mat::zeros(img.size(),CV_8UC1));
    Greenchal.push_back(chals[1]);
    Greenchal.push_back(cv::Mat::zeros(img.size(),CV_8UC1));
    cv::merge(Greenchal,green);

    cv::Mat red = cv::Mat::zeros(img.size(),img.type());
    std::vector<cv::Mat> Redchal;
    Redchal.push_back(cv::Mat::zeros(img.size(),CV_8UC1));
    Redchal.push_back(cv::Mat::zeros(img.size(),CV_8UC1));
    Redchal.push_back(chals[2]);
    cv::merge(Redchal,red);

    cv::imshow("DISPLAY",blue);
    cv::waitKey(3000);
    cv::imshow("DISPLAY",green);
    cv::waitKey(3000);
    cv::imshow("DISPLAY",red);
    cv::waitKey(3000);



    
    cv::destroyAllWindows();
    return 0;
}