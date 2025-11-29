#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <iostream>

class BasketballTracker 
{
private:
    cv::KalmanFilter kalman_filter;
    cv::Mat measurement;
    cv::Rect current_bbox;
    cv::Rect predicted_bbox;
    float ball_radius;
    bool first_detection;
    int lost_frames;
    cv::Point last_position;
    bool has_previous_position;
    float last_speed_y;  // 记录Y方向速度
    
public:
    BasketballTracker() : kalman_filter(4, 2, 0), measurement(2, 1, CV_32F), 
    ball_radius(25), first_detection(true), lost_frames(0), last_position(-1, -1),
    has_previous_position(false), last_speed_y(0) 
    {
        initializeKalmanFilter();
    }

private:
    void initializeKalmanFilter() 
    {
        float dt = 1.0f;
        // 状态向量: [x, y, vx, vy]
        kalman_filter.transitionMatrix = (cv::Mat_<float>(4, 4) <<
            1, 0, dt, 0,
            0, 1, 0, dt,  
            0, 0, 1, 0,
            0, 0, 0, 1);
        
        kalman_filter.measurementMatrix = cv::Mat::zeros(2, 4, CV_32F);
        kalman_filter.measurementMatrix.at<float>(0, 0) = 1;
        kalman_filter.measurementMatrix.at<float>(1, 1) = 1;
        
        // 针对篮球运球特点调整噪声参数
        // Y方向的过程噪声更大，因为上下运动更剧烈
        kalman_filter.processNoiseCov = (cv::Mat_<float>(4, 4) <<
            1e-3, 0,    0,    0,
            0,    1e-2, 0,    0,  // Y方向噪声更大
            0,    0,    1e-3, 0,
            0,    0,    0,    1e-2); // Vy方向噪声更大
        
        // 测量噪声 - 相信检测结果
        cv::setIdentity(kalman_filter.measurementNoiseCov, cv::Scalar::all(1e-3));
        
        cv::setIdentity(kalman_filter.errorCovPost, cv::Scalar::all(1));
        
        kalman_filter.statePost = cv::Mat::zeros(4, 1, CV_32F);
    }

    cv::Rect detectBasketball(const cv::Mat& frame) 
    {
        cv::Mat hsv_frame, mask;
        cv::cvtColor(frame, hsv_frame, cv::COLOR_BGR2HSV);
        
        cv::Scalar lower_orange = cv::Scalar(5, 120, 120);
        cv::Scalar upper_orange = cv::Scalar(20, 255, 255);
        cv::inRange(hsv_frame, lower_orange, upper_orange, mask);
        
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
        
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        
        if (contours.empty()) 
        {
            lost_frames++;
            return cv::Rect(-1, -1, -1, -1);
        }
        
        auto largest_contour = std::max_element(contours.begin(), contours.end(),
            [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) 
            {
                return cv::contourArea(a) < cv::contourArea(b);
            });
        
        double area = cv::contourArea(*largest_contour);
        
        if (area < 50 || area > 5000) 
        {
            lost_frames++;
            return cv::Rect(-1, -1, -1, -1);
        }
        
        lost_frames = 0;
        return cv::boundingRect(*largest_contour);
    }
    
    cv::Rect createBoundingBox(const cv::Point& center, float radius) 
    {
        int size = static_cast<int>(radius * 2.0f);
        return cv::Rect(center.x - size/2, center.y - size/2, size, size);
    }
    
