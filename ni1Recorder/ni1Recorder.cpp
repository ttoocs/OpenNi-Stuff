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
#include <XnOpenNI.h>
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <XnFPSCalculator.h>

#include <iostream>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

bool erroring(XnStatus nRetVal){
    if (nRetVal != XN_STATUS_OK){
      std::cout << xnGetStatusString(nRetVal) << std::endl;
      return false;
    }  
    return true;
}

using namespace xn;

int main()
{
	XnStatus nRetVal = XN_STATUS_OK;

	Context context;
	ScriptNode scriptNode;
	EnumerationErrors errors;

  context.Init();
  
  xn::ImageGenerator imgGen;
  xn::DepthGenerator DGen;  

  nRetVal = DGen.Create(context);
  erroring(nRetVal);  
  nRetVal = imgGen.Create(context);
  erroring(nRetVal);  

  //Recording stuff.
  Recorder recorder;
  nRetVal = recorder.Create(context);
  erroring(nRetVal);  

  nRetVal = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, "oni1.oni");
  erroring(nRetVal);  
  
  nRetVal = recorder.AddNodeToRecording(DGen);
  erroring(nRetVal);  
  
  nRetVal = recorder.AddNodeToRecording(imgGen);
  erroring(nRetVal);  
 
  std::cout << "Starting." << std::endl; 

/*
  nRetVal = recorder.start();
  erroring(nRetVal);  
  nRetVal = DGen.StartGenerating();
  erroring(nRetVal);  
  nRetVal = imgGen.StartGenerating();
  erroring(nRetVal);  
*/

  nRetVal = context.StartGeneratingAll();
  erroring(nRetVal);  

  //xnOSSleep(10000);


// /*
  int i=0;
  while (true){
    nRetVal = context.WaitAnyUpdateAll();
//    nRetVal = context.WaitAndUpdateAll();
    recorder.Record();
    std::cout << "Frame: " << i << std::endl;

    if(!erroring(nRetVal)){
      std::cout << "Error, breaking." << std::endl;
      break;
    }
//
//    XnMapOutputMode Imode;
//    XnMapOutputMode Dmode;
//    imgGen.GetMapOutputMode(Imode);
//    DGen.GetMapOutputMode(Dmode);
//
//    int Iheight = Imode.nYRes;
//    int Dheight = Dmode.nYRes;
//    int Iwidth = Imode.nXRes;
//    int Dwidth = Dmode.nXRes;
//    
//    std::cout << "Img: Height: " << Iheight << ", Width: " << Iwidth <<  std::endl;
//  
//    cv::Mat img = cv::Mat( Iheight, Iwidth, CV_8UC3, (void *) imgGen.GetData()); //Color
// 
//    cv::imwrite("cImg"+std::to_string(i)+".jpg", img);
//
//    std::cout << "Depth: Height: " << Dheight << ", Width: " << Dwidth <<  std::endl;
//
//    cv::Mat dimg = cv::Mat (Dheight, Dwidth,  CV_8U , (void *) DGen.GetData()); //Depth
//    cv::imwrite("dImg"+std::to_string(i)+".jpg", dimg);
    i++;
  }
 //*/
  nRetVal = recorder.RemoveNodeFromRecording(DGen);
  nRetVal = recorder.RemoveNodeFromRecording(imgGen);
	recorder.Release();

  context.Release();

	return 0;
}
