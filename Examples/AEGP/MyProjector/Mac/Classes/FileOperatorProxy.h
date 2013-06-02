//
//  FileOperatorProxy.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#ifndef __Projector__FileOperatorProxy__
#define __Projector__FileOperatorProxy__

#include "AEConfig.h"
#include "IFileOperator.h"

/**
 * 文件操作的代理类
 * @author Zhenyu Yao
 */
class FileOperatorProxy : public IFileOperator
{
	
public:
	FileOperatorProxy();
	~FileOperatorProxy();
	
	virtual void saveAnimationData(bool forever);
	virtual void saveMoudleData();
	
private:
	
	IFileOperator *			m_operator;
};

#endif /* defined(__Projector__FileOperatorProxy__) */
