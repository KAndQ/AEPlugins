//
//  DataPacker.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-4.
//
//

#ifndef __Projector__DataPacker__
#define __Projector__DataPacker__

#include "AE_GeneralPlug.h"

/**
 * 数据打包器, InitDataPacker 和 AnimDataPacker 数据打包的机器
 * @author Zhenyu Yao
 */
class DataPacker
{
public:
	DataPacker(DataPacker * packer);
	virtual ~DataPacker();
	
	/// 将数据打包起来
	virtual void pack(AEGP_LayerH layer);
	
private:
	DataPacker *		m_packer;
};

#endif /* defined(__Projector__DataPacker__) */
