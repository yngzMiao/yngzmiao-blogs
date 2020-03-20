俗话说：好记性不如烂笔头。在使用OpenCV的过程中，时常会用到很多函数，而且往往可能会一时记不起这个函数的具体参数怎么设置，故在此将常用函数做一汇总。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200320](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200320)。

<br/>

### 图像缩放与放大
对图像的各项操作中，放大或者缩小图像是最常见的操作之一。OpenCV提供`resize`函数来完成这项功能，其原型为：
```c
void resize(InputArray src, OutputArray dst,
                        Size dsize, double fx = 0, double fy = 0,
                        int interpolation = INTER_LINEAR);
```
其中，src参数表示输入图像，dst参数表示输出图像，dsize参数表示输出图像的大小。**如果这个参数不为0，那么就代表将原图像缩放到这个Size(width，height)指定的大小；如果这个参数为0，那么原图像缩放之后的大小就要通过下面的公式来计算**：
```c
dsize = Size(round(fx ∗ src.cols), round(fy ∗ src.rows))
```
其中，fx和fy就是下面要说的两个参数，是图像width方向和height方向的缩放比例。fx：width方向的缩放比例，如果它是0，那么它就会按照(double)dsize.width/src.cols来计算；fy：height方向的缩放比例，如果它是0，那么它就会按照(double)dsize.height/src.rows来计算；interpolation参数表示插值的方式，图像缩放之后，肯定像素要进行重新计算的，就靠这个参数来指定重新计算像素的方式。

resize提供的插值方式包括：

| 插值方式 | 含义 |
| :---: | :---: |
| INTER_NEAREST | 最近邻插值 |
| INTER_LINEAR | 线性插值(默认) |
| INTER_AREA | 区域插值 |
| INTER_CUBIC | 三次样条插值 |
| INTER_LANCZOS4 | Lanczos插值 |

如果想要了解这五种方式的区别和实现：[OpenCV中resize函数五种插值算法的实现过程](https://blog.csdn.net/fengbingchun/article/details/17335477)、[OpenCV图像缩放resize各种插值方式的比较](https://blog.csdn.net/guyuealian/article/details/85097633)。

同时，使用resize函数需要注意以下几点：
* **dsize和fx/fy不能同时为0**，要么指定好dsize的值，让fx和fy空置直接使用默认值，要么将dsize置零，指定fx和fy；
* 插值方法，正常情况下使用默认的双线性插值就够用了，几种常用方法的效率是：最邻近插值>双线性插值>双立方插值>Lanczos插值。

一般在实际使用过程中，既可以通过`resize`来计算，也可以指定好输出图像的大小。例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
    std::cout << "src: size " << frame_src.size[0] << " " << frame_src.size[1] << std::endl;

    cv::Mat frame_dest_1;
    cv::resize(frame_src, frame_dest_1, cv::Size(), 0.5, 0.5);
    std::cout << "dest_1: size " << frame_dest_1.size[0] << " " << frame_dest_1.size[1] << std::endl;

    cv::Mat frame_dest_2 = cv::Mat::zeros(512, 512, frame_src.type());
    cv::resize(frame_src, frame_dest_2, frame_dest_2.size());
    std::cout << "dest_2: size " << frame_dest_2.size[0] << " " << frame_dest_2.size[1] << std::endl;

    return 0;
}

```

### 图像数据类型转换
一般情况下，常用CV_8U来存放像素点，每个像素点的取值在0-255之间。但是有时候，也需要将每个像素点的取值转化成CV_32F、CV_8S等格式。OpenCV提供`convertTo`函数来实现，其定义为：
```c
void convertTo(OutputArray m, int rtype, double alpha=1, double beta=0) const;
```
其中，m参数表示生成的输出图像，rtype参数表示输出图像的类型，由于**convertTo不能修改通道数，所以rtype修改的是图像的位深度**。如果rtype为负值，输入图像和输出图像将使用同样的类型。alpha参数表示尺度变换因子；beta参数表示附加到尺度变换后的值上的偏移量。

函数将输入图像中的像素值转换到输出图像。最后会使用溢出保护函数`saturate_cast<>`，以避免转换过程中可能出现的溢出。函数执行如下运算：
```c
m(x, y) = saturate_cast < rtype> (alpha(*this)(x, y) + beta)
```

为什么使用溢出保护函数saturate_cast？

**在像素值的计算过程中，由于尺度变换因子和偏移量的存在，可能使像素值超过其取值范围，这时候需要对其进行溢出保护**。saturate_cast<>模板函数的原理如下，以CV_8U为例：
```c
if (data < 0)
    data = 0;
