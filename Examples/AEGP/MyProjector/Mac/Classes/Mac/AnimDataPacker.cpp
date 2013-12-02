//
//  AnimDataPacker.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-4.
//
//

#include "AnimDataPacker.h"
#include "CommonUtils.h"

#ifdef AE_OS_MAC

/// 创建定义动画的 NSMutableDictionary 对象, 需要手动 release 返回的 NSMutableDictionary 对象
static NSMutableDictionary * createAnimDict(NSMutableDictionary * mainDict, NSString * animType, bool forever)
{
	NSMutableDictionary * animDict = [[NSMutableDictionary alloc] init];
	[mainDict setObject:animDict forKey:animType];
	
	// 如果是循环动画则先定义 RepeatForever 类型
	if (forever)
	{
		NSMutableDictionary * actionDict = [[NSMutableDictionary alloc] init];
		[animDict setObject:@"RepeatForever" forKey:@"Type"];
		[animDict setObject:actionDict forKey:@"Action"];
		[animDict release];
		return actionDict;
	}
	
	return animDict;
}

/// 创建保存 Action 的数组, 需要手动 release 返回的 NSMutableArray 对象
static NSMutableArray * createActionsArray(NSMutableDictionary * animDict)
{
	[animDict setObject:@"Sequence" forKey:@"Type"];
	NSMutableArray * actionsArray = [[NSMutableArray alloc] init];
	[animDict setObject:actionsArray forKey:@"Actions"];
	return actionsArray;
}

/// 打包 AnchorPointTo Action
static void packAnchorPointTo(AEGP_TwoDVal * pTwoVals, float * pTimes, A_long index, NSMutableDictionary * animDict)
{
	[animDict setObject:@"AnchorPointTo" forKey:@"Type"];
	[animDict setObject:NSStringFromPoint(CGPointMake(pTwoVals[index].x, pTwoVals[index].y)) forKey:@"AnchorPoint"];
	[animDict setObject:[NSNumber numberWithFloat:pTimes[index]] forKey:@"Duration"];
}

/// 打包 MoveTo Action
static void packMoveTo(AEGP_TwoDVal * pTwoVals, float * pTimes, A_long index, NSMutableDictionary * animDict)
{
	[animDict setObject:@"MoveTo" forKey:@"Type"];
	[animDict setObject:NSStringFromPoint(CGPointMake(pTwoVals[index].x, pTwoVals[index].y)) forKey:@"Position"];
	[animDict setObject:[NSNumber numberWithFloat:pTimes[index]] forKey:@"Duration"];
}

/// 打包 ScaleTo Action
static void packScaleTo(AEGP_TwoDVal * pTwoVals, float * pTimes, A_long index, NSMutableDictionary * animDict)
{
	[animDict setObject:@"ScaleTo" forKey:@"Type"];
	[animDict setObject:[NSNumber numberWithFloat:pTimes[index]] forKey:@"Duration"];
	[animDict setObject:[NSNumber numberWithFloat:pTwoVals[index].x] forKey:@"ScaleX"];
	[animDict setObject:[NSNumber numberWithFloat:pTwoVals[index].y] forKey:@"ScaleY"];
}

/// 打包 RotateTo Action
static void packRotateTo(AEGP_OneDVal * pOneVals, float * pTimes, A_long index, NSMutableDictionary * animDict)
{
	[animDict setObject:@"RotateTo" forKey:@"Type"];
	[animDict setObject:[NSNumber numberWithFloat:pTimes[index]] forKey:@"Duration"];
	[animDict setObject:[NSNumber numberWithFloat:pOneVals[index]] forKey:@"Angle"];
}

/// 打包 FadeTo Action
static void packFadeTo(AEGP_OneDVal * pOneVals, float * pTimes, A_long index, NSMutableDictionary * animDict)
{
	[animDict setObject:@"FadeTo" forKey:@"Type"];
	[animDict setObject:[NSNumber numberWithFloat:pTimes[index]] forKey:@"Duration"];
	[animDict setObject:[NSNumber numberWithFloat:pOneVals[index]] forKey:@"Opacity"];
}

AnimDataPacker::AnimDataPacker(DataPacker * packer, NSMutableDictionary * dict, bool forever) : DataPacker(packer)
{
	m_forever = forever;
	m_dict = [dict retain];
}

AnimDataPacker::~AnimDataPacker()
{
	[m_dict release], m_dict = nil;
}

void AnimDataAnchorPointPacker::pack(AEGP_LayerH layer)
{
	DataPacker::pack(layer);
	
	AEGP_TwoDVal * pTwoVals = NULL;
	float * pTimes = NULL;
	A_long numkfs = 0;
	obtainKeyframesAnchorPointDataWithPercent(layer, &pTwoVals, &pTimes, &numkfs);
	
	// 没有动画则直接返回, 解析的时候直接判断如果 key AnchorPoint 返回的值是 nil 则表示没有动画
	if (numkfs == 0)
	{
		return;
	}
	
	// 创建 AnchorPoint 类型动画
	NSMutableDictionary * animDict = createAnimDict(m_dict, @"AnchorPoint", m_forever);
	
	// 如果只有一帧, 那么则直接定义其动画, 而不使用 CCSequence
	if (numkfs == 1)
	{
		packAnchorPointTo(pTwoVals, pTimes, 0, animDict);
	}
	else
	{
		NSMutableArray * actionsArray = createActionsArray(animDict);
		for (A_long i = 0; i < numkfs; ++i)
		{
			NSMutableDictionary * actionDict = [[NSMutableDictionary alloc] init];
			packAnchorPointTo(pTwoVals, pTimes, i, actionDict);
			[actionsArray addObject:actionDict];
			[actionDict release];
		}
		[actionsArray release];
	}
	
	delete pTwoVals, pTwoVals = NULL;
	delete pTimes, pTimes = NULL;
	[animDict release], animDict = nil;
}

