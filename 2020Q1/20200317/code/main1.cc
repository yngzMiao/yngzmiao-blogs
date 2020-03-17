#include <opencv2/opencv.hpp>
#include<iostream>

int main(int argc, char const *argv[])
{
    cv::VideoCapture cap;
    cap.open("test.mp4");

    if (!cap.isOpened())
        return 0;

    int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);             //帧宽度
    int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);           //帧高度
    int totalFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);       //总帧数
    int frameRate = cap.get(CV_CAP_PROP_FPS);                 //帧率 x frames/s

    std::cout << "视频宽度： " << width << std::endl;
    std::cout << "视频高度： " << height << std::endl;
    std::cout << "视频总帧数： " << totalFrames << std::endl;
    std::cout << "帧率： " << frameRate << std::endl;

    cv::VideoWriter wri;
    wri.open("test_copy.avi", CV_FOURCC('M', 'P', '4', '2'), frameRate, cv::Size(width, height));

    cv::Mat frame;
    while(1) {
        cap >> frame;
        if (frame.empty())
            break;
        wri << frame;
    }
    cap.release();
    wri.release();

    return 0;
}