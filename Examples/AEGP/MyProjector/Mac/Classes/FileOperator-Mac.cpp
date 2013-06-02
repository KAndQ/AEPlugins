//
//  FileOperator-Mac.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#include <Foundation/Foundation.h>

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
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	NSMutableDictionary * mainDict = [[NSMutableDictionary alloc] init];
	NSMutableDictionary * initDataDict = [[NSMutableDictionary alloc] init];
	NSMutableDictionary * animDataDict = [[NSMutableDictionary alloc] init];
	
	// 包装初始化数据
	[mainDict setObject:initDataDict forKey:@"InitData"];
	InitDataAnchorPointPacker anchorPacker = InitDataAnchorPointPacker(NULL, initDataDict);
	InitDataPositionPacker positionPacker = InitDataPositionPacker(&anchorPacker, initDataDict);
	InitDataScalePacker scalePacker = InitDataScalePacker(&positionPacker, initDataDict);
	InitDataRotationPacker rotationPacker = InitDataRotationPacker(&scalePacker, initDataDict);
	InitDataOpacityPacker opacityPacker = InitDataOpacityPacker(&rotationPacker, initDataDict);
	opacityPacker.pack();
	
	// 包装动画数据
	[mainDict setObject:animDataDict forKey:@"AnimData"];
	AnimDataAnchorPointPacker animAnchorPacker = AnimDataAnchorPointPacker(NULL, animDataDict, forever);
	AnimDataPositionPacker animPositionPacker = AnimDataPositionPacker(&animAnchorPacker, animDataDict, forever);
	AnimDataScalePacker animScalePacker = AnimDataScalePacker(&animPositionPacker, animDataDict, forever);
	AnimDataRotationPacker animRotationPacker = AnimDataRotationPacker(&animScalePacker, animDataDict, forever);
	AnimDataOpacityPacker animOpacityPacker = AnimDataOpacityPacker(&animRotationPacker, animDataDict, forever);
	animOpacityPacker.pack();
	
	// 将文件保存在与当前 AE 项目相同的路径处, 文件名是 图层名.plist
	NSString * projPath = NSStringFromA_UTF16Char(AnimatorPluginInfo::shared()->getProjectPath());
	NSString * layerName = NSStringFromA_UTF16Char(AnimatorPluginInfo::shared()->getActiveLayerName());
	projPath = [projPath stringByDeletingLastPathComponent];
	projPath = [NSString stringWithFormat:@"%@/%@.plist", projPath, layerName];
	[mainDict writeToFile:projPath atomically:YES];
	
	[animDataDict release];
	[initDataDict release];
	[mainDict release];
	
	[pool release];
}

void FileOperator_Mac::saveMoudleData()
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	A_long width, height;
	obtainCompositionItemDimensions(&width, &height);
	
	NSMutableDictionary * moudleDict = [[NSMutableDictionary alloc] init];
	[moudleDict setObject:NSStringFromSize(CGSizeMake(width, height)) forKey:@"Size"];
	[moudleDict setObject:NSStringFromPoint(CGPointMake(0.5, 0.5f)) forKey:@"AnchorPoint"];		// 默认 (0.5, 0.5), 需要手动在文件里调整, 没有在 AE 里面做控制面板
	
	// 将文件保存在与当前 AE 项目相同的路径处, 文件名是 项目名_Moudle.plist
	NSString * projPath = NSStringFromA_UTF16Char(AnimatorPluginInfo::shared()->getProjectPath());
	NSString * projName = [projPath stringByDeletingPathExtension];
	NSString * moudlePath = [NSString stringWithFormat:@"%@_Moudle.plist", projName];
	[moudleDict writeToFile:moudlePath atomically:YES];
	
	[moudleDict release];
	[pool release];
}

#endif // AE_OS_MAC



