#include<iostream>
#include<opencv2/opencv.hpp>

int main()
{
    int charge = 0;
    cv::Mat img = cv::imread("/home/ad/OpenCV-try/test.jpeg");
    cv::Mat img2 = cv::imread("/home/ad/OpenCV-try/test2.jpeg");
    cv::Mat result,img3;
    cv::cvtColor(img2,img3,cv::COLOR_BGR2GRAY);
    cv::namedWindow("DISPLAY",cv::WINDOW_AUTOSIZE);
    if(img.empty())
    {
        std::cout << "error!" << std::endl;
    }
    std::cout << "请输入你想改变的图像风格：" << std::endl;
    std::cout << "1.彩色图像；2.灰度图像；3.通透的彩色图像。" << std:: endl;
    std::cin >> charge;
    switch (charge)
    {
    case 1:
        cv::cvtColor(img2,result,cv::COLOR_GRAY2BGR);
        std::cout << "输出彩色图像" << std::endl;
        break;
    case 2:
        cv::cvtColor(img,result,cv::COLOR_BGR2GRAY);
        std::cout << "输出灰色图像" << std::endl;
        break;
    case 3:
        cv::cvtColor(img,result,cv::COLOR_BGR2Lab);
        std::cout << "输出带透明通路的图像" << std::endl;
        break;
    default:
        break;
    }
    cv::imshow("DISPLAY",result);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}