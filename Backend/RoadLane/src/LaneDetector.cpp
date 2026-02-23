#include "LaneDetector.h"
#include <opencv2/opencv.hpp>
#include <iostream>

void runLaneDetection()
{
    cv::VideoCapture cap("C:/Users/LENOVO/Desktop/RoadLane/video/vid2.mp4");

    if (!cap.isOpened()) {
        std::cout << "video not opened!" << std::endl;
        return;
    }

    cv::Mat frame;
    double fps = 0;
    int64 prevTick = cv::getTickCount();

    // Variables for HUD text
    std::string direction = "Calculating...";
    double smoothAngle = 0;
    double deviationPercent = 0;
    
    // Variables for Crosshair
    int crossX = 640, crossY = 360; 

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::Mat original = frame.clone();
        
        
        cv::resize(frame, frame, cv::Size(640, 360));

        // ================= PREPROCESS =================
        cv::Mat gray, blur, edges;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blur, cv::Size(5,5), 0);
        cv::Canny(blur, edges, 30, 100);

        // ================= ROI =================
        cv::Mat mask = cv::Mat::zeros(edges.size(), CV_8UC1);

        std::vector<cv::Point> roi = {
            {static_cast<int>(frame.cols * 0.1), frame.rows},
            {static_cast<int>(frame.cols * 0.45), static_cast<int>(frame.rows * 0.65)},
            {static_cast<int>(frame.cols * 0.55), static_cast<int>(frame.rows * 0.65)},
            {static_cast<int>(frame.cols * 0.9), frame.rows}
        };

        cv::fillPoly(mask, std::vector<std::vector<cv::Point>>{roi}, 255);

        cv::Mat maskedEdges;
        cv::bitwise_and(edges, mask, maskedEdges);

        // ================= HOUGH =================
        std::vector<cv::Vec4i> lines;
        cv::HoughLinesP(maskedEdges, lines, 1, CV_PI/180, 30, 30, 100);

        std::vector<cv::Vec4i> leftLines, rightLines;

        for (auto &l : lines) {
            double slope = (double)(l[3] - l[1]) / (l[2] - l[0] + 0.001);
            if (fabs(slope) < 0.4) continue;
            if (slope < 0) leftLines.push_back(l);
            else rightLines.push_back(l);
        }

        cv::Mat overlay = frame.clone();

        // ================= FUNCTION TO AVERAGE LINES =================
        auto getLine = [&](std::vector<cv::Vec4i>& vec) {
            if (vec.empty()) return std::make_pair(cv::Point(0,0), cv::Point(0,0));
            double slope = 0, intercept = 0;
            for (auto &l : vec) {
                double m = (double)(l[3] - l[1]) / (l[2] - l[0] + 0.001);
                double b = l[1] - m * l[0];
                slope += m;
                intercept += b;
            }
            slope /= vec.size();
            intercept /= vec.size();

            int y1 = frame.rows;
            int y2 = static_cast<int>(frame.rows * 0.75);
            int x1 = (y1 - intercept) / slope;
            int x2 = (y2 - intercept) / slope;

            return std::make_pair(cv::Point(x1, y1), cv::Point(x2, y2));
        };

        if (!leftLines.empty() && !rightLines.empty()) {
            auto L = getLine(leftLines);
            auto R = getLine(rightLines);

            cv::Point leftBottom = L.first;
            cv::Point leftTop    = L.second;
            cv::Point rightBottom = R.first;
            cv::Point rightTop    = R.second;

            // DRAW LANES on small frame
            cv::line(overlay, leftBottom, leftTop, {0,0,255}, 6);
            cv::line(overlay, rightBottom, rightTop, {255,0,0}, 6);

            // FILL AREA
            std::vector<cv::Point> lanePoly = { leftBottom, leftTop, rightTop, rightBottom };
            
            // ===== CENTER CALCULATION =====
            int laneCenter = (leftBottom.x + rightBottom.x) / 2;
            int frameCenter = frame.cols / 2;
            int deviation = frameCenter - laneCenter;

            double rawAngle = deviation * 0.1;

            // ===== ANGLE SMOOTHING =====
            double alpha = 0.2;
            smoothAngle = alpha * rawAngle + (1 - alpha) * smoothAngle;

            // ===== DIRECTION LOGIC =====
            if (smoothAngle > 5) direction = "Turn Left";
            else if (smoothAngle < -5) direction = "Turn Right";
            else direction = "Go Straight";

            deviationPercent = (double)abs(deviation) / (frame.cols / 2) * 100;

            // ===== HORIZON CROSSHAIR CALCULATION =====

            crossX = ((leftTop.x + rightTop.x) / 2) * 2;
            crossY = ((leftTop.y + rightTop.y) / 2) * 2;

            // ===== SMART CARPET COLOR LOGIC =====
            cv::Scalar carpetColor = cv::Scalar(0, 255, 0); // Normal: Green
            if (abs(smoothAngle) > 4) carpetColor = cv::Scalar(0, 255, 255); // Warning: Yellow
            if (abs(smoothAngle) > 8) carpetColor = cv::Scalar(0, 0, 255);   // Danger: Red

            // Draw Smart Poly
            cv::Mat fill = frame.clone();
            cv::fillPoly(fill, std::vector<std::vector<cv::Point>>{lanePoly}, carpetColor);
            cv::addWeighted(fill, 0.3, overlay, 0.7, 0, overlay);
        }


        cv::resize(overlay, overlay, cv::Size(1280, 720));
        
        // 1. Draw Semi-Transparent HUD Box & Radar Box
        cv::Mat hud = overlay.clone();
        cv::rectangle(hud, cv::Rect(30, 30, 450, 320), cv::Scalar(0, 0, 0), -1);
        
        int radarSize = 180;
        cv::Point radarCenter(1280 - radarSize/2 - 30, 30 + radarSize/2);
        cv::rectangle(hud, cv::Rect(1280 - radarSize - 30, 30, radarSize, radarSize), cv::Scalar(0, 20, 0), -1); 
        
        cv::addWeighted(hud, 0.6, overlay, 0.4, 0, overlay); 

        // 2. Telemetry Text Calculation
        int64 currentTick = cv::getTickCount();
        double time = (currentTick - prevTick) / cv::getTickFrequency();
        fps = 1.0 / time;
        prevTick = currentTick;

        // Fake Speed 
        static int speed = 62;
        if (currentTick % 10 == 0) speed = 60 + (rand() % 6);

        std::string statusText = "Status: SAFE";
        cv::Scalar statusColor = cv::Scalar(0, 255, 0);
        if (abs(smoothAngle) > 8) { statusText = "Status: DANGER"; statusColor = cv::Scalar(0, 0, 255); }
        else if (abs(smoothAngle) > 4) { statusText = "Status: WARNING"; statusColor = cv::Scalar(0, 255, 255); }

        std::string angleText = "Angle: " + std::to_string(smoothAngle).substr(0,5) + " deg";
        std::string devText   = "Dev: " + std::to_string((int)deviationPercent) + " %";
        std::string speedText = "Speed: " + std::to_string(speed) + " km/h";
        std::string fpsText   = "FPS: " + std::to_string((int)fps);

        // 3. Draw Text
        int font = cv::FONT_HERSHEY_DUPLEX;
        cv::putText(overlay, angleText, cv::Point(50, 80), font, 1.0, cv::Scalar(255, 255, 255), 2);
        
        cv::Scalar dirColor = cv::Scalar(0, 255, 0);
        if (direction == "Turn Left") dirColor = cv::Scalar(0, 255, 255);
        if (direction == "Turn Right") dirColor = cv::Scalar(0, 165, 255);
        
        cv::putText(overlay, direction, cv::Point(50, 130), font, 1.0, dirColor, 2);
        cv::putText(overlay, devText,   cv::Point(50, 180), font, 1.0, cv::Scalar(255, 255, 0), 2);
        cv::putText(overlay, speedText, cv::Point(50, 230), font, 1.0, cv::Scalar(255, 100, 100), 2);
        cv::putText(overlay, statusText,cv::Point(50, 280), font, 1.0, statusColor, 2);
        cv::putText(overlay, fpsText,   cv::Point(50, 330), font, 0.8, cv::Scalar(200, 200, 200), 1);

        // 4. Draw Direction Arrow
        cv::arrowedLine(overlay, cv::Point(400, 150), 
                        cv::Point(400 + (smoothAngle > 5 ? -60 : (smoothAngle < -5 ? 60 : 0)), smoothAngle > 5 || smoothAngle < -5 ? 110 : 90), 
                        dirColor, 5, 8, 0, 0.2);

        

        // A. RADAR UI (Top Right)
        cv::rectangle(overlay, cv::Rect(1280 - radarSize - 30, 30, radarSize, radarSize), cv::Scalar(0, 255, 0), 2);
        cv::putText(overlay, "RADAR", cv::Point(1280 - radarSize - 20, 55), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
        
        // Radar Center Dot (Our Car)
        cv::circle(overlay, radarCenter, 6, cv::Scalar(0, 0, 255), -1);
        cv::circle(overlay, radarCenter, 15, cv::Scalar(0, 255, 0), 1); // Ping circle
        
        // Dynamic Lanes in Radar
        int laneOffset = (int)(smoothAngle * 3.0); // Amplify angle for radar visual
        cv::line(overlay, cv::Point(radarCenter.x - 35 + laneOffset, radarCenter.y - 50), 
                          cv::Point(radarCenter.x - 45, radarCenter.y + 60), cv::Scalar(255, 0, 0), 3); // Left lane (Red)
        cv::line(overlay, cv::Point(radarCenter.x + 35 + laneOffset, radarCenter.y - 50), 
                          cv::Point(radarCenter.x + 45, radarCenter.y + 60), cv::Scalar(0, 0, 255), 3); // Right lane (Blue)

        // B. TARGETING CROSSHAIR (Road Horizon)
        cv::circle(overlay, cv::Point(crossX, crossY), 15, cv::Scalar(0, 255, 0), 2);
        cv::circle(overlay, cv::Point(crossX, crossY), 2, cv::Scalar(0, 255, 0), -1);
        cv::line(overlay, cv::Point(crossX - 25, crossY), cv::Point(crossX + 25, crossY), cv::Scalar(0, 255, 0), 2);
        cv::line(overlay, cv::Point(crossX, crossY - 25), cv::Point(crossX, crossY + 25), cv::Scalar(0, 255, 0), 2);

        // Exit text
        cv::putText(overlay, "Press Q to exit", cv::Point(1280 - 200, 720 - 30), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);

        // ==============================================================

        cv::imshow("Lane Detection", overlay);

        if (cv::waitKey(1) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
}