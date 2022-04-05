#pragma once
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using cv::Mat;
using cv::pyrDown;
using cv::pyrUp;
using cv::imshow;
using cv::Size;
using cv::Mat;
using cv::imread;
using cv::waitKey;

using std::cout;
using std::endl;

class GaussianPyramid {

public:
    GaussianPyramid() {}
    GaussianPyramid(Mat img, int _levels) : original_image(img), levels(_levels) {}
    void buildPyramid(Mat);
    void reconPyramid();
    void printPyramid();
    void showEachStage();
    void showSingleStage(Mat);
    Mat& getOriginalImg() {return original_image;}

    std::vector<Mat> pyramid;
    const char* window_name = "Pyramids Demo";

private:
    Mat original_image;
    int levels = 0;
    

};