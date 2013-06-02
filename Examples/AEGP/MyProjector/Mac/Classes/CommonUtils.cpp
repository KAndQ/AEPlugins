//
//  CommonUtils.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#include "AEConfig.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"

#include "CommonUtils.h"
#include "AnimatorPluginInfo.h"

// 反转 Y 轴坐标系
#define REVERSE_COOR_Y

A_UTF16Char * UTF16CharDuplicate(AEGP_MemorySuite1 * memSuite, AEGP_MemHandle memHandle)
{
	void * buffer = NULL;
	AEGP_MemSize size = 0;
	memSuite->AEGP_GetMemHandleSize(memHandle, &size);
	memSuite->AEGP_LockMemHandle(memHandle, &buffer);
	A_UTF16Char * utf16 = (A_UTF16Char *)(new char[size]);
	memcpy(utf16, buffer, size);
	memSuite->AEGP_UnlockMemHandle(memHandle);
	return utf16;
}

static void obtainLayerTimeInitData(AEGP_StreamVal2 * pVal2, AEGP_LayerStream whichStream)
{
	AnimatorPluginInfo * info = AnimatorPluginInfo::shared();
	AEGP_SuiteHandler suites(info->getBasicSuite());
	AEGP_LayerH layerH = info->getActiveLayerH();
	assert(layerH != NULL);
	
	AEGP_StreamRefH streamH;
	suites.StreamSuite4()->AEGP_GetNewLayerStream(info->getPluginID(), layerH, whichStream, &streamH);
	assert(streamH != NULL);
	
	A_Time initTime;
	initTime.value = 0;
	initTime.scale = 1;
	AEGP_StreamValue2 streamValue2;
	suites.StreamSuite4()->AEGP_GetNewStreamValue(info->getPluginID(),
												  streamH,
												  AEGP_LTimeMode_LayerTime,
												  &initTime,
												  false,
												  &streamValue2);
	*pVal2 = streamValue2.val;
	suites.StreamSuite4()->AEGP_DisposeStreamValue(&streamValue2);
	suites.StreamSuite4()->AEGP_DisposeStream(streamH);
}

void obtainInitAnchorPointData(AEGP_TwoDVal * pValue)
{
	AEGP_StreamVal2 streamVal2;
	obtainLayerTimeInitData(&streamVal2, AEGP_LayerStream_ANCHORPOINT);
	
	pValue->x = streamVal2.two_d.x;
	
#ifdef REVERSE_COOR_Y
	A_long width, height;
	obtainSourceItemDimensions(&width, &height);
	pValue->y = height - streamVal2.two_d.y;
#else
	pValue->y = streamVal2.two_d.y;
#endif // REVERSE_COOR_Y
}

void obtainInitAnchorPointDataWithPercent(AEGP_TwoDVal * pValue)
{
	AEGP_TwoDVal anchorPoint;
	obtainInitAnchorPointData(&anchorPoint);
	
	A_long width, height;
	obtainSourceItemDimensions(&width, &height);
	pValue->x = anchorPoint.x / width;
	pValue->y = anchorPoint.y / height;
}

void obtainInitPositionData(AEGP_TwoDVal * pValue)
{
	AEGP_StreamVal2 streamVal2;
	obtainLayerTimeInitData(&streamVal2, AEGP_LayerStream_POSITION);
	pValue->x = streamVal2.two_d.x;
#ifdef REVERSE_COOR_Y
	A_long width, height;
	obtainCompositionItemDimensions(&width, &height);
	pValue->y = height - streamVal2.two_d.y;
#else
	pValue->y = streamVal2.two_d.y;
#endif // REVERSE_COOR_Y
}

void obtainInitScaleData(AEGP_TwoDVal * pValue)
{
	AEGP_StreamVal2 streamVal2;
	obtainLayerTimeInitData(&streamVal2, AEGP_LayerStream_SCALE);
	pValue->x = streamVal2.two_d.x / 100.0f;
	pValue->y = streamVal2.two_d.y / 100.0f;
}

void obtainInitRotationData(AEGP_OneDVal * pValue)
{
	AEGP_StreamVal2 streamVal2;
	obtainLayerTimeInitData(&streamVal2, AEGP_LayerStream_ROTATION);
	*pValue = streamVal2.one_d;
}

void obtainInitOpacityData(AEGP_OneDVal * pValue)
{
	AEGP_StreamVal2 streamVal2;
	obtainLayerTimeInitData(&streamVal2, AEGP_LayerStream_OPACITY);
	*pValue = streamVal2.one_d * 255.0f / 100.0f;
}

float obtainDurationInSecond(A_Time time)
{
	// time.value 表示的是 AE 时间值, 但是它不是以 sec 为单位的
	// 换算 AE 时间值等于多少秒
	return (float)time.value / (float)time.scale;
}

float obtainFrameratePerSecond(A_Time time)
{
	// time.scale 表示的是帧频, 即:FPS, 但是其数值是放大了 1000 倍的, 例如: 30FPS, time.scale = 30000;
	return (float)time.scale / 1000.0f;
}

void obtainSourceItemDimensions(A_long * pWidth, A_long * pHeight)
{
	AEGP_SuiteHandler suites(AnimatorPluginInfo::shared()->getBasicSuite());
	AEGP_LayerH layerH = AnimatorPluginInfo::shared()->getActiveLayerH();
	if (layerH != NULL)
	{
		A_Err err = A_Err_NONE;
		AEGP_ItemH sourceItemH;
		ERR(suites.LayerSuite7()->AEGP_GetLayerSourceItem(layerH, &sourceItemH));
		ERR(suites.ItemSuite8()->AEGP_GetItemDimensions(sourceItemH, pWidth, pHeight));
	}
	else
	{
		*pWidth = *pHeight = 0;
	}
}

void obtainCompositionItemDimensions(A_long * pWidth, A_long * pHeight)
{
	AEGP_SuiteHandler suites(AnimatorPluginInfo::shared()->getBasicSuite());
	AEGP_CompH compH = AnimatorPluginInfo::shared()->getActiveCompH();
	if (compH != NULL)
	{
		A_Err err = A_Err_NONE;
		AEGP_ItemH itemH;
		ERR(suites.CompSuite9()->AEGP_GetItemFromComp(compH, &itemH));
		ERR(suites.ItemSuite8()->AEGP_GetItemDimensions(itemH, pWidth, pHeight));
		if (err != A_Err_NONE)
		{
			*pWidth = *pHeight = 0;
		}
	}
	else
	{
		*pWidth = *pHeight = 0;
	}
}

/// 必须手动调用 delete *ppVal2s 指向的内存空间
static void obtainLayerTimeFrameData(AEGP_StreamVal2 ** ppVal2s, float ** ppTimes, A_long * pNumkfs, AEGP_LayerStream whichStream)
{
	A_Err err = A_Err_NONE;
	AnimatorPluginInfo * info = AnimatorPluginInfo::shared();
	AEGP_SuiteHandler suites(info->getBasicSuite());
	AEGP_LayerH layerH = info->getActiveLayerH();
	assert(layerH != NULL);

	AEGP_StreamRefH streamH;
	err = suites.StreamSuite4()->AEGP_GetNewLayerStream(info->getPluginID(), layerH, whichStream, &streamH);
	assert(err == A_Err_NONE);
	assert(streamH != NULL);
	
	err = suites.KeyframeSuite4()->AEGP_GetStreamNumKFs(streamH, pNumkfs);
	assert(err == A_Err_NONE);
	
	if (*pNumkfs == 0)
	{
		suites.StreamSuite4()->AEGP_DisposeStream(streamH);
		*ppVal2s = NULL;
		*ppTimes = NULL;
		return;
	}
	
	*ppTimes = new float[*pNumkfs];
	*ppVal2s = new AEGP_StreamVal2[*pNumkfs];
	float lastTime = 0.0f;
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		// 计算相对时间
		A_Time time;
		err = suites.KeyframeSuite4()->AEGP_GetKeyframeTime(streamH, i, AEGP_LTimeMode_LayerTime, &time);
		assert(err == A_Err_NONE);
		float sec = obtainDurationInSecond(time);
		(*ppTimes)[i] = sec - lastTime;
		lastTime = sec;
		
		// 获得数值
		AEGP_StreamValue2 streamValue2;
		err = suites.KeyframeSuite4()->AEGP_GetNewKeyframeValue(info->getPluginID(), streamH, i, &streamValue2);
		assert(err == A_Err_NONE);
		(*ppVal2s)[i] = streamValue2.val;
		err = suites.StreamSuite4()->AEGP_DisposeStreamValue(&streamValue2);
		assert(err == A_Err_NONE);
	}
	
	suites.StreamSuite4()->AEGP_DisposeStream(streamH);
}

