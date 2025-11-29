#include<iostream>
#include<opencv2/opencv.hpp>

int main()
{
    cv::Mat img(400,400,CV_8UC3,cv::Scalar(50,50,50));
    cv::Mat img2 = cv::imread("/home/ad/OpenCV-try/test.jpeg",cv::IMREAD_COLOR);
    cv::Mat img3 = cv::imread("/home/ad/OpenCV-try/test.jpeg",cv::IMREAD_GRAYSCALE);
    if(img2.empty())
    {
        std::cout << "图像加载失败！" << std::endl;
        std::cout << "图像路径错误"  << std::endl;
        return -1;
    }
    else
    {
        std::cout << "successed get image" << std::endl;
    }
    std::cout << "====the image's baes informations====" << std::endl;
    std::cout << "i got image" << " col: " <<img2.cols <<
    "row: " << img2.rows << std::endl;
    std::cout << "and i will cout channel, kind " << " channel: " << img2.channels()
    << " kind: " << std::endl;
    switch (img2.type())
    {
    case CV_8UC1:
        std::cout << "CV_8UC1(GRAY)";break;
    case CV_8UC3:
        std::cout << "CV_8UC3(COLOR:THREE_CHANNELS)";break;
    case CV_8UC4:
        std::cout << "CV_8UC4(COLOR/ALPHA)";break;
    default:
        std::cout << "else type";break;
    }
    std::cout << "image's rgb " << img2.cols * img2.rows << std::endl;
    std::cout << "====image information caculate====" << std::endl;
    //col: 1400row: 1400
    std::cout << "====xs information===="<<std::endl;
    cv::Rect conner1(1390,1390,10,10);
    std::cout << "avages is :" << std::endl;
    cv::Mat cout1 = img3(conner1);
    cv::Scalar Val1 = cv::mean(cout1);
    std::cout << "channel:" << Val1[0] << std::endl;
    
    cv::Rect center(175,175,1050,1050);
    cv::Mat con = img3.clone()(center);
    cv::rectangle(con,center,cv::Scalar(0,0,255),0);
    bool savesuccess = cv::imwrite("img3.jpg",con);
    if(savesuccess)
    {
        std::cout << "savesuccess" << std::endl;

    }    
    std::cout << " ====output image==== " << std::endl;
    cv::namedWindow("DISPLAY",cv::WINDOW_AUTOSIZE);
    cv::imshow("DISPLAY",img2);
    cv::waitKey(0);

    cv::imshow("DISPLAY",con);
    cv::waitKey(0);
    cv::destroyAllWindows();
    
    return 0;

}