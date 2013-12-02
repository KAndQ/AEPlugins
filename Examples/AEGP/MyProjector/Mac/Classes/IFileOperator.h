//
//  IFileOperator.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#ifndef __Projector__IFileOperator__
#define __Projector__IFileOperator__

#include "AE_GeneralPlug.h"

/**
 * 文件操作的接口
 * @author Zhenyu Yao
 */
class IFileOperator
{
	
public:
	
	IFileOperator() {};
	virtual ~IFileOperator() {};
	
	/// 存储 Layer 动画数据
	virtual void saveAnimationData(bool forever) = 0;
	
	/// 存储 Moudle 数据
	virtual void saveMoudleData() = 0;
	
	/// 存储当前 Composition 的所有数据
	virtual void saveAllData(bool forever) = 0;
};

#endif /* defined(__Projector__IFileOperator__) */