    // 检测篮球运动模式
    std::string detectMotionPattern(float vx, float vy, float prev_vy) 
    {
        // 检测弹跳模式
        if (std::abs(vy) > 20 && prev_vy * vy < 0) {
            return "BOUNCE"; // 速度方向反转，可能是弹跳
        }
        
        // 检测快速下落
        if (vy > 15) {
            return "FAST_DOWN";
        }
        
        // 检测快速上升  
        if (vy < -15) {
            return "FAST_UP";
        }
        
        return "NORMAL";
    }
    
public:
    void processFrame(const cv::Mat& frame) 
    {
        cv::Rect detected_bbox = detectBasketball(frame);
        
        if (detected_bbox.x >= 0 && detected_bbox.y >= 0)
        {
            cv::Point ball_center(detected_bbox.x + detected_bbox.width/2,
                                 detected_bbox.y + detected_bbox.height/2);
            
            float current_radius = (detected_bbox.width + detected_bbox.height) / 4.0f;
            ball_radius = 0.9f * ball_radius + 0.1f * current_radius;
            
            if (first_detection) {
                kalman_filter.statePost.at<float>(0) = ball_center.x;
                kalman_filter.statePost.at<float>(1) = ball_center.y;
                kalman_filter.statePost.at<float>(2) = 0;
                kalman_filter.statePost.at<float>(3) = 0;
                first_detection = false;
                last_position = ball_center;
                has_previous_position = true;
            }
            else if (has_previous_position) {
                // 计算实际速度
                float vx = ball_center.x - last_position.x;
                float vy = ball_center.y - last_position.y;
                
                // 检测运动模式
                std::string motion_pattern = detectMotionPattern(vx, vy, last_speed_y);
                
                // 根据运动模式动态调整参数
                if (motion_pattern == "BOUNCE") {
                    // 弹跳时增加Y方向的过程噪声
                    kalman_filter.processNoiseCov.at<float>(1, 1) = 1e-1;
                    kalman_filter.processNoiseCov.at<float>(3, 3) = 1e-1;
                } else if (motion_pattern == "FAST_DOWN" || motion_pattern == "FAST_UP") {
                    // 快速运动时适度增加噪声
                    kalman_filter.processNoiseCov.at<float>(1, 1) = 5e-2;
                    kalman_filter.processNoiseCov.at<float>(3, 3) = 5e-2;
                } else {
                    // 正常运动
                    kalman_filter.processNoiseCov.at<float>(1, 1) = 1e-2;
                    kalman_filter.processNoiseCov.at<float>(3, 3) = 1e-2;
                }
                
                last_speed_y = vy;
                last_position = ball_center;
            }
            
            // 预测步骤 - 重点在这里
            cv::Mat prediction = kalman_filter.predict();
            cv::Point predict_center(static_cast<int>(prediction.at<float>(0)),
                                   static_cast<int>(prediction.at<float>(1)));
            
            predicted_bbox = createBoundingBox(predict_center, ball_radius);
            
            measurement.at<float>(0) = static_cast<float>(ball_center.x);
            measurement.at<float>(1) = static_cast<float>(ball_center.y);
            
            cv::Mat estimated = kalman_filter.correct(measurement);
            cv::Point kalman_center(static_cast<int>(estimated.at<float>(0)),
                                  static_cast<int>(estimated.at<float>(1)));
            
            current_bbox = createBoundingBox(kalman_center, ball_radius);
            
            // 绘制运动轨迹和预测信息
            drawBoundingBoxes(frame.clone(), detected_bbox, predicted_bbox, current_bbox,
                             ball_center, predict_center, kalman_center);
        }
        else 
        {
            // 未检测到时，根据最后已知的运动状态调整预测
            float current_vy = kalman_filter.statePost.at<float>(3);
            
            // 如果最后是快速下落，增加向下的预测
            if (current_vy > 10) {
                kalman_filter.processNoiseCov.at<float>(1, 1) = 1e-1;
                kalman_filter.processNoiseCov.at<float>(3, 3) = 1e-1;
            }
            
            cv::Mat prediction = kalman_filter.predict();
            cv::Point predict_center(static_cast<int>(prediction.at<float>(0)),
                                   static_cast<int>(prediction.at<float>(1)));
            
            predicted_bbox = createBoundingBox(predict_center, ball_radius);
            drawBoundingBoxes(frame.clone(), cv::Rect(-1,-1,-1,-1), predicted_bbox, current_bbox,
                             cv::Point(-1,-1), predict_center, cv::Point(-1,-1));
            
            // 恢复默认噪声参数
            kalman_filter.processNoiseCov.at<float>(1, 1) = 1e-2;
            kalman_filter.processNoiseCov.at<float>(3, 3) = 1e-2;
        }
    }
    
private:
    void drawBoundingBoxes(cv::Mat frame, const cv::Rect& detected_bbox, 
                          const cv::Rect& predicted_bbox, const cv::Rect& current_bbox,
                          const cv::Point& detected_center, const cv::Point& predicted_center,
                          const cv::Point& kalman_center) 
    {
        // 绘制检测框（红色）
        if (detected_bbox.x >= 0) 
        {
            cv::rectangle(frame, detected_bbox, cv::Scalar(0, 0, 255), 2);
        }
        
        // 绘制预测框（蓝色）- 重点观察这个框的预测效果
        if (predicted_bbox.x >= 0) 
        {
            cv::rectangle(frame, predicted_bbox, cv::Scalar(255, 0, 0), 2);
            
            // // 绘制预测箭头，显示预测方向
            // if (detected_bbox.x >= 0) {
            //     cv::arrowedLine(frame, detected_center, predicted_center, 
            //                    cv::Scalar(255, 255, 0), 2);
            // }
        }
        
        // 绘制当前框（绿色）
        if (current_bbox.x >= 0) 
        {
            cv::rectangle(frame, current_bbox, cv::Scalar(0, 255, 0), 3);
        }
        
        // 显示运动信息
        float vx = kalman_filter.statePost.at<float>(2);
        float vy = kalman_filter.statePost.at<float>(3);
        float speed = std::sqrt(vx*vx + vy*vy);
        
        std::string motion_info = "Vy: " + std::to_string((int)vy) + 
                                 " Speed: " + std::to_string((int)speed);
        cv::putText(frame, motion_info, cv::Point(10, frame.rows - 40), 
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        
        std::string lost_info = "Lost: " + std::to_string(lost_frames);
        cv::putText(frame, lost_info, cv::Point(10, frame.rows - 20), 
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        
        cv::putText(frame, "Basketball Dribble Tracking", cv::Point(10, 30), 
                   cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
        
        cv::imshow("Basketball Dribble Tracking", frame);
    }
};

int main() 
{
    cv::VideoCapture cap("/home/ad/OpenCV-try/test3.mp4");
    
    if (!cap.isOpened()) 
    {
        std::cout << "错误：无法打开视频文件！" << std::endl;
        return -1;
    }
    
    BasketballTracker tracker;
    cv::Mat frame;
    
    while (true) 
    {
        cap >> frame;
        if (frame.empty()) break;
        
        tracker.processFrame(frame);
        
        char key = cv::waitKey(30);
        if (key == 'q' || key == 'Q') break;
        if (key == ' ') cv::waitKey(0);
    }
    
    cap.release();
    cv::destroyAllWindows();
    return 0;
}