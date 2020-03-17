使用C++开发图像处理算法时，最基础的就是利用OpenCV完成图像文件的输入、输出以及自动内存管理(重点)。所以，只要需要掌握一些简单的OpenCV的操作即可。本博文就对这些基础内容进行讲解。

**本文实例源码github地址**：[https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200317](https://github.com/yngzMiao/yngzmiao-blogs/tree/master/2020Q1/20200317)。

<br/>

## 图像操作
### 图像读取
OpenCV支持**bmp、jpg、png、tiff**等常用图像格式的解析，所用函数为`imread`。imread函数原型如下：
```c
Mat imread( const String& filename, int flags = IMREAD_COLOR );
```
其中，filename参数为图像文件的路径，可以是相对路径，也可以是绝对路径；flags参数为图像文件解析的方式，支持的方式有如下：
* **IMREAD_UNCHANGED**：不对图像文件进行任何转换，直接读取；
* **IMREAD_GRAYSCALE**：将任何图像均转换为灰度图像(单通道)进行读取；
* **IMREAD_COLOR**：将任何图像均转为RGB彩色图像(三通道)进行读取；
* IMREAD_ANYDEPTH：如果不设置这个参数，16/32位图像将会自动转为8位图像；
* IMREAD_ANYCOLOR：将按照图像文件设定的颜色格式进行图像读取；
* IMREAD_LOAD_GDAL：调用gdal库进行图像文件读取(可以简单地理解为读取TIFF图像文件)。

例如：
```c
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

    return 0;
}
```

### 图像显示
OpenCV的图像显示函数为`imshow`，函数原型如下：
```c
void imshow(const String& winname, InputArray mat);
```
其中，winname参数表示显示图像窗口的名称(任意字符)，mat参数表示需要显示的图像。对于这个函数，需要注意的是(特别是新手)，imshow函数**只支持8位灰度图像、8位彩色图像和32位灰度图像(像素值范围0-1)**，具体原因大家可以自行百度`显示器灰度等级`。

同时还有，除了使用该函数之外，还需要使用`waitKey`函数使图像界面一直显示。例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat img = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
    cv::imshow("hello", img);
    cv::waitKey();

    return 0;
}
```

### 图像保存
`imwrite`函数用来保存图片，函数原型如下：
```c
bool imwrite( const String& filename, InputArray img,
              const std::vector<int>& params = std::vector<int>());
```
其中，filename参数表示需要写入的文件名，必须要加上后缀，比如“123.png”；img参数表示Mat类型的图像数据；params参数表示为特定格式保存的参数编码，它有一个默认值std::vector<int>()，所以一般情况下不用写。

同时还需要注意的是，并不是所有格式的Mat型数据都能被保存为图片，目前OpenCV主要**只支持单通道和3通道的图像，并且此时要求其深度为8bit和16bit无符号**。所以，其他一些数据类型是不支持的，比如说float型等。如果Mat类型数据的深度和通道数不满足上面的要求，则需要使用`convertTo()`函数和`cvtColor()`函数来进行转换。

例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat img = cv::imread("test.png", cv::IMREAD_GRAYSCALE);
    cv::imwrite("test_copy.png", img);

    return 0;
}
```

### 图像遍历的几种方式
#### OpenCV的at成员函数
OpenCV提供了便利的访问图像数据的接口，`at`函数原型：
```c
template<typename _Tp> _Tp& at(int row, int col);
```
其中，参数row为行号，参数col为列号；模板参数_Tp常用类型如下：

| 图像类型 | _Tp参数 |
| :---: | :---: |
| 单通道灰度 | unsigned char |
| 三通道彩色(8位) | cv::Vec3b |
| 16位灰度 | unsigned short |
| 32位浮点型 | float |
| 双通道32位 | cv::Vec2f |

例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat img = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

    for (int r = 0; r < img.rows; ++r) {
        for (int c = 0; c < img.cols; ++c) {
            int data = img.at<unsigned char>(r, c);
            std::cout << data << std::endl;
        }
    }

  return 0;
}
```

#### 数据缓存区指针
使用`data`属性获取数据缓存区指针，利用指针直接读取数据缓存区的内容。例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat img = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

    unsigned char * img_data = reinterpret_cast<unsigned char *>(img.data);
    for(int r = 0; r < img.rows; ++r) {
        for(int c = 0; c < img.cols; ++c) {
            std::cout << static_cast<float>(*img_data) << std::endl;
            ++img_data;
        }
    }

    return 0;
}
```

### 行首指针
除了data属性可以获取数据缓存区的指针，OpenCV还提供了`ptr`接口来直接获取每一行的行首指针。例如：
```c
#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char const *argv[])
{
    cv::Mat img = cv::imread("test.png", cv::IMREAD_GRAYSCALE);

    for(int r = 0; r < img.rows; ++r) {
        unsigned char * rdata = img.ptr<unsigned char>(r);
        for(int c = 0; c < img.cols; ++c) {
            std::cout << static_cast<float>(*rdata) << std::endl;
            ++rdata;
        }
    }

    return 0;
}
```

<br/>

