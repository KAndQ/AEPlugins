//
//  InitDataPacker.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-4.
//
//

#ifndef __Projector__InitDataPacker__
#define __Projector__InitDataPacker__

#include "AEConfig.h"
#include "AE_Macros.h"
#include "DataPacker.h"

/**
 * 负责对初始化数据进行打包的操作的基类
 * @author Zhenyu Yao
 */
class InitDataPacker : public DataPacker
{
	
#ifdef AE_OS_MAC
	
public:
	InitDataPacker(DataPacker * packer, NSMutableDictionary * dict);
	virtual ~InitDataPacker();
	
protected:
	NSMutableDictionary *			m_dict;
	
#endif // AE_OS_MAC
	
};

/**
 * 包装 AnchorPoint 数据
 * @author Zhenyu Yao
 */
class InitDataAnchorPointPacker : public InitDataPacker
{
public:
	InitDataAnchorPointPacker(DataPacker * packer, NSMutableDictionary * dict) : InitDataPacker(packer, dict) {};
	void pack(AEGP_LayerH layer);
};

/**
 * 包装 Position 数据
 * @author Zhenyu Yao
 */
class InitDataPositionPacker : public InitDataPacker
{
public:
	InitDataPositionPacker(DataPacker * packer, NSMutableDictionary * dict) : InitDataPacker(packer, dict) {};
	void pack(AEGP_LayerH layer);
};

/**
 * 包装 Scale 数据
 * @author Zhenyu Yao
 */
class InitDataScalePacker : public InitDataPacker
{
public:
	InitDataScalePacker(DataPacker * packer, NSMutableDictionary * dict) : InitDataPacker(packer, dict) {};
	void pack(AEGP_LayerH layer);
};

/**
 * 包装 Rotation 数据
 * @author Zhenyu Yao
 */
class InitDataRotationPacker : public InitDataPacker
{
public:
	InitDataRotationPacker(DataPacker * packer, NSMutableDictionary * dict) : InitDataPacker(packer, dict) {};
	void pack(AEGP_LayerH layer);
};

/**
 * 包装 Opacity 数据
 * @author Zhenyu Yao
 */
class InitDataOpacityPacker : public InitDataPacker
{
public:
	InitDataOpacityPacker(DataPacker * packer, NSMutableDictionary * dict) : InitDataPacker(packer, dict) {};
	void pack(AEGP_LayerH layer);
};

/**
 * 包装 ZOrder 数据
 */
class InitDataZOrderPacker : public InitDataPacker
{
public:
	InitDataZOrderPacker(DataPacker * packer, NSMutableDictionary * dict) : InitDataPacker(packer, dict) {};
	void pack(AEGP_LayerH layer);
};

#endif /* defined(__Projector__InitDataPacker__) */
