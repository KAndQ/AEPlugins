//
//  AnimatorPluginInfo.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-1.
//
//

#include "AEConfig.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"

#include "AnimatorPluginInfo.h"
#include "CommonUtils.h"

#ifdef AE_OS_MAC
#include "MacNSStringUtils.h"
#endif

AnimatorPluginInfo * AnimatorPluginInfo::shared()
{
	static AnimatorPluginInfo * info = new AnimatorPluginInfo();
	return info;
}

AnimatorPluginInfo::AnimatorPluginInfo()
{
	m_commandID = 0;
	m_pluginID = 0;
	m_basicSuite = NULL;
}

A_UTF16Char * AnimatorPluginInfo::getActiveLayerName()
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_LayerH layerH = getActiveLayerH();
	if (layerH == NULL)
	{
		A_UTF16Char * value = new A_UTF16Char[1];
		memset(value, 0, sizeof(A_UTF16Char));
		return value;
	}
	
	AEGP_MemHandle memLayerNameHandle = NULL;
	AEGP_MemHandle memSourceNameHandle = NULL;
	ERR(suites.LayerSuite7()->AEGP_GetLayerName(m_pluginID, layerH, &memLayerNameHandle, &memSourceNameHandle));
	A_UTF16Char * layerName = UTF16CharDuplicate(suites.MemorySuite1(), memLayerNameHandle);
	ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(memLayerNameHandle));
	ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(memSourceNameHandle));
	
	return layerName;
}

A_long AnimatorPluginInfo::getActiveLayerIndex()
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_LayerH layerH = getActiveLayerH();
	if (layerH != NULL)
	{
		A_long layerIndex = -1;
		ERR(suites.LayerSuite7()->AEGP_GetLayerIndex(layerH, &layerIndex));
		return layerIndex;
	}
	
	return -1;
}

A_UTF16Char * AnimatorPluginInfo::getProjectPath()
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_ProjectH projH;
	AEGP_MemHandle memHandle = NULL;
	
	ERR(suites.ProjSuite6()->AEGP_GetProjectByIndex(0, &projH));
	if (projH == NULL)
	{
		A_UTF16Char * result = new A_UTF16Char[1];
		memset(result, 0, sizeof(A_UTF16Char));
		return result;
	}
	ERR(suites.ProjSuite6()->AEGP_GetProjectPath(projH, &memHandle));
	A_UTF16Char * path = UTF16CharDuplicate(suites.MemorySuite1(), memHandle);
	ERR(suites.MemorySuite1()->AEGP_FreeMemHandle(memHandle));
	
	return path;
}