## 视频操作
### 视频读取
OpenCV中通过`VideoCaptrue`类对视频进行读取操作以及调用摄像头，该类的API如下：
```c
//功能：创建一个VideoCapture类的实例，如果传入对应的参数，可以直接打开视频文件或者要调用的摄像头。
//参数: filename – 打开的视频文件名。
//device – 打开的视频捕获设备id ，如果只有一个摄像头可以填0，表示打开默认的摄像头。
VideoCapture::VideoCapture();  
VideoCapture::VideoCapture(const string& filename);  
VideoCapture::VideoCapture(int device);  

//功能：打开一个视频文件或者打开一个捕获视频的设备(也就是摄像头)
bool VideoCapture::open(const string& filename);  
bool VideoCapture::open(int device);

//功能：判断视频读取或者摄像头调用是否成功，成功则返回true
bool VideoCapture::isOpened();

//功能：关闭视频文件或者摄像头
void VideoCapture::release();

//功能：读取视频内容
bool VideoCapture::grab();  //若没有视频帧被捕获，返回false
bool VideoCapture::retrieve(Mat& image, int channel=0);  //若没有视频帧被捕获，返回false
VideoCapture& VideoCapture::operator>>(Mat& image);  //若没有视频帧被捕获，返回空，即cv::Mat.empty()返回true
bool VideoCapture::read(Mat& image);  //若没有视频帧被捕获，返回false

//功能：获得视频有诸多属性，比如：帧率、总帧数、尺寸、格式等
//如果查询的视频属性是VideoCapture类不支持的，将会返回0
double VideoCapture::get(int propId);  

//功能：设置VideoCapture类的属性，设置成功返回ture,失败返回false
//参数：第一个是属性ID，第二个是该属性要设置的值
bool VideoCapture::set(int propertyId, double value);
```
其中，视频的属性主要包括：

| 属性值 | 属性含义 |
| :---: | :---: |
| CV_CAP_PROP_POS_MSEC | 以毫秒计算的当前的位置 |
| **CV_CAP_PROP_POS_FRAMES** | 以帧计算当前的位置 |
| CV_CAP_PROP_POS_AVI_RATIO | 视频的相对位置，0表示视频开始，1表示视频结束 |
| **CV_CAP_PROP_FRAME_WIDTH** | 帧宽度 |
| **CV_CAP_PROP_FRAME_HEIGHT** | 帧高度 |
| **CV_CAP_PROP_FPS** | 帧率 |
| CV_CAP_PROP_FOURCC | 4字符编码方式 |
| **CV_CAP_PROP_FRAME_COUNT** | 视频帧数 |
| CV_CAP_PROP_FORMAT | 视频格式  |
| CV_CAP_PROP_MODE |  |
| CV_CAP_PROP_BRIGHTNESS | 亮度 |
| CV_CAP_PROP_CONTRAST | 对比度 |
| CV_CAP_PROP_SATURATION | 饱和度 |
| CV_CAP_PROP_HUE | 色调 |
| CV_CAP_PROP_GAIN | 增益 |
| CV_CAP_PROP_EXPOSURE | 曝光 |
| CV_CAP_PROP_CONVERT_RGB | 图像是否应转换为RGB的标志 |
| CV_CAP_PROP_WHITE_BALANCE | 白平衡 |
| CV_CAP_PROP_RECTIFICATION |  |

例如：
```c
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

    cv::Mat frame;
    while(1) {
        cap >> frame;
        if (frame.empty())
            break;
        cv::imshow("hello", frame);
        cv::waitKey(50);
    }
    cap.release();

    return 0;
}
```

在上面的API中还将到`set`方法，可以指定属性的值。比如，**想要跳转到视频的中间的某一帧，就可以通过设置CV_CAP_PROP_POS_FRAMES属性来实现**，即：
```c
cap.set(CV_CAP_PROP_POS_FRAMES, 20);        //跳转到第20帧
```

### 视频保存
OpenCV中通过`VideoWriter`类对视频进行读取操作以及调用摄像头，该类的API与VideoCapture类似，该类的主要API除了构造函数外，提供了open、IsOpen、release、write和重载操作符<<。

其主要的API如下：
```c
VideoWriter::VideoWriter(const string& filename, int fourcc,
                         double fps, Size frameSize, bool isColor=true);
bool VideoWriter::open(const string& filename, int fourcc,
                       double fps, Size frameSize, bool isColor=true);
```
其中，filename参数表示文件名，fourcc参数表示编码格式，fps参数表示视频帧率，frameSize表示视频的尺寸大小。

OpenCV支持的编码格式包括：

| 编码格式 | 编码格式含义 |
| :---: | :---: |
| CV_FOURCC('P','I','M','1') | MPEG-1 |
| CV_FOURCC('M','J','P','G') | motion-jpeg |
| CV_FOURCC('M', 'P', '4', '2') | MPEG-4.2 |
| CV_FOURCC('D', 'I', 'V', '3') | MPEG-4.3 |
| CV_FOURCC('D', 'I', 'V', 'X') | MPEG-4 |
| CV_FOURCC('U', '2', '6', '3') | H263 |
| CV_FOURCC('I', '2', '6', '3') | H263I |
| CV_FOURCC('F', 'L', 'V', '1') | FLV1 |

值得注意的是：**OpenCV里对视频的编码解码等支持并不是很良好，所以不要希望用这个类去实现摄像头图像的获取与转码**，有兴趣的可以参考FFmpeg库。

例如：
```c
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
```

使用VideoWriter，建议生成`.avi`后缀的视频。

<br/>

## 相关阅读
* [opencv 遍历mat的三种方法](https://blog.csdn.net/koibiki/article/details/85954121)
* [VideoWriter生成视频流highgui](https://blog.csdn.net/yang_xian521/article/details/7440190)

<center><img src="https://img-blog.csdnimg.cn/20190309211249199.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly95bmd6bWlhby5ibG9nLmNzZG4ubmV0,size_16,color_FFFFFF,t_70">
