//
//  FileOperator-Mac.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#ifndef __Projector__FileOperator_Mac__
#define __Projector__FileOperator_Mac__

#include "AEConfig.h"
#include "IFileOperator.h"

#ifdef AE_OS_MAC

/**
 * Mac 上的文件操作
 * @author Zhenyu Yao
 */
class FileOperator_Mac : public IFileOperator
{
	
public:
	FileOperator_Mac();
	~FileOperator_Mac();
	
	void saveAnimationData(bool forever);
	void saveMoudleData();
	void saveAllData(bool forever);
	
private:
	
	/**
	 * 存储图层的数据
	 * @param layer 图层
	 * @param forever 动画类型是否循环
	 */
	void saveAnimationData(AEGP_LayerH layer, bool forever);
};

#endif // AE_OS_MAC

#endif /* defined(__Projector__FileOperator_Mac__) */