void obtainKeyframesAnchorPointData(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs)
{
	AEGP_StreamVal2 * pStreamVal2s = NULL;
	obtainLayerTimeFrameData(&pStreamVal2s, ppTimes, pNumkfs, AEGP_LayerStream_ANCHORPOINT);
	
	if (*pNumkfs == 0)
	{
		*ppValues = NULL;
		*ppTimes = NULL;
		return;
	}
	
#ifdef REVERSE_COOR_Y
	A_long width, height;
	obtainSourceItemDimensions(&width, &height);
#endif // REVERSE_COOR_Y
	
	AEGP_TwoDVal * pTwoVals = new AEGP_TwoDVal[*pNumkfs];
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		pTwoVals[i].x = pStreamVal2s[i].two_d.x;
#ifdef REVERSE_COOR_Y
		pTwoVals[i].y = height - pStreamVal2s[i].two_d.y;
#else
		pTwoVals[i].y = pStreamVal2s[i].two_d.y;
#endif // REVERSE_COOR_Y
	}
	*ppValues = pTwoVals;
	
	delete pStreamVal2s, pStreamVal2s = NULL;
}

void obtainKeyframesAnchorPointDataWithPercent(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs)
{
	A_long width, height;
	obtainSourceItemDimensions(&width, &height);
	
	obtainKeyframesAnchorPointData(ppValues, ppTimes, pNumkfs);
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		(*ppValues)[i].x /= width;
		(*ppValues)[i].y /= height;
	}
}

void obtainKeyframesPositionData(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs)
{
	AEGP_StreamVal2 * pStreamVal2s = NULL;
	obtainLayerTimeFrameData(&pStreamVal2s, ppTimes, pNumkfs, AEGP_LayerStream_POSITION);
	
	if (*pNumkfs == 0)
	{
		*ppValues = NULL;
		*ppTimes = NULL;
		return;
	}
	
#ifdef REVERSE_COOR_Y
	A_long width, height;
	obtainCompositionItemDimensions(&width, &height);
#endif // REVERSE_COOR_Y
	
	AEGP_TwoDVal * pTwoVals = new AEGP_TwoDVal[*pNumkfs];
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		pTwoVals[i].x = pStreamVal2s[i].two_d.x;
#ifdef REVERSE_COOR_Y
		pTwoVals[i].y = height - pStreamVal2s[i].two_d.y;
#else
		pTwoVals[i].y = pStreamVal2s[i].two_d.y;
#endif // REVERSE_COOR_Y
	}
	*ppValues = pTwoVals;
	
	delete pStreamVal2s, pStreamVal2s = NULL;
}

void obtainKeyframesScaleData(AEGP_TwoDVal ** ppValues, float ** ppTimes, A_long * pNumkfs)
{
	AEGP_StreamVal2 * pStreamVal2s = NULL;
	obtainLayerTimeFrameData(&pStreamVal2s, ppTimes, pNumkfs, AEGP_LayerStream_SCALE);
	
	if (*pNumkfs == 0)
	{
		*ppValues = NULL;
		*ppTimes = NULL;
		return;
	}
	
	AEGP_TwoDVal * pTwoVals = new AEGP_TwoDVal[*pNumkfs];
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		pTwoVals[i].x = pStreamVal2s[i].two_d.x / 100.0f;
		pTwoVals[i].y = pStreamVal2s[i].two_d.y / 100.0f;
	}
	*ppValues = pTwoVals;
	
	delete pStreamVal2s, pStreamVal2s = NULL;
}

void obtainKeyframesRotationData(AEGP_OneDVal ** ppValues, float ** ppTimes, A_long * pNumkfs)
{
	AEGP_StreamVal2 * pStreamVal2s = NULL;
	obtainLayerTimeFrameData(&pStreamVal2s, ppTimes, pNumkfs, AEGP_LayerStream_ROTATION);
	
	if (*pNumkfs == 0)
	{
		*ppValues = NULL;
		*ppTimes = NULL;
		return;
	}
	
	AEGP_OneDVal * pOneVals = new AEGP_OneDVal[*pNumkfs];
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		pOneVals[i] = pStreamVal2s[i].one_d;
	}
	*ppValues = pOneVals;
	
	delete pStreamVal2s, pStreamVal2s = NULL;
}

void obtainKeyframesOpacityData(AEGP_OneDVal ** ppValues, float ** ppTimes, A_long * pNumkfs)
{
	AEGP_StreamVal2 * pStreamVal2s = NULL;
	obtainLayerTimeFrameData(&pStreamVal2s, ppTimes, pNumkfs, AEGP_LayerStream_OPACITY);
	
	if (*pNumkfs == 0)
	{
		*ppValues = NULL;
		*ppTimes = NULL;
		return;
	}
	
	AEGP_OneDVal * pOneVals = new AEGP_OneDVal[*pNumkfs];
	for (A_long i = 0; i < *pNumkfs; ++i)
	{
		pOneVals[i] = pStreamVal2s[i].one_d * 255.0f / 100.0f;
	}
	*ppValues = pOneVals;
	
	delete pStreamVal2s, pStreamVal2s = NULL;
}


