/*****************************************************************************
*                                                                            *
*  ttoocs                                                                    *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <iostream>

#include "Image.h"

//#define SUPPORTED_X_RES 400
//#define SUPPORTED_Y_RES 300
//#define SUPPORTED_FPS 30

#define SUPPORTED_X_RES videoStream.getVideoMode().getResolutionX()
#define SUPPORTED_Y_RES videoStream.getVideoMode().getResolutionY()
//#define SUPPORTED_FPS videoStream.getVideoMode().getFps()
#define SUPPORTED_FPS m_lastFPS

Image::Image(const XnChar* strName) :
  m_bGenerating(FALSE),
  m_bDataAvailable(FALSE),
  m_nFrameID(0),
  m_nTimestamp(0),
  m_hScheduler(NULL),
  m_lastFPS(30)
{
  xnOSStrCopy(m_strName, strName, sizeof(m_strName)); //Copy in the strName
}
Image::~Image()
{
  openni::OpenNI::shutdown();
}

XnStatus Image::Init()
{
  std::cout << "OpenNI2 Image: Init filename: " << m_strName << std::endl;

  openni::Status r;

  r = openni::OpenNI::initialize();
  if (r !=  openni::STATUS_OK){
      std::cout << "OpenNi2 init failed: " << openni::OpenNI::getExtendedError() << std::endl;
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  r = device.open(m_strName);
  if (r != openni::STATUS_OK){
    if(openni::OpenNI::getExtendedError() != ""){
      std::cout << "OpenNi2 open failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    }else
      std::cout << "OpenNi2 open failed, for an unknown reason. (maybe a empty file?)" << std::endl;
  //  throw ffs; //Good for debugging this.
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  r = device.getPlaybackControl()->setSpeed(-1);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 set playback speed failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

//  r = device.getPlaybackControl()->setRepeatEnabled(TRUE); //Only good for debugging
  r = device.getPlaybackControl()->setRepeatEnabled(FALSE);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 set playback repeat failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  r = videoStream.create(device, openni::SENSOR_COLOR);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 create stream failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  r = videoStream.start();
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 start stream  failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  m_lastFrame = device.getPlaybackControl()->getNumberOfFrames(videoStream);
  
  return (XN_STATUS_OK);
}

XnBool Image::IsCapabilitySupported( const XnChar* strCapabilityName )
{
  return FALSE;
}

XnStatus Image::StartGenerating()
{
  XnStatus nRetVal = XN_STATUS_OK;

  m_bGenerating = TRUE;

  videoStream.addNewFrameListener(this);
  device.getPlaybackControl()->setSpeed(1);

  return (XN_STATUS_OK);
}

XnBool Image::IsGenerating()
{
  return m_bGenerating;
}

void Image::StopGenerating()
{
  m_bGenerating = FALSE;
  
  videoStream.removeNewFrameListener(this);
  device.getPlaybackControl()->setSpeed(-1);

  m_generatingEvent.Raise();
}

XnStatus Image::RegisterToGenerationRunningChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
  return m_generatingEvent.Register(handler, pCookie, hCallback);
}

void Image::UnregisterFromGenerationRunningChange( XnCallbackHandle hCallback )
{
  m_generatingEvent.Unregister(hCallback);
}

XnStatus Image::RegisterToNewDataAvailable( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
  return m_dataAvailableEvent.Register(handler, pCookie, hCallback);
}

void Image::UnregisterFromNewDataAvailable( XnCallbackHandle hCallback )
{
  m_dataAvailableEvent.Unregister(hCallback);
}

XnBool Image::IsNewDataAvailable( XnUInt64& nTimestamp )
{
  // return next timestamp
  nTimestamp = NULL;
  return m_bDataAvailable;
}

XnStatus Image::UpdateData()
{

  if(m_nFrameID >=  m_lastFrame -1 ){
    std::cout << "Reached end of recording! FrameID: " << m_nFrameID << " numFrames: " << m_lastFrame  << std::endl;
    std::cout << "TODO: Handle this properly." << std::endl;
    m_bDataAvailable = FALSE;
    m_bGenerating = FALSE;
    return (XN_STATUS_OK);
  }
  //If using oldcode:
  //videoStream.readFrame(&videoFrame);

  m_pImageMap = (XnImagePixel *) videoFrame.getData();

  m_nFrameID = videoFrame.getFrameIndex();
  m_nTimestamp = videoFrame.getTimestamp();
  // mark that data is old
  m_bDataAvailable = FALSE;
  
  return (XN_STATUS_OK);
}

XnUInt32 Image::GetDataSize()
{
  return (SUPPORTED_X_RES * SUPPORTED_Y_RES * sizeof(XnImagePixel));
}

XnUInt64 Image::GetTimestamp()
{
  return m_nTimestamp;
}

XnUInt32 Image::GetFrameID()
{
  return m_nFrameID;
}


XnUInt32 Image::GetSupportedMapOutputModesCount()
{
  // we only support a single mode
  return 1;
}

XnStatus Image::GetSupportedMapOutputModes( XnMapOutputMode aModes[], XnUInt32& nCount )
{
  if (nCount < 1)
  {
    return XN_STATUS_OUTPUT_BUFFER_OVERFLOW;
  }

  aModes[0].nXRes = SUPPORTED_X_RES;
  aModes[0].nYRes = SUPPORTED_Y_RES;
  aModes[0].nFPS = SUPPORTED_FPS;

  return (XN_STATUS_OK);
}

XnStatus Image::SetMapOutputMode( const XnMapOutputMode& Mode )
{
  // make sure this is our supported mode
  if (Mode.nXRes != SUPPORTED_X_RES ||
    Mode.nYRes != SUPPORTED_Y_RES ||
    Mode.nFPS != SUPPORTED_FPS)
  {
    return (XN_STATUS_BAD_PARAM);
  }

  return (XN_STATUS_OK);
}

XnStatus Image::GetMapOutputMode( XnMapOutputMode& Mode )
{
  Mode.nXRes = SUPPORTED_X_RES;
  Mode.nYRes = SUPPORTED_Y_RES;
  Mode.nFPS = SUPPORTED_FPS;

  return (XN_STATUS_OK);
}

XnStatus Image::RegisterToMapOutputModeChange( XnModuleStateChangedHandler /*handler*/, void* /*pCookie*/, XnCallbackHandle& hCallback )
{
  // no need. we only allow one mode
  hCallback = this;
  return XN_STATUS_OK;
}

