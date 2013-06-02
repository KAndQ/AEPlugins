//
//  InitDataPacker.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-4.
//
//

#include "InitDataPacker.h"
#include "CommonUtils.h"

#ifdef AE_OS_MAC

InitDataPacker::InitDataPacker(DataPacker * packer, NSMutableDictionary * dict) : DataPacker(packer)
{
	m_dict = [dict retain];
}

InitDataPacker::~InitDataPacker()
{
	[m_dict release], m_dict = nil;
}

void InitDataAnchorPointPacker::pack()
{
	InitDataPacker::pack();
	
	AEGP_TwoDVal twoVals;
	obtainInitAnchorPointDataWithPercent(&twoVals);
	CGPoint anchorPoint = CGPointMake(twoVals.x, twoVals.y);
	[m_dict setObject:NSStringFromPoint(anchorPoint) forKey:@"AnchorPoint"];
}

void InitDataPositionPacker::pack()
{
	InitDataPacker::pack();
	
	AEGP_TwoDVal twoVals;
	obtainInitPositionData(&twoVals);
	CGPoint position = CGPointMake(twoVals.x, twoVals.y);
	[m_dict setObject:NSStringFromPoint(position) forKey:@"Position"];
}

void InitDataScalePacker::pack()
{
	InitDataPacker::pack();
	
	AEGP_TwoDVal twoVals;
	obtainInitScaleData(&twoVals);
	CGPoint scale = CGPointMake(twoVals.x, twoVals.y);
	[m_dict setObject:NSStringFromPoint(scale) forKey:@"Scale"];
}

void InitDataRotationPacker::pack()
{
	InitDataPacker::pack();
	
	AEGP_OneDVal oneVals;
	obtainInitRotationData(&oneVals);
	[m_dict setObject:[NSNumber numberWithFloat:oneVals] forKey:@"Rotation"];
}

void InitDataOpacityPacker::pack()
{
	InitDataPacker::pack();
	
	AEGP_OneDVal oneVals;
	obtainInitOpacityData(&oneVals);
	[m_dict setObject:[NSNumber numberWithFloat:oneVals] forKey:@"Opacity"];
}

#endif // AE_OS_MAC


