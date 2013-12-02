//
//  AnimDataPacker.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-4.
//
//

#ifndef __Projector__AnimDataPacker__
#define __Projector__AnimDataPacker__

#include "AEConfig.h"
#include "AE_Macros.h"
#include "DataPacker.h"

/**
 * 动画数据打包
 * @author Zhenyu Yao
 */
class AnimDataPacker : public DataPacker
{
	
#ifdef AE_OS_MAC
	
public:
	AnimDataPacker(DataPacker * packer, NSMutableDictionary * dict, bool forever);
	virtual ~AnimDataPacker();
	
protected:
	bool						m_forever;
	NSMutableDictionary *		m_dict;
	
#endif // AE_OS_MAC
	
};

/**
 * AnchorPoint 的动画数据打包
 * @author Zhenyu Yao
 */
class AnimDataAnchorPointPacker : public AnimDataPacker
{
public:
	AnimDataAnchorPointPacker(DataPacker * packer, NSMutableDictionary * dict, bool forever) : AnimDataPacker(packer, dict, forever) {};
	void pack(AEGP_LayerH layer);
};

/**
 * Position 的动画数据打包
 * @author Zhenyu Yao
 */
class AnimDataPositionPacker : public AnimDataPacker
{
public:
	AnimDataPositionPacker(DataPacker * packer, NSMutableDictionary * dict, bool forever) : AnimDataPacker(packer, dict, forever) {};
	void pack(AEGP_LayerH layer);
};

/**
 * Scale 的动画数据打包
 * @author Zhenyu Yao
 */
class AnimDataScalePacker : public AnimDataPacker
{
public:
	AnimDataScalePacker(DataPacker * packer, NSMutableDictionary * dict, bool forever) : AnimDataPacker(packer, dict, forever) {};
	void pack(AEGP_LayerH layer);
};

/**
 * Rotation 的动画数据打包
 * @author Zhenyu Yao
 */
class AnimDataRotationPacker : public AnimDataPacker
{
public:
	AnimDataRotationPacker(DataPacker * packer, NSMutableDictionary * dict, bool forever) : AnimDataPacker(packer, dict, forever) {};
	void pack(AEGP_LayerH layer);
};

/**
 * Opacity 的动画数据打包
 * @author Zhenyu Yao
 */
class AnimDataOpacityPacker : public AnimDataPacker
{
public:
	AnimDataOpacityPacker(DataPacker * packer, NSMutableDictionary * dict, bool forever) : AnimDataPacker(packer, dict, forever) {};
	void pack(AEGP_LayerH layer);
};

#endif /* defined(__Projector__AnimDataPacker__) */