void AnimatorPluginInfo::printInfomation()
{
	A_UTF16Char * utf16Path = getProjectPath();
	A_UTF16Char * layerName = getActiveLayerName();
	
#ifdef AE_OS_MAC
	
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	NSString * strPath = NSStringFromA_UTF16Char(utf16Path);
	NSString * strLayerName = NSStringFromA_UTF16Char(layerName);
	NSLog(@"PluginID:%u, CommandID:%u, ProjPath:%@, LayerName:%@",
		  getPluginID(), getCommandID(), strPath.length == 0 ? @"NULL" : strPath, strLayerName.length == 0 ? @"NULL" : strLayerName);
	
	// 测试打印帧信息
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_LayerH layerH = getActiveLayerH();
	if (layerH != NULL)
	{
		A_long width, height;
		
		// Composition 的大小
		obtainCompositionItemDimensions(&width, &height);
		NSLog(@"---- test width: %u, test height: %u ----", width, height);
		
		// 资源信息
		obtainSourceItemDimensions(&width, &height);
		NSLog(@"---- source width: %u, source height: %u ----", width, height);
		
		// 帧信息
		AEGP_StreamRefH streamRH;
		ERR(suites.StreamSuite4()->AEGP_GetNewLayerStream(m_pluginID, layerH, AEGP_LayerStream_POSITION, &streamRH));
		if (streamRH != NULL)
		{
			// 初始化信息
			AEGP_TwoDVal two;
			obtainInitAnchorPointData(&two);
			NSLog(@"---- initAnchorPoint.x: %f, initAnchorPoint.y: %f ----", two.x, two.y);
			obtainInitAnchorPointDataWithPercent(&two);
			NSLog(@"---- initAnchorPoint.x: %f, initAnchorPoint.y: %f ----", two.x, two.y);
			obtainInitPositionData(&two);
			NSLog(@"---- position.x: %f, position.y: %f ----", two.x, two.y);
			obtainInitScaleData(&two);
			NSLog(@"---- scale.x: %f, scale.y: %f ----", two.x, two.y);
			AEGP_OneDVal one;
			obtainInitRotationData(&one);
			NSLog(@"---- rotation: %f ----", one);
			obtainInitOpacityData(&one);
			NSLog(@"---- opacity: %f ----", one);
			
			AEGP_TwoDVal * pTwos = NULL;
			float * pTimes = NULL;
			A_long numkfs = 0;
			
			// Anchor Point
			obtainKeyframesAnchorPointData(&pTwos, &pTimes, &numkfs);
			NSLog(@"---- AnchorPoint %u frames infomation ----", numkfs);
			for (A_long i = 0; i < numkfs; ++i)
			{
				NSLog(@"---- Time: %f, AnchorPoint.x: %f, AnchorPoint.y: %f ----", pTimes[i], pTwos[i].x, pTwos[i].y);
			}
			delete pTwos, pTwos = NULL;
			delete pTimes, pTimes = NULL;
			obtainKeyframesAnchorPointDataWithPercent(&pTwos, &pTimes, &numkfs);
			NSLog(@"---- AnchorPoint percent %u frames infomation ----", numkfs);
			for (A_long i = 0; i < numkfs; ++i)
			{
				NSLog(@"---- Time: %f, AnchorPoint.x: %f, AnchorPoint.y: %f ----", pTimes[i], pTwos[i].x, pTwos[i].y);
			}
			delete pTwos, pTwos = NULL;
			delete pTimes, pTimes = NULL;
			
			// Position
			obtainKeyframesPositionData(&pTwos, &pTimes, &numkfs);
			NSLog(@"---- Position %u frames infomation ----", numkfs);
			for (A_long i = 0; i < numkfs; ++i)
			{
				NSLog(@"---- Time: %f, Position.x: %f, Position.y: %f ----", pTimes[i], pTwos[i].x, pTwos[i].y);
			}
			delete pTwos, pTwos = NULL;
			delete pTimes, pTimes = NULL;
			
			// Scale
			obtainKeyframesScaleData(&pTwos, &pTimes, &numkfs);
			NSLog(@"---- Scale %u frames infomation ----", numkfs);
			for (A_long i = 0; i < numkfs; ++i)
			{
				NSLog(@"---- Time: %f, Scale.x: %f, Scale.y: %f ----", pTimes[i], pTwos[i].x, pTwos[i].y);
			}
			delete pTwos, pTwos = NULL;
			delete pTimes, pTimes = NULL;
			
			// Rotation
			AEGP_OneDVal * pOneVals = NULL;
			obtainKeyframesRotationData(&pOneVals, &pTimes, &numkfs);
			NSLog(@"---- Rotation %u frames infomation ----", numkfs);
			for (A_long i = 0; i < numkfs; ++i)
			{
				NSLog(@"---- Time: %f, Rotation: %f ----", pTimes[i], pOneVals[i]);
			}
			delete pOneVals, pOneVals = NULL;
			
			// Opacity
			obtainKeyframesOpacityData(&pOneVals, &pTimes, &numkfs);
			NSLog(@"---- Opacity %u frames infomation ----", numkfs);
			for (A_long i = 0; i < numkfs; ++i)
			{
				NSLog(@"---- Time: %f, Opacity: %f ----", pTimes[i], pOneVals[i]);
			}
			delete pOneVals, pOneVals = NULL;
		}
	}
	
	[pool release];
	
#endif
	
	delete layerName, layerName = NULL;
	delete utf16Path, utf16Path = NULL;
}

AEGP_ItemH AnimatorPluginInfo::getActiveItemH()
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_ItemH itemH = NULL;
	ERR(suites.ItemSuite8()->AEGP_GetActiveItem(&itemH));
	return itemH;
}

AEGP_CompH AnimatorPluginInfo::getActiveCompH()
{
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_ItemH itemH = getActiveItemH();
	if (itemH != NULL)
	{
		A_Err err = A_Err_NONE;
		AEGP_CompH compH;
		ERR(suites.CompSuite9()->AEGP_GetCompFromItem(itemH, &compH));
		return compH;
	}
	
	return NULL;
}

AEGP_LayerH AnimatorPluginInfo::getActiveLayerH()
{
	A_Err err = A_Err_NONE;
	AEGP_SuiteHandler suites(m_basicSuite);
	AEGP_LayerH layerH;
	ERR(suites.LayerSuite7()->AEGP_GetActiveLayer(&layerH));
	return layerH;
}



