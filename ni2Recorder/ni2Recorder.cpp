#include <openni2/OpenNI.h>
#include <opencv2/opencv.hpp>
#include <iostream>

#include <boost/filesystem.hpp>

#include "props.hpp"

bool imageReg = true;
bool imageSync = true;
bool OpenNIDebug = false;
bool recProps = true;
bool irAsColor = false;
bool enableIR = true;
bool enableDepth = true;
bool enableColor = true;
void parseArgs(int argc,char *argv[]){
  for(int i = 1; i < argc; i++){
    if (std::string(argv[i]) == "-noReg" || (std::string(argv[i]) == "-noreg" )) {
        imageReg = false;
        break;
    }else if (std::string(argv[i]) == "-reg" || std::string(argv[i]) == "-Reg"){
        imageReg = true;
        break;
    }else if (std::string(argv[i]) == "-sync"){
        imageSync = true;
    }else if (std::string(argv[i]) == "-nosync"){
        imageSync = false;
    }else if (std::string(argv[i]) == "-d"){
        OpenNIDebug = true;
    }else if (std::string(argv[i]) == "-noD"){
        OpenNIDebug = false;
    }else if (std::string(argv[i]) == "-irAsColor"){
        irAsColor = true;
        enableColor = true;
    }else if (std::string(argv[i]) == "-noirAsColor"){
        irAsColor = false;
    }else if (std::string(argv[i]) == "-ir"){
        enableIR = true;
    }else if (std::string(argv[i]) == "-noir"){
        enableIR = false; 
    }else if (std::string(argv[i]) == "-color"){
        enableColor = true;
    }else if (std::string(argv[i]) == "-nocolor"){
        enableColor = false; 
    }else if (std::string(argv[i]) == "-depth"){
        enableDepth = true;
    }else if (std::string(argv[i]) == "-nodepth"){
        enableDepth = false; 
    }else{
        std::cout << " Unknown arg: " << argv[i] << std::endl;
    }
  }
}    


const char* getFormatName(openni::PixelFormat format)
{
	switch (format)
	{
	case openni::PIXEL_FORMAT_DEPTH_1_MM:
		return "1 mm";
	case openni::PIXEL_FORMAT_DEPTH_100_UM:
		return "100 um";
	case openni::PIXEL_FORMAT_SHIFT_9_2:
		return "Shifts 9.2";
	case openni::PIXEL_FORMAT_SHIFT_9_3:
		return "Shifts 9.3";
	case openni::PIXEL_FORMAT_RGB888:
		return "RGB 888";
	case openni::PIXEL_FORMAT_YUV422:
		return "YUV 422";
	case openni::PIXEL_FORMAT_YUYV:
		return "YUYV";
	case openni::PIXEL_FORMAT_GRAY8:
		return "Grayscale 8-bit";
	case openni::PIXEL_FORMAT_GRAY16:
		return "Grayscale 16-bit";
	case openni::PIXEL_FORMAT_JPEG:
		return "JPEG";
	default:
		return "Unknown";
	}
}

