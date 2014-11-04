//
//  FileOperator-Mac.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#include <Foundation/Foundation.h>

#include "AEGP_SuiteHandler.h"

#include "FileOperator-Mac.h"
#include "InitDataPacker.h"
#include "AnimDataPacker.h"
#include "AnimatorPluginInfo.h"
#include "CommonUtils.h"

#ifdef AE_OS_MAC

#include "MacNSStringUtils.h"

FileOperator_Mac::FileOperator_Mac()
{
}

FileOperator_Mac::~FileOperator_Mac()
{
}

void FileOperator_Mac::saveAnimationData(bool forever)
{
	saveAnimationData(AnimatorPluginInfo::shared()->getActiveLayerH(), forever);
}

void FileOperator_Mac::saveMoudleData()
{
	if (AnimatorPluginInfo::shared()->getActiveCompH() != NULL)
	{
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		
		A_long width, height;
		obtainCompositionItemDimensions(&width, &height);
		
		NSMutableDictionary * moudleDict = [[NSMutableDictionary alloc] init];
		[moudleDict setObject:NSStringFromSize(CGSizeMake(width, height)) forKey:@"Size"];
		[moudleDict setObject:NSStringFromPoint(CGPointMake(0.5, 0.5f)) forKey:@"AnchorPoint"];		// 默认 (0.5, 0.5), 需要手动在文件里调整, 没有在 AE 里面做控制面板
		
		// 将文件保存在与当前 AE 项目相同的路径处, 文件名是 Module_[ProjectName]_[CompositionName].plist
		A_UTF16Char * utf16ProjPath = AnimatorPluginInfo::shared()->getProjectPath();
		A_UTF16Char * utf16ItemName = obtainActiveItemName();
		NSString * projPath = NSStringFromA_UTF16Char(utf16ProjPath);
		NSString * projName = [[projPath lastPathComponent] stringByDeletingPathExtension];
		projPath = [projPath stringByDeletingLastPathComponent];
		NSString * itemName = NSStringFromA_UTF16Char(utf16ItemName);
		NSString * moudlePath = [NSString stringWithFormat:@"%@/Moudle_%@_%@.plist", projPath, projName, itemName];
		[moudleDict writeToFile:moudlePath atomically:YES];
		
		delete utf16ProjPath, utf16ProjPath = NULL;
		delete utf16ItemName, utf16ItemName = NULL;
		[moudleDict release];
		[pool release];
	}
}

