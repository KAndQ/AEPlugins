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

void InitDataAnchorPointPacker::pack(AEGP_LayerH layer)
{
	InitDataPacker::pack(layer);
	
	AEGP_TwoDVal twoVals;
	obtainInitAnchorPointDataWithPercent(layer, &twoVals);
	CGPoint anchorPoint = CGPointMake(twoVals.x, twoVals.y);
	[m_dict setObject:NSStringFromPoint(anchorPoint) forKey:@"AnchorPoint"];
}

void InitDataPositionPacker::pack(AEGP_LayerH layer)
{
	InitDataPacker::pack(layer);
	
	AEGP_TwoDVal twoVals;
	obtainInitPositionData(layer, &twoVals);
	CGPoint position = CGPointMake(twoVals.x, twoVals.y);
	[m_dict setObject:NSStringFromPoint(position) forKey:@"Position"];
}

void InitDataScalePacker::pack(AEGP_LayerH layer)
{
	InitDataPacker::pack(layer);
	
	AEGP_TwoDVal twoVals;
	obtainInitScaleData(layer, &twoVals);
	CGPoint scale = CGPointMake(twoVals.x, twoVals.y);
	[m_dict setObject:NSStringFromPoint(scale) forKey:@"Scale"];
}

void InitDataRotationPacker::pack(AEGP_LayerH layer)
{
	InitDataPacker::pack(layer);
	
	AEGP_OneDVal oneVals;
	obtainInitRotationData(layer, &oneVals);
	[m_dict setObject:[NSNumber numberWithFloat:oneVals] forKey:@"Rotation"];
}

void InitDataOpacityPacker::pack(AEGP_LayerH layer)
{
	InitDataPacker::pack(layer);
	
	AEGP_OneDVal oneVals;
	obtainInitOpacityData(layer, &oneVals);
	[m_dict setObject:[NSNumber numberWithFloat:oneVals] forKey:@"Opacity"];
}

void InitDataZOrderPacker::pack(AEGP_LayerH layer)
{
	InitDataPacker::pack(layer);
	
	AEGP_OneDVal oneVals;
	obtainInitZOrderData(layer, &oneVals);
	[m_dict setObject:[NSNumber numberWithInt:oneVals] forKey:@"ZOrder"];
}

#endif // AE_OS_MAC