int main(int argc, char *argv[])
{
  std::cout << "we" << std::endl;
  parseArgs(argc,argv);

  openni::OpenNI::initialize();


  //MAX DEBUG
  if(OpenNIDebug){
    openni::OpenNI::setLogConsoleOutput (TRUE);
    openni::OpenNI::setLogMinSeverity(0);
  }
  
  openni::Status r;

  openni::Device device;
  r = device.open(openni::ANY_DEVICE);
  if (r != openni::STATUS_OK){
    if(openni::OpenNI::getExtendedError() != ""){
      std::cout << "OpenNi2 open failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    }else
      std::cout << "OpenNi2 open failed, for an unknown reason. (maybe a empty file?)" << std::endl;
//    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  openni::VideoStream image,depth,ir;

  if(enableColor){
    if(irAsColor){
      r = image.create(device, openni::SENSOR_IR);
    }else{
      r = image.create(device, openni::SENSOR_COLOR);
    }
    if (r != openni::STATUS_OK){
      std::cout << "OpenNi2 create stream failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  //    return XN_STATUS_DEVICE_NOT_CONNECTED;
    }
  }
  
  if(enableDepth){  
    r = depth.create(device, openni::SENSOR_DEPTH);
    if (r != openni::STATUS_OK){
      std::cout << "OpenNi2 create stream failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  //    return XN_STATUS_DEVICE_NOT_CONNECTED;
    }
  }

  if(enableIR){  
    r = ir.create(device, openni::SENSOR_IR);
    if (r != openni::STATUS_OK){
      std::cout << "OpenNi2 create stream failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  //    return XN_STATUS_DEVICE_NOT_CONNECTED;
    }
  }
   
  if(imageReg){
    r = device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR);
    if (r != openni::STATUS_OK)
      std::cout << "OpenNi2 Image Registration failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  }
  if(imageSync){
    r = device.setDepthColorSyncEnabled(true);
    if (r != openni::STATUS_OK)
      std::cout << "OpenNi2 Depth Color Sync  failed: " << r << " : " <<  openni::OpenNI::getExtendedError() << std::endl;
  }

  if(enableColor){
    r = image.start();
    if (r != openni::STATUS_OK){
      std::cout << "OpenNi2 start stream  failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  //    return XN_STATUS_DEVICE_NOT_CONNECTED;
    }
  }

  if(enableDepth){
    r = depth.start();
    if (r != openni::STATUS_OK){
      std::cout << "OpenNi2 start stream  failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  //    return XN_STATUS_DEVICE_NOT_CONNECTED;
    }
  }

  if(enableIR){
    r = ir.start();
    if (r != openni::STATUS_OK){
      std::cout << "OpenNi2 start stream  failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
  //    return XN_STATUS_DEVICE_NOT_CONNECTED;
    }
  }



  openni::VideoFrameRef dFrame, iFrame, irFrame;
  cv::Mat iMat, dMat, irMat;
  int cnt=0;
  
  if(enableColor)
    boost::filesystem::create_directory("colourframes");
  if(enableDepth)
    boost::filesystem::create_directory("depthframes");
  if(enableIR)
    boost::filesystem::create_directory("irframes");

  std::cout << "Starting." << std::endl;

  while(true){ 

    if(enableDepth) 
      depth.readFrame(&dFrame);
    if(enableIR)
      ir.readFrame(&irFrame);
    if(enableColor)
      image.readFrame(&iFrame);

    if(enableColor){
      int iH = iFrame.getHeight();
      int iW = iFrame.getWidth();
//    std::cout << getFormatName(image.getVideoMode().getPixelFormat()) << std::endl;
      if(irAsColor){
        iMat = cv::Mat(iH, iW, CV_16U, (openni::Grayscale16Pixel*)  iFrame.getData());
      }else{
        iMat = cv::Mat(iH, iW, CV_8UC3, (openni::RGB888Pixel*) iFrame.getData());
        cv::cvtColor(iMat, iMat, CV_RGB2BGR);
      }
      imwrite("./colourframes/Image" + std::to_string(cnt) + ".jpg",iMat);
    }

    if(enableDepth){
      int dH = dFrame.getHeight();
      int dW = dFrame.getWidth();
      
      dMat = cv::Mat(dH, dW, CV_16U, (openni::DepthPixel*) dFrame.getData());
      imwrite("./depthframes/Image" + std::to_string(cnt) + ".png",dMat); 
    }

    if(enableIR){
      int irH = irFrame.getHeight();
      int irW = irFrame.getWidth();
      
      irMat = cv::Mat(irH, irW, CV_16U,  (openni::Grayscale16Pixel*) irFrame.getData());
      imwrite("./irframes/Image" + std::to_string(cnt) + ".png", irMat); 
    } 

    if(! (cnt%50))
      std::cout << "Frame: " << cnt << std::endl;

    cnt++;
  }

  openni::OpenNI::shutdown();
  return 0;
}