void AnimDataPositionPacker::pack(AEGP_LayerH layer)
{
	DataPacker::pack(layer);
	
	AEGP_TwoDVal * pTwoVals = NULL;
	float * pTimes = NULL;
	A_long numkfs = 0;
	obtainKeyframesPositionData(layer, &pTwoVals, &pTimes, &numkfs);
	
	if (numkfs == 0)
	{
		return;
	}
	
	NSMutableDictionary * animDict = createAnimDict(m_dict, @"Position", m_forever);
	
	if (numkfs == 1)
	{
		packMoveTo(pTwoVals, pTimes, 0, animDict);
	}
	else
	{
		NSMutableArray * actionsArray = createActionsArray(animDict);
		for (A_long i = 0; i < numkfs; ++i)
		{
			NSMutableDictionary * actionDict = [[NSMutableDictionary alloc] init];
			packMoveTo(pTwoVals, pTimes, i, actionDict);
			[actionsArray addObject:actionDict];
			[actionDict release];
		}
		[actionsArray release];
	}
	
	delete pTwoVals, pTwoVals = NULL;
	delete pTimes, pTimes = NULL;
	[animDict release], animDict = nil;
}

void AnimDataScalePacker::pack(AEGP_LayerH layer)
{
	DataPacker::pack(layer);
	
	AEGP_TwoDVal * pTwoVals = NULL;
	float * pTimes = NULL;
	A_long numkfs = 0;
	obtainKeyframesScaleData(layer, &pTwoVals, &pTimes, &numkfs);
	
	if (numkfs == 0)
	{
		return;
	}
	
	NSMutableDictionary * animDict = createAnimDict(m_dict, @"Scale", m_forever);
	
	if (numkfs == 1)
	{
		packScaleTo(pTwoVals, pTimes, 0, animDict);
	}
	else
	{
		NSMutableArray * actionsArray = createActionsArray(animDict);
		for (A_long i = 0; i < numkfs; ++i)
		{
			NSMutableDictionary * actionDict = [[NSMutableDictionary alloc] init];
			packScaleTo(pTwoVals, pTimes, i, actionDict);
			[actionsArray addObject:actionDict];
			[actionDict release];
		}
		[actionsArray release];
	}
	
	delete pTwoVals, pTwoVals = NULL;
	delete pTimes, pTimes = NULL;
	[animDict release], animDict = nil;
}

void AnimDataRotationPacker::pack(AEGP_LayerH layer)
{
	DataPacker::pack(layer);
	
	AEGP_OneDVal * pOneVals = NULL;
	float * pTimes = NULL;
	A_long numkfs = 0;
	obtainKeyframesRotationData(layer, &pOneVals, &pTimes, &numkfs);
	
	if (numkfs == 0)
	{
		return;
	}
	
	NSMutableDictionary * animDict = createAnimDict(m_dict, @"Rotation", m_forever);
	
	if (numkfs == 1)
	{
		packRotateTo(pOneVals, pTimes, 0, animDict);
	}
	else
	{
		NSMutableArray * actionsArray = createActionsArray(animDict);
		for (A_long i = 0; i < numkfs; ++i)
		{
			NSMutableDictionary * actionDict = [[NSMutableDictionary alloc] init];
			packRotateTo(pOneVals, pTimes, i, actionDict);
			[actionsArray addObject:actionDict];
			[actionDict release];
		}
		[actionsArray release];
	}
	
	delete pOneVals, pOneVals = NULL;
	delete pTimes, pTimes = NULL;
	[animDict release], animDict = nil;
}

void AnimDataOpacityPacker::pack(AEGP_LayerH layer)
{
	DataPacker::pack(layer);
	
	AEGP_OneDVal * pOneVals = NULL;
	float * pTimes = NULL;
	A_long numkfs = 0;
	obtainKeyframesOpacityData(layer, &pOneVals, &pTimes, &numkfs);
	
	if (numkfs == 0)
	{
		return;
	}
	
	NSMutableDictionary * animDict = createAnimDict(m_dict, @"Opacity", m_forever);
	
	if (numkfs == 1)
	{
		packFadeTo(pOneVals, pTimes, 0, animDict);
	}
	else
	{
		NSMutableArray * actionsArray = createActionsArray(animDict);
		for (A_long i = 0; i < numkfs; ++i)
		{
			NSMutableDictionary * actionDict = [[NSMutableDictionary alloc] init];
			packFadeTo(pOneVals, pTimes, i, actionDict);
			[actionsArray addObject:actionDict];
			[actionDict release];
		}
		[actionsArray release];
	}
	
	delete pOneVals, pOneVals = NULL;
	delete pTimes, pTimes = NULL;
	[animDict release], animDict = nil;
}

#endif // AE_OS_MAC




