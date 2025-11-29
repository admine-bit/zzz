#include<iostream>
#include<opencv2/opencv.hpp>
#include<math.h>
int main(int argc,char** argv)
{
    cv::namedWindow("atd",cv::WINDOW_NORMAL);
    cv::resizeWindow("atd",900,900);
    cv::namedWindow("light",cv::WINDOW_NORMAL);
    cv::resizeWindow("light",900,900);
    std::cout << "OpenCV版本: " << CV_VERSION << std::endl;
    cv::VideoCapture cap("/home/ad/OpenCV-try/test.mp4");
    if (!cap.isOpened())
     {
    std::cout << "错误：无法打开视频文件！" << std::endl;
    return -1;
    }
    cv::Mat get_F;
    int frame_count = 0;
    
    while(true)
    {
        cap >> get_F;
        if(get_F.empty())break;
        frame_count++;
        cv::Mat gfgray;
        cv::cvtColor(get_F,gfgray,cv::COLOR_BGR2GRAY);
        cv::Mat gflight;
        cv::threshold(gfgray,gflight,150,255,cv::THRESH_BINARY);

        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(gflight, gflight, cv::MORPH_OPEN, kernel);
        cv::imshow("atd",gflight);




        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(gflight,contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Rect> lig_rects;

        for(const auto& contour : contours)
        {
            double area = cv::contourArea(contour);
            if(area >1 && area < 2000)
            {
                cv::Rect rect = cv::boundingRect(contour);
                float ratio = (float)rect.width / rect.height;
                lig_rects.push_back(rect);

                // if(ratio > 3 && rect.height > 50 && rect.width < 30)
                // {
                //     lig_rects.push_back(rect);
                // }
            }
        }
        for(const auto& rect : lig_rects)
        {
            cv::rectangle(get_F,rect,cv::Scalar(0,255,0),2);
        }
        
        if (lig_rects.size() >= 2)
        {
            double min_distance = 1000;  
            int best_i = 0, best_j = 1;
            
            for(int i = 0; i < lig_rects.size(); i++) 
            {
                for(int j = i + 1; j < lig_rects.size(); j++) 
                {
                    cv::Point center1 = (lig_rects[i].tl() + lig_rects[i].br()) / 2;
                    cv::Point center2 = (lig_rects[j].tl() + lig_rects[j].br()) / 2;
                    double dist = std::sqrt(std::pow(center1.x - center2.x, 2) + 
                                          std::pow(center1.y - center2.y, 2));
                    double height_diff = std::abs(lig_rects[i].height - lig_rects[j].height);
                    double width_diff = std::abs(lig_rects[i].width - lig_rects[j].width);
                    double y_diff = std::abs(center1.y - center2.y);
                    
                    
                    bool similar_size = (height_diff < 20 && width_diff < 10);
                    bool horizontal_aligned = (y_diff < 30);
                    
                    if(dist < min_distance && similar_size && horizontal_aligned)
                    {
                        min_distance = dist;
                        best_i = i;
                        best_j = j;
                    }
                }
            }
            if(min_distance > 60 && min_distance < 130)
            {
                cv::Rect big_rect = lig_rects[best_i] | lig_rects[best_j];
                cv::rectangle(get_F, big_rect, cv::Scalar(0, 0, 255), 3);
                cv::Point center1 = (lig_rects[best_i].tl() + lig_rects[best_i].br()) / 2;
                cv::Point center2 = (lig_rects[best_j].tl() + lig_rects[best_j].br()) / 2;
                cv::line(get_F, center1, center2, cv::Scalar(255, 0, 0), 2);
            }
        }
        cv::imshow("light",get_F);
        
        if (cv::waitKey(30) == 27) 
        {
            break;        return 0;

        }
    }
    
        return 0;
    
}