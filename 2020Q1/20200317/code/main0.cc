#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
  cv::Mat img = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

  //宽度
  std::cout << "宽度： "<< img.cols << std::endl;
  //高度
  std::cout << "高度： " << img.rows << std::endl;
  //通道数
  std::cout << "通道数： " << img.channels() << std::endl;
  //深度
  //elemSize函数返回的是一个像素占用的字节数
  std::cout << "深度： " << img.elemSize() / img.channels() * 8 << std::endl;
  
  cv::imwrite("test_copy.png", img);

  cv::imshow("hello", img);
  cv::waitKey();

  for (int r = 0; r < img.rows; ++r) {
    for (int c = 0; c < img.cols; ++c) {
      int data = img.at<unsigned char>(r, c);
      std::cout << data << std::endl;
    }
  }

  unsigned char * img_data = reinterpret_cast<unsigned char *>(img.data);
  for(int r = 0; r < img.rows; ++r) {
    for(int c = 0; c < img.cols; ++c) {
      std::cout << static_cast<float>(*img_data) << std::endl;
      ++img_data;
    }
  }

  for(int r = 0; r < img.rows; ++r) {
    unsigned char * rdata = img.ptr<unsigned char>(r);
    for(int c = 0; c < img.cols; ++c) {
      std::cout << static_cast<float>(*rdata) << std::endl;
      ++rdata;
    }
  }

  return 0;
}
