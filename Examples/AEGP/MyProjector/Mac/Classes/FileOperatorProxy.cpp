//
//  FileOperatorProxy.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-3.
//
//

#include "FileOperatorProxy.h"

#ifdef AE_OS_MAC
#include "FileOperator-Mac.h"
#else
#include "FileOperator-Win.h"
#endif // AE_OS_MAC

FileOperatorProxy::FileOperatorProxy()
{
#ifdef AE_OS_MAC
	m_operator = new FileOperator_Mac();
#else
	m_operator = new FileOperator_Win();
#endif // AE_OS_MAC
}

FileOperatorProxy::~FileOperatorProxy()
{
	delete m_operator, m_operator = NULL;
}

void FileOperatorProxy::saveAnimationData(bool forever)
{
	m_operator->saveAnimationData(forever);
}

void FileOperatorProxy::saveMoudleData()
{
	m_operator->saveMoudleData();
}


