/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
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

#include "Depth.h"

#include <OpenNI.h>

#define SUPPORTED_X_RES 400
#define SUPPORTED_Y_RES 300
#define SUPPORTED_FPS 30
#define MAX_DEPTH_VALUE	15000

Depth::Depth(const XnChar* strName) : 
	m_bGenerating(FALSE),
	m_bDataAvailable(FALSE),
	m_pDepthMap(NULL),
	m_nFrameID(0),
	m_nTimestamp(0),
	m_hScheduler(NULL),
	m_bMirror(FALSE)
{
  xnOSStrCopy(m_strName, strName, sizeof(m_strName)); //Copy in the strName
  openni::OpenNI::initialize();
}

Depth::~Depth()
{
  openni::OpenNI::shutdown();
	delete[] m_pDepthMap;
}

XnStatus Depth::Init()
{
	m_pDepthMap = new XnDepthPixel[SUPPORTED_X_RES * SUPPORTED_Y_RES];

  

	if (m_pDepthMap == NULL)
	{
		return XN_STATUS_ALLOC_FAILED;
	}

	return (XN_STATUS_OK);
}

XnBool Depth::IsCapabilitySupported( const XnChar* strCapabilityName )
{
	// we only support the mirror capability
	return (strcmp(strCapabilityName, XN_CAPABILITY_MIRROR) == 0);
}

XnStatus Depth::StartGenerating()
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	m_bGenerating = TRUE;

  std::cout << "Depth generating started with filename: " << m_strName << std::endl;


	// start scheduler thread
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

	// wait for thread to exit
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
  /*
	XnDepthPixel* pPixel = m_pDepthMap;

	// change our internal data, so that pixels go from frameID incrementally in both axes.
	for (XnUInt y = 0; y < SUPPORTED_Y_RES; ++y)
	{
		for (XnUInt x = 0; x < SUPPORTED_X_RES; ++x, ++pPixel)
		{
			*pPixel = (m_nFrameID + x + y) % MAX_DEPTH_VALUE;
		}
	}

	// if needed, mirror the map
	if (m_bMirror)
	{
		XnDepthPixel temp;

		for (XnUInt y = 0; y < SUPPORTED_Y_RES; ++y)
		{
			XnDepthPixel* pUp = &m_pDepthMap[y * SUPPORTED_X_RES];
			XnDepthPixel* pDown = &m_pDepthMap[(y+1) * SUPPORTED_X_RES - 1];

			for (XnUInt x = 0; x < SUPPORTED_X_RES/2; ++x, ++pUp, --pDown)
			{
				temp = *pUp;
				*pUp = *pDown;
				*pDown = temp;
			}
		}
	}

	m_nFrameID++;
	m_nTimestamp += 1000000 / SUPPORTED_FPS;

	// mark that data is old
	m_bDataAvailable = FALSE;
	
	return (XN_STATUS_OK);
  */
}


const void* Depth::GetData()
{
	return m_pDepthMap;
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

xn::ModuleMirrorInterface* Depth::GetMirrorInterface()
{
	return this;
}

XnStatus Depth::SetMirror( XnBool bMirror )
{
	m_bMirror = bMirror;
	m_mirrorEvent.Raise();
	return (XN_STATUS_OK);
}

XnBool Depth::IsMirrored()
{
	return m_bMirror;
}

XnStatus Depth::RegisterToMirrorChange( XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback )
{
	return m_mirrorEvent.Register(handler, pCookie, hCallback);
}

void Depth::UnregisterFromMirrorChange( XnCallbackHandle hCallback )
{
	m_mirrorEvent.Unregister(hCallback);
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
	return m_pDepthMap;
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
		xnOSSleep(1000000/SUPPORTED_FPS/1000);

		pThis->OnNewFrame();
	}

	XN_THREAD_PROC_RETURN(0);
}

void Depth::OnNewFrame()
{
	m_bDataAvailable = TRUE;
	m_dataAvailableEvent.Raise();
}