else if (data > 255)
	data = 255;
```

例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
  cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
  std::cout << "src: type " << frame_src.type() << " " << frame_src.elemSize1() << std::endl;

  cv::Mat frame_dest_3;
  frame_src.convertTo(frame_dest_3, CV_32F);
  std::cout << "dest_3: type " << frame_dest_3.type() << " " << frame_dest_3.elemSize1() << std::endl;

  return 0;
}
```

### 图像颜色空间转换
日常大多数看到的彩色图片都是RGB类型，但是在进行图像处理时，需要用到灰度图、二值图、HSV、HSI等颜色制式，OpenCV提供了`cvtColor`函数来实现这些功能。其函数定义为：
```c
void cvtColor(InputArray src, OutputArray dst, int code, int dstCn = 0);
```
其中，src参数表示输入图像，dst参数表示输出图像，code参数表示颜色映射类型，通常格式为cv_xxx2xxx；dstCn参数表示输出图像的通道数，取0则表示由src和code来决定。

常见的颜色映射类型如：CV_GRAY2BGR、CV_BGR2GRAY、CV_BGR2RGB、CV_RGB2BGR等等。更多的颜色映射类型可以查看：[opencv学习（十六）之颜色空间转换cvtColor()](https://blog.csdn.net/keith_bb/article/details/53470170)。

例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
    std::cout << "src: channel " << frame_src.channels() << std::endl;

    cv::Mat frame_dest_4;
    cv::cvtColor(frame_src, frame_dest_4, CV_GRAY2BGR);
    std::cout << "dest_4: channel " << frame_dest_4.channels() << std::endl;

    return 0;
}
```

### 图像通道分离与合并
在图像处理中，尤其是处理多通道图像时，有时需要对各个通道进行分离，分别处理；有时还需要对分离处理后的各个通道进行合并，重新合并成一个多通道的图像。OpenCV中实现图像通道的合并与分离的函数分别是`split`和`merge`，其定义如下：
```c
void split(const Mat& src, Mat* mvbegin);
void split(InputArray m, OutputArrayOfArrays mv);

void merge(const Mat* mv, size_t count, OutputArray dst);
void merge(InputArrayOfArrays mv, OutputArray dst);
```
其中，split函数的第一个参数表示输入图像，第二个参数表示分离的各通道数据；merge函数的第一个参数表示需要合并的各通道数据，第二个参数表示输出图像。

同时，**在实际使用过程中，一般将分离后或待合并的多通道数据放在std::vector< Mat >中**。例如：

```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
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

    return 0;
}
```

### 图像翻转
图像翻转一般包括垂直翻转、水平翻转、水平垂直翻转三种。OpenCV提供了`flip`函数来实现，其函数定义为：
```c
void flip(InputArray src, OutputArray dst, int flipCode);
```
其中，src参数表示输入图像，dst参数表示输出图像，flipCode参数表示翻转模式。flipCode=0垂直翻转(沿X轴翻转)，flipCode>0水平翻转(沿Y轴翻转)，flipCode<0水平垂直翻转(先沿X轴翻转，再沿Y轴翻转，等价于旋转180°)。

需要注意的是，**flipCode是int类型**，如果传入0.8会转换成0，沿X轴翻转，而不是沿Y轴翻转。因此，**建议flipCode的取值-1、0、1**。例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

    cv::Mat frame_dest_6;
    cv::flip(frame_src, frame_dest_6, -1);

    return 0;
}
```

### 图像旋转
除了图像翻转之外，图像的旋转也许更加常见的操作。OpenCV提供需要**先获得旋转矩阵，再根据旋转矩阵进行仿射变换来获得**，其函数定义如下：

第一步，根据旋转中心和旋转角度获取旋转矩阵；
```c
Mat getRotationMatrix2D(Point2f center, double angle, double scale);
```
其中，center参数表示旋转中心点，angle参数表示旋转角度，**角度为正则表示逆时针旋转，角度为负表示逆时针旋转**，scale参数表示缩放因子。

