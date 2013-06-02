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
};

#endif // AE_OS_MAC

#endif /* defined(__Projector__FileOperator_Mac__) */
