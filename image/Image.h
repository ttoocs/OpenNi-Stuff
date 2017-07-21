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
#include <XnModuleCppInterface.h>
#include <XnEventT.h>

#include "cvin.h"
#include <OpenNI.h>

typedef XnRGB24Pixel XnImagePixel;

class Image :
  //public virtual xn::ModuleImageGenerator
  public virtual xn::ModuleImageGenerator //, public virtual openni::VideoStream::NewFrameListener
{
public:
  Image(const XnChar* strName);
  virtual ~Image();

  XnStatus Init();

  // ProductionNode methods
  virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);

  // Generator methods
  virtual XnStatus StartGenerating();
  virtual XnBool IsGenerating();
  virtual void StopGenerating();
  virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
  virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback);
  virtual XnStatus RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
  virtual void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback);
  virtual XnBool IsNewDataAvailable(XnUInt64& nTimestamp);
  virtual XnStatus UpdateData();
  virtual XnUInt32 GetDataSize();
  virtual XnUInt64 GetTimestamp();
  virtual XnUInt32 GetFrameID();

  // MapGenerator methods
  virtual XnUInt32 GetSupportedMapOutputModesCount();
  virtual XnStatus GetSupportedMapOutputModes(XnMapOutputMode aModes[], XnUInt32& nCount);
  virtual XnStatus SetMapOutputMode(const XnMapOutputMode& Mode);
  virtual XnStatus GetMapOutputMode(XnMapOutputMode& Mode);
  virtual XnStatus RegisterToMapOutputModeChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
  virtual void UnregisterFromMapOutputModeChange(XnCallbackHandle hCallback);

  // ImageGenerator methods

  virtual XnBool IsPixelFormatSupported(XnPixelFormat Format);
  virtual XnStatus SetPixelFormat(XnPixelFormat Format);
  virtual XnPixelFormat GetPixelFormat();
  virtual XnStatus RegisterToPixelFormatChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback) {return XN_STATUS_OK;}
  virtual void UnregisterFromPixelFormatChange(XnCallbackHandle hCallback) {};

  virtual XnUInt8* GetImageMap();
  virtual void GetFieldOfView(XnFieldOfView& FOV);
  virtual XnStatus RegisterToFieldOfViewChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
  virtual void UnregisterFromFieldOfViewChange(XnCallbackHandle hCallback);

private:
  static XN_THREAD_PROC SchedulerThread(void* pCookie);
  void OnNewFrame();

  cv::Mat m_IFrame, m_DFrame; 
  CVIN imgin;
 
  XnInt m_lastFPS;
  XnUInt32 m_lastFrame;
  XnBool m_bGenerating;
  XnBool m_bDataAvailable;
  XnImagePixel* m_pImageMap;
  XnUInt32 m_nFrameID;
  XnUInt64 m_nTimestamp;
  XN_THREAD_HANDLE m_hScheduler;
  XnBool m_bMirror;
  XnEventNoArgs m_generatingEvent;
  XnEventNoArgs m_dataAvailableEvent;
  XnEventNoArgs m_mirrorEvent;
  XnChar m_strName[XN_MAX_NAME_LENGTH];
};
