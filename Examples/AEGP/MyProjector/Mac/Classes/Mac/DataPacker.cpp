//
//  DataPacker.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-4.
//
//

#include "DataPacker.h"

DataPacker::DataPacker(DataPacker * packer)
{
	m_packer = packer;
}

DataPacker::~DataPacker()
{
	m_packer = NULL;
}

void DataPacker::pack()
{
	if (m_packer != NULL)
	{
		m_packer->pack();
	}
}


