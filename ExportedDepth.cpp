/*****************************************************************************
*  ttoocs.                                                                   *
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
#include "ExportedDepth.h"
#include "Depth.h"

void ExportedDepth::GetDescription( XnProductionNodeDescription* pDescription )
{
	pDescription->Type = XN_NODE_TYPE_DEPTH;
	strcpy(pDescription->strVendor, "OpenNI");
	strcpy(pDescription->strName, "Depth");
	pDescription->Version.nMajor = XN_MAJOR_VERSION;
	pDescription->Version.nMinor = XN_MINOR_VERSION;
	pDescription->Version.nMaintenance = XN_MAINTENANCE_VERSION;
	pDescription->Version.nBuild = XN_BUILD_VERSION;
}

XnStatus ExportedDepth::EnumerateProductionTrees( xn::Context& /*context*/, xn::NodeInfoList& TreesList, xn::EnumerationErrors* /*pErrors*/ )
{
	XnStatus nRetVal = XN_STATUS_OK;

	// return one option
	XnProductionNodeDescription desc;
	GetDescription(&desc);

	nRetVal = TreesList.Add(desc, NULL, NULL);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus ExportedDepth::Create( xn::Context& /*context*/, const XnChar* /*strInstanceName*/, const XnChar* /*strCreationInfo*/, xn::NodeInfoList* /*pNeededTrees*/, const XnChar* /*strConfigurationDir*/, xn::ModuleProductionNode** ppInstance )
{
	XnStatus nRetVal = XN_STATUS_OK;
	
  const char* inputfile = "in.oni";
	Depth* pDepth = new Depth(inputfile);

	nRetVal = pDepth->Init();
	if (nRetVal != XN_STATUS_OK)
	{
		delete pDepth;
		return (nRetVal);
	}

	*ppInstance = pDepth;
	
	return (XN_STATUS_OK);
}

void ExportedDepth::Destroy( xn::ModuleProductionNode* pInstance )
{
	delete pInstance;
}
