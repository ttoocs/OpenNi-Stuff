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
#include <opencv2/opencv.hpp>

#include "Depth.h"

//#define SUPPORTED_X_RES 400
//#define SUPPORTED_Y_RES 300
//#define SUPPORTED_FPS 30
#define MAX_DEPTH_VALUE 15000

#define SUPPORTED_X_RES videoStream.getVideoMode().getResolutionX()
#define SUPPORTED_Y_RES videoStream.getVideoMode().getResolutionY()
//#define SUPPORTED_FPS videoStream.getVideoMode().getFps()
#define SUPPORTED_FPS m_lastFPS

Depth::Depth(const XnChar* strName) :
  m_bGenerating(FALSE),
  m_bDataAvailable(FALSE),
  m_nFrameID(0),
  m_nTimestamp(0),
  m_hScheduler(NULL),
  m_lastFPS(30)
{
  xnOSStrCopy(m_strName, strName, sizeof(m_strName)); //Copy in the strName
}
Depth::~Depth()
{
  openni::OpenNI::shutdown();
}

XnStatus Depth::Init()
{
  std::cout << "OpenNI2 Depth: Init filename: " << m_strName << std::endl;

  openni::Status r;

  r = openni::OpenNI::initialize();
  if (r !=  openni::STATUS_OK){
      std::cout << "OpenNi2 init failed: " << openni::OpenNI::getExtendedError() << std::endl;
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

#ifdef ANY_STREAM
  r = device.open(openni::ANY_DEVICE);
#else
  r = device.open(m_strName);
#endif
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

  r = videoStream.create(device, openni::SENSOR_DEPTH);
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 create stream failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    //return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  r = videoStream.start();
  if (r != openni::STATUS_OK){
    std::cout << "OpenNi2 start stream  failed: " <<  openni::OpenNI::getExtendedError() << std::endl;
    //return XN_STATUS_DEVICE_NOT_CONNECTED;
  }

  //m_lastFrame = device.getPlaybackControl()->getNumberOfFrames(videoStream);
  
  return (XN_STATUS_OK);
}

XnBool Depth::IsCapabilitySupported( const XnChar* strCapabilityName )
{
  return FALSE;
}

XnStatus Depth::StartGenerating()
{
  XnStatus nRetVal = XN_STATUS_OK;

  m_bGenerating = TRUE;

  videoStream.addNewFrameListener(this);
  device.getPlaybackControl()->setSpeed(1);

  /* Origonal code
  // start scheduler thread
  nRetVal = xnOSCreateThread(SchedulerThread, this, &m_hScheduler);
  if (nRetVal != XN_STATUS_OK)
  {
    m_bGenerating = FALSE;
    return (nRetVal);
  }
  m_generatingEvent.Raise();
// */
  return (XN_STATUS_OK);
}

XnBool Depth::IsGenerating()
{
  return m_bGenerating;
}

void Depth::StopGenerating()
{
  m_bGenerating = FALSE;
  
  
  videoStream.removeNewFrameListener(this);
  device.getPlaybackControl()->setSpeed(-1);

//  /* Origonal code
  // wait for thread to exit
//  xnOSWaitForThreadExit(m_hScheduler, 100);

//  */
  m_generatingEvent.Raise();
}

XnStatus Depth::RegisterToGenerationRunningChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
  return m_generatingEvent.Register(handler, pCookie, hCallback);
}

void Depth::UnregisterFromGenerationRunningChange( XnCallbackHandle hCallback )
{
  m_generatingEvent.Unregister(hCallback);
}

XnStatus Depth::RegisterToNewDataAvailable( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
  return m_dataAvailableEvent.Register(handler, pCookie, hCallback);
}

void Depth::UnregisterFromNewDataAvailable( XnCallbackHandle hCallback )
{
  m_dataAvailableEvent.Unregister(hCallback);
}

XnBool Depth::IsNewDataAvailable( XnUInt64& nTimestamp )
{
  // return next timestamp
  nTimestamp = 1000000 / SUPPORTED_FPS;
  return m_bDataAvailable;
}

XnStatus Depth::UpdateData()
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

  m_pDepthMap = (XnDepthPixel *) videoFrame.getData();

  m_nFrameID = videoFrame.getFrameIndex();
  m_nTimestamp = videoFrame.getTimestamp();
  // mark that data is old
  m_bDataAvailable = FALSE;
  
  return (XN_STATUS_OK);
}

XnUInt32 Depth::GetDataSize()
{
  return (SUPPORTED_X_RES * SUPPORTED_Y_RES * sizeof(XnDepthPixel));
}

XnUInt64 Depth::GetTimestamp()
{
  return m_nTimestamp;
}

XnUInt32 Depth::GetFrameID()
{
  return m_nFrameID;
}


XnUInt32 Depth::GetSupportedMapOutputModesCount()
{
  // we only support a single mode
  return 1;
}

XnStatus Depth::GetSupportedMapOutputModes( XnMapOutputMode aModes[], XnUInt32& nCount )
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

XnStatus Depth::SetMapOutputMode( const XnMapOutputMode& Mode )
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

XnStatus Depth::GetMapOutputMode( XnMapOutputMode& Mode )
{
  Mode.nXRes = SUPPORTED_X_RES;
  Mode.nYRes = SUPPORTED_Y_RES;
  Mode.nFPS = SUPPORTED_FPS;

  return (XN_STATUS_OK);
}

XnStatus Depth::RegisterToMapOutputModeChange( XnModuleStateChangedHandler /*handler*/, void* /*pCookie*/, XnCallbackHandle& hCallback )
{
  // no need. we only allow one mode
  hCallback = this;
  return XN_STATUS_OK;
}

void Depth::UnregisterFromMapOutputModeChange( XnCallbackHandle /*hCallback*/ )
{
  // do nothing (we didn't really register)
}

XnDepthPixel* Depth::GetDepthMap()
{
  if(*((int*)(&videoFrame)) != 0) //Check if videoframe is good. (dirty hack, i'm so sorry)
    return (XnDepthPixel*) videoFrame.getData();
  return NULL;
}

XnDepthPixel Depth::GetDeviceMaxDepth()
{
  return MAX_DEPTH_VALUE;
}

void Depth::GetFieldOfView( XnFieldOfView& FOV )
{
  // some numbers
  FOV.fHFOV = 1.35;
  FOV.fVFOV = 1.35;
}

XnStatus Depth::RegisterToFieldOfViewChange( XnModuleStateChangedHandler /*handler*/, void* /*pCookie*/, XnCallbackHandle& hCallback )
{
  // no need. it never changes
  hCallback = this;
  return XN_STATUS_OK;
}

void Depth::UnregisterFromFieldOfViewChange( XnCallbackHandle /*hCallback*/ )
{
  // do nothing (we didn't really register)
}

XN_THREAD_PROC Depth::SchedulerThread( void* pCookie )
{
  Depth* pThis = (Depth*)pCookie;

  while (pThis->m_bGenerating)
  {
    // wait 33 ms (to produce 30 FPS)
//    xnOSSleep(1000000/SUPPORTED_FPS/1000);
    xnOSSleep(1000000/pThis->m_lastFPS/1000);


    pThis->OnNewFrame();
  }

  XN_THREAD_PROC_RETURN(0);
}

void Depth::OnNewFrame()
{
  m_bDataAvailable = TRUE;
  m_dataAvailableEvent.Raise();
}
void Depth::onNewFrame(openni::VideoStream& vs)
{
  videoStream.readFrame(&videoFrame);
  OnNewFrame();
}
