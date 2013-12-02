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

void DataPacker::pack(AEGP_LayerH layer)
{
	if (m_packer != NULL && layer != NULL)
	{
		m_packer->pack(layer);
	}
}


