
#include <OpenNI.h>
#include <iostream>

openni::Device device;
openni::VideoStream videoStream;
openni::Status r;
openni::VideoFrameRef frame;

int main(){

  r = openni::OpenNI::initialize();
  if (r !=  openni::STATUS_OK){
      std::cout << "OpenNi2 init failed: " << openni::OpenNI::getExtendedError() << std::endl;
    return 1;
  }

  //MAX DEBUG:
  openni::OpenNI::setLogConsoleOutput (TRUE);
  
  openni::OpenNI::setLogMinSeverity(0);

  r = device.open("in.oni");
//  r = device.open( openni::ANY_DEVICE ); 
  if (r != openni::STATUS_OK){
    if(openni::OpenNI::getExtendedError() != ""){
      std::cout << "OpenNi2 open failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    }else
      std::cout << "OpenNi2 open failed, for an unknown reason. (maybe a empty file?)" << std::endl;
    return 1;
  }

  std::cout << "Quick debug:" << std::endl;
  std::cout << "Has IR: " << device.hasSensor(openni::SENSOR_IR) << std::endl;
  std::cout << "Has COLOR: " << device.hasSensor(openni::SENSOR_COLOR) << std::endl;
  std::cout << "Has DEPTH: " << device.hasSensor(openni::SENSOR_DEPTH) << std::endl;


  r = device.getPlaybackControl()->setSpeed(-1);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 set playback speed failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return 1;
  }

//  r = device.getPlaybackControl()->setRepeatEnabled(TRUE); //Only good for debugging
  r = device.getPlaybackControl()->setRepeatEnabled(FALSE);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 set playback repeat failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return 1;
  }

  r = videoStream.create(device, openni::SENSOR_COLOR);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 create stream failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return 1;
  }

  r = videoStream.start();
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 start stream  failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return 1;
  }

//  device.getPlaybackControl()->seek(videoStream,32);
  
  videoStream.readFrame(&frame);
  
  std::cout << "A frame was read! ITS NOT BROKED." << std::endl;

}