void FileOperator_Mac::saveAllData(bool forever)
{
	saveMoudleData();
	
	// 导出全部图层的动画数据
	A_Err err = A_Err_NONE;
	AEGP_CompH compH = AnimatorPluginInfo::shared()->getActiveCompH();
	if (compH != NULL)
	{
		AEGP_SuiteHandler suites(AnimatorPluginInfo::shared()->getBasicSuite());
		A_long numLayers;	
		ERR(suites.LayerSuite7()->AEGP_GetCompNumLayers(compH, &numLayers));
		for (A_long i = 0; i < numLayers && err == A_Err_NONE; ++i)
		{
			AEGP_LayerH layer = NULL;
			ERR(suites.LayerSuite7()->AEGP_GetCompLayerByIndex(compH, i, &layer));
			if (layer != NULL)
				saveAnimationData(layer, forever);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////////////////////////

void FileOperator_Mac::saveAnimationData(AEGP_LayerH layer, bool forever)
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	NSMutableDictionary * mainDict = [[NSMutableDictionary alloc] init];
	NSMutableDictionary * initDataDict = [[NSMutableDictionary alloc] init];
	NSMutableDictionary * animDataDict = [[NSMutableDictionary alloc] init];
	
	// 包装初始化数据, 用下装饰者模式
	[mainDict setObject:initDataDict forKey:@"InitData"];
	InitDataAnchorPointPacker anchorPacker = InitDataAnchorPointPacker(NULL, initDataDict);
	InitDataPositionPacker positionPacker = InitDataPositionPacker(&anchorPacker, initDataDict);
	InitDataScalePacker scalePacker = InitDataScalePacker(&positionPacker, initDataDict);
	InitDataRotationPacker rotationPacker = InitDataRotationPacker(&scalePacker, initDataDict);
	InitDataOpacityPacker opacityPacker = InitDataOpacityPacker(&rotationPacker, initDataDict);
	InitDataZOrderPacker ZOrderPacker = InitDataZOrderPacker(&opacityPacker, initDataDict);
	ZOrderPacker.pack(layer);
	
	// 包装动画数据, 用了装饰者模式
	[mainDict setObject:animDataDict forKey:@"AnimData"];
	AnimDataAnchorPointPacker animAnchorPacker = AnimDataAnchorPointPacker(NULL, animDataDict, forever);
	AnimDataPositionPacker animPositionPacker = AnimDataPositionPacker(&animAnchorPacker, animDataDict, forever);
	AnimDataScalePacker animScalePacker = AnimDataScalePacker(&animPositionPacker, animDataDict, forever);
	AnimDataRotationPacker animRotationPacker = AnimDataRotationPacker(&animScalePacker, animDataDict, forever);
	AnimDataOpacityPacker animOpacityPacker = AnimDataOpacityPacker(&animRotationPacker, animDataDict, forever);
	animOpacityPacker.pack(layer);
    
    // 记录当前是否是循环播放模式
    [animDataDict setObject:[NSNumber numberWithBool:forever] forKey:@"Forever"];
    
    // 创建 Delay Action
    A_Err err = A_Err_NONE;
    AnimatorPluginInfo * info = AnimatorPluginInfo::shared();
    AEGP_SuiteHandler suites(info->getBasicSuite());
    A_Time time;
    ERR(suites.LayerSuite7()->AEGP_GetLayerOffset(layer, &time));
    float offset = obtainDurationInSecond(time);
    
    if (offset != 0)
    {
        NSMutableDictionary * delayDict = [[NSMutableDictionary alloc] init];
        [animDataDict setObject:delayDict forKey:@"Delay"];
        [delayDict setObject:@"DelayTime" forKey:@"Type"];
        [delayDict setObject:[NSNumber numberWithFloat:offset] forKey:@"Duration"];
        [delayDict release];
        
        // 修改 SumTime 时间
        float sumTime = [[animDataDict objectForKey:@"SumTime"] floatValue];
        sumTime += offset;
        [animDataDict setObject:[NSNumber numberWithFloat:sumTime] forKey:@"SumTime"];
    }
	
	// 将文件保存在与当前 AE 项目相同的路径处, 文件名是 Anim_[ProjectName]_[CompositionName]_[LayerName].plist
	A_UTF16Char * utfProjectName = AnimatorPluginInfo::shared()->getProjectPath();
	A_UTF16Char * utfLayerName = obtainLayerName(layer);
	A_UTF16Char * utfItemName = obtainActiveItemName();
	NSString * projPath = NSStringFromA_UTF16Char(utfProjectName);
	NSString * projName = [[projPath lastPathComponent] stringByDeletingPathExtension];
	NSString * layerName = NSStringFromA_UTF16Char(utfLayerName);
	NSString * itemName = NSStringFromA_UTF16Char(utfItemName);
	projPath = [projPath stringByDeletingLastPathComponent];
	projPath = [NSString stringWithFormat:@"%@/Anim_%@_%@_%@.plist", projPath, projName, itemName, layerName];
	[mainDict writeToFile:projPath atomically:YES];
	
	delete utfProjectName, utfProjectName = NULL;
	delete utfLayerName, utfLayerName = NULL;
	delete utfItemName, utfItemName = NULL;
	[animDataDict release];
	[initDataDict release];
	[mainDict release];
	
	[pool release];
}

#endif // AE_OS_MAC

















































////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// end file


