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

#define SUPPORTED_X_RES m_DFrame.cols
#define SUPPORTED_Y_RES m_DFrame.rows
#define SUPPORTED_FPS   m_lastFPS
#define MAX_DEPTH_VALUE 15000

Depth::Depth(const XnChar* strName) :
  m_bGenerating(FALSE),
  m_bDataAvailable(FALSE),
  m_nFrameID(0),
  m_nTimestamp(0),
  m_hScheduler(NULL),
  cvIn(strName),
  m_lastFPS(30)
{
  xnOSStrCopy(m_strName, strName, sizeof(m_strName)); //Copy in the strName
}
Depth::~Depth()
{
  
}

XnStatus Depth::Init()
{
  cvIn.getNextFrames(m_DFrame,m_IFrame);
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

  nRetVal = xnOSCreateThread(SchedulerThread, this, &m_hScheduler);
  if (nRetVal != XN_STATUS_OK)
  {
    m_bGenerating = FALSE;
    return (nRetVal);
  }
  m_generatingEvent.Raise();

  return (XN_STATUS_OK);
}

XnBool Depth::IsGenerating()
{
  return m_bGenerating;
}

void Depth::StopGenerating()
{
  m_bGenerating = FALSE;

  xnOSWaitForThreadExit(m_hScheduler, 100);

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

  int MaxFrame  =  cvIn.getNumFrames();
  int curFrame = cvIn.getCurFrames();
  if(curFrame >= MaxFrame){
    StopGenerating();
    return XN_STATUS_DEVICE_NOT_CONNECTED;
  }
    
  cvIn.getNextFrames(m_DFrame,m_IFrame);

  m_pDepthMap = (XnDepthPixel *) m_DFrame.data;
  
  m_nFrameID = curFrame;
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
  return (XnDepthPixel*) m_DFrame.data;
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
