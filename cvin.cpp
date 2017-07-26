#include <cvin.h>

#include <boost/filesystem.hpp>

#define depthDir "depthframes/"
#define imgDir "colourframes/"

#define prefix "Image"
#define dsuffix ".png"
#define isuffix ".jpg"


CVIN::CVIN(const char* indir){
  dirPath = std::string(indir);
  curFrame = 0;
  numFrames =  (int)std::count_if(boost::filesystem::directory_iterator(boost::filesystem::path(dirPath + depthDir)),boost::filesystem::directory_iterator(),  [](const boost::filesystem::directory_entry& e) {return e.path().extension() == dsuffix;});

}

int CVIN::getNumFrames(){
  return numFrames;
}

void CVIN::getNextFrames(cv::Mat &depth, cv::Mat &color){
  cv::Mat adepth, acol;
  
  std::string strDepth = dirPath + depthDir + prefix+ std::to_string(curFrame) + dsuffix;
  std::string strImg = dirPath + imgDir +  prefix + std::to_string(curFrame) + isuffix;

  //std::cout << "depth path: " << strDepth << std::endl;

  adepth = cv::imread(strDepth, CV_LOAD_IMAGE_ANYDEPTH);
  acol = cv::imread(strImg, CV_LOAD_IMAGE_COLOR);

  //std::cout << "TEST: " << adepth.rows << std::endl;

  depth = adepth;
  color = acol;

  curFrame++;
}

