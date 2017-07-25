#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>

#define maxFrames 2
class CVIN {
  
  public:
    CVIN(const char* indir);

    int getNumFrames();
    int getCurFrames() { return curFrame ;}

    void getNextFrames(cv::Mat &depth, cv::Mat &color);
  
  private:
    int curFrame;
    int numFrames;
    std::string dirPath;
    
};