第二步，根据旋转矩阵进行仿射变换，实现图像旋转。
```c
void warpAffine(InputArray src, OutputArray dst,
                    InputArray M, Size dsize,
                    int flags = INTER_LINEAR,
                    int borderMode = BORDER_CONSTANT,
                    const Scalar& borderValue = Scalar());
```
其中，src参数表示输入图像，dst参数表示输出图像，M参数表示变换矩阵，dsize参数表示输出图像的尺寸，flags表示插值算法标识符，borderMode参数表示边界像素模式，boderValue参数表示边界取值。

更多地了解OpenCV旋转内容，以及插值算法的种类可以查看：[仿射变换warpAffine](https://blog.csdn.net/keith_bb/article/details/56331356)。

例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

    cv::Mat rot_mat = cv::getRotationMatrix2D(cv::Point2f(0, 0), 45, 1);
    cv::Mat frame_dest_7;
    cv::warpAffine(frame_src, frame_dest_7, rot_mat, frame_src.size());

    return 0;
}
```

### 图像滤波平滑
图像的滤波平滑处理很普遍，最主要的功能就是去噪声。在OpenCV中最经常使用的滤波平滑处理有四种：**均值滤波、中值滤波、高斯滤波和双边滤波**，OpenCV对它们也逐一进行了实现。其函数定义为：
```c
//均值滤波
void blur(InputArray src, OutputArray dst,
                    Size ksize, Point anchor = Point(-1,-1),
                    int borderType = BORDER_DEFAULT);
//src参数表示输入图像，dst参数表示输出图像，与输入图像有同样的size和type；ksize参数表示核的大小
//anchor参数表示核的中心，默认值为Point(-1,-1)，会转化为核的中点；borderType参数表示边缘点插值类型

//中值滤波
void medianBlur(InputArray src, OutputArray dst, int ksize);
//src参数表示输入图像，dst参数表示输出图像，与输入图像有同样的size和type；ksize参数表示正方形的边长. 边长必须是奇数而且大于1
//需要注意的是，当ksize是3或5的时候，图像的深度可以是CV_8U、CV_16U或CV_32F，对于更大的ksize，那么图像的深度只能是CV_8U

//高斯滤波
void GaussianBlur(InputArray src, OutputArray dst, Size ksize,
                    double sigmaX, double sigmaY = 0,
                    int borderType = BORDER_DEFAULT);
//src参数表示输入图像，dst参数表示输出图像，与输入图像有同样的size和type；ksize参数表示高斯核大小
//ksize.width和ksize.height可以不同，但是都必须是正整数而且是奇数，也可以是O，如果是0的话，它的值由sigmaX和sigmaY决定
//sigmaX参数表示X方向上的标准差，sigmaY参数表示Y方向上的标准差，如果Y方向为0，则被设置和X方向一样，如果X和Y方向都为0，则它们由ksize.width和ksize.height计算得到，建议全部都给定数值。
//borderType参数表示边缘点插值类型

//双边滤波器
void bilateralFilter( InputArray src, OutputArray dst, int d,
                    double sigmaColor, double sigmaSpace,
                    int borderType = BORDER_DEFAULT );
//src参数表示输入图像为单通道或3通道的8比特图像，dst参数表示输出图像，与输入图像有同样的size和type
//d参数表示每个像素领域的直径，如果为负数，则由sigmaSpace确定
//sigmaColor参数表示颜色空间的标准偏差，sigmaSpace表示坐标空间的标准偏差，borderType参数表示边缘点插值类型
```

如果想要深入查看几种滤波方式的内容，可以查看：[图像平滑处理](https://blog.csdn.net/chenjiazhou12/article/details/21346029)。

例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat frame_src = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

    cv::Mat frame_dest_8;
    cv::blur(frame_src, frame_dest_8, cv::Size(3, 3));
    cv::medianBlur(frame_src, frame_dest_8, 3);
    cv::GaussianBlur(frame_src, frame_dest_8, cv::Size(3, 3), 0, 0);
    cv::bilateralFilter(frame_src, frame_dest_8, 4, 2, 2);

    return 0;
}
```

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
