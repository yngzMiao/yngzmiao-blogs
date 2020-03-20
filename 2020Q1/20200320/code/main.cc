#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
  cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
  std::cout << "src: size " << frame_src.size[0] << " " << frame_src.size[1] << std::endl;
  std::cout << "src: type " << frame_src.type() << " " << frame_src.elemSize1() << std::endl;
  std::cout << "src: channel " << frame_src.channels() << std::endl;

  // 图像缩小与放大
  cv::Mat frame_dest_1;
  cv::resize(frame_src, frame_dest_1, cv::Size(), 0.5, 0.5);
  std::cout << "dest_1: size " << frame_dest_1.size[0] << " " << frame_dest_1.size[1] << std::endl;

  cv::Mat frame_dest_2 = cv::Mat::zeros(512, 512, frame_src.type());
  cv::resize(frame_src, frame_dest_2, frame_dest_2.size());
  std::cout << "dest_2: size " << frame_dest_2.size[0] << " " << frame_dest_2.size[1] << std::endl;

  // 图像数据类型转换
  cv::Mat frame_dest_3;
  frame_src.convertTo(frame_dest_3, CV_32F);
  std::cout << "dest_3: type " << frame_dest_3.type() << " " << frame_dest_3.elemSize1() << std::endl;

  // 图像颜色空间转换
  cv::Mat frame_dest_4;
  cv::cvtColor(frame_src, frame_dest_4, CV_GRAY2BGR);
  std::cout << "dest_4: channel " << frame_dest_4.channels() << std::endl;

  // 图像通道分离与合并
  std::vector<cv::Mat> bgr_mat;
  cv::split(frame_src, bgr_mat);
  std::cout << bgr_mat.size() << std::endl;

  cv::Mat frame_src_1 = cv::imread("test.png", cv::IMREAD_COLOR);
  bgr_mat.clear();
  cv::split(frame_src_1, bgr_mat);
  std::cout << bgr_mat.size() << std::endl;

  bgr_mat.pop_back();
  cv::Mat frame_dest_5;
  cv::merge(bgr_mat, frame_dest_5);
  std::cout << frame_dest_5.channels() << std::endl;

  // 图像翻转
  cv::Mat frame_dest_6;
  cv::flip(frame_src, frame_dest_6, -1);

  // 图像旋转
  cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point2f(0, 0), 45, 1);
  cv::Mat frame_dest_7;
  cv::warpAffine(frame_src, frame_dest_7, rot_mat, frame_src.size());

  // 图像滤波平滑
  cv::Mat frame_dest_8;
  cv::blur(frame_src, frame_dest_8, cv::Size(3, 3));
  cv::medianBlur(frame_src, frame_dest_8, 3);
  cv::GaussianBlur(frame_src, frame_dest_8, cv::Size(3, 3), 0, 0);
  cv::bilateralFilter(frame_src, frame_dest_8, 4, 2, 2);

  return 0;
}
