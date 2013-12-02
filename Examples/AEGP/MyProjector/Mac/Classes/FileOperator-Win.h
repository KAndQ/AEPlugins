//
//  FileOperator-Win.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#ifndef __Projector__FileOperator_Win__
#define __Projector__FileOperator_Win__

#include "AEConfig.h"
#include "IFileOperator.h"

#ifdef AE_OS_WIN

/**
 * Win 上的文件操作
 * @author Zhenyu Yao
 */
class FileOperator_Win : public IFileOperator
{
	
public:
	
	FileOperator_Win();
	~FileOperator_Win();
	
	void saveAnimationData(bool forever);
	void saveMoudleData();
	void saveAllData(bool forever);
};

#endif // AE_OS_WIN

#endif /* defined(__Projector__FileOperator_Win__) */