void Image::UnregisterFromMapOutputModeChange( XnCallbackHandle /*hCallback*/ )
{
  // do nothing (we didn't really register)
}

XnUInt8* Image::GetImageMap()
{
  if(*((int*)(&videoFrame)) != 0) //Check if videoframe is good. (dirty hack, i'm so sorry)
    return (XnUInt8*) videoFrame.getData();
  return NULL;
}


void Image::GetFieldOfView( XnFieldOfView& FOV )
{
  // some numbers
  FOV.fHFOV = 1.35;
  FOV.fVFOV = 1.35;
}

XnStatus Image::RegisterToFieldOfViewChange( XnModuleStateChangedHandler /*handler*/, void* /*pCookie*/, XnCallbackHandle& hCallback )
{
  // no need. it never changes
  hCallback = this;
  return XN_STATUS_OK;
}

void Image::UnregisterFromFieldOfViewChange( XnCallbackHandle /*hCallback*/ )
{
  // do nothing (we didn't really register)
}

XN_THREAD_PROC Image::SchedulerThread( void* pCookie )
{
  Image* pThis = (Image*)pCookie;

  while (pThis->m_bGenerating)
  {
    // wait 33 ms (to produce 30 FPS)
//    xnOSSleep(1000000/SUPPORTED_FPS/1000);
    xnOSSleep(1000000/pThis->m_lastFPS/1000);


    pThis->OnNewFrame();
  }

  XN_THREAD_PROC_RETURN(0);
}

void Image::OnNewFrame()
{
  m_bDataAvailable = TRUE;
  m_dataAvailableEvent.Raise();
}
void Image::onNewFrame(openni::VideoStream& vs)
{
  videoStream.readFrame(&videoFrame);
  OnNewFrame();
}


//Misc img stuffs
XnBool Image::IsPixelFormatSupported(XnPixelFormat Format){
  return (Format == XN_PIXEL_FORMAT_RGB24);
}

XnStatus Image::SetPixelFormat(XnPixelFormat Format){
  if(Format == XN_PIXEL_FORMAT_RGB24)
    return (XN_STATUS_OK);
  return  (XN_STATUS_BAD_PARAM);
}

XnPixelFormat Image::GetPixelFormat(){
  return XN_PIXEL_FORMAT_RGB24;
}


