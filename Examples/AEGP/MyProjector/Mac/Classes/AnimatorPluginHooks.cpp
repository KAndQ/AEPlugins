//
//  AnimatorPluginHooks.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-1.
//
//

#include "AnimatorPluginHooks.h"
#include "AnimatorPluginInfo.h"
#include "FileOperatorProxy.h"

#include "AEGP_SuiteHandler.h"

A_Err animatorPlugin_CommandHook(AEGP_GlobalRefcon		plugin_refconPV,
								 AEGP_CommandRefcon		refconPV,
								 AEGP_Command 			command,
								 AEGP_HookPriority 		hook_priority,
								 A_Boolean 				already_handledB,
								 A_Boolean * 			handledPB)
{
	A_Err err = A_Err_NONE;
	
	*handledPB = FALSE;
	
	FileOperatorProxy fileOperator;
	
	if (AnimatorPluginInfo::shared()->getCommandID() == command)
	{
		*handledPB = TRUE;
		fileOperator.saveAnimationData(false);
	}
	else if (AnimatorPluginInfo::shared()->getForeverCommandID() == command)
	{
		*handledPB = TRUE;
		fileOperator.saveAnimationData(true);
	}
	else if (AnimatorPluginInfo::shared()->getMoudleCommandID() == command)
	{
		*handledPB = TRUE;
		fileOperator.saveMoudleData();
	}
	else if (AnimatorPluginInfo::shared()->getBatchCommandID() == command)
	{
		*handledPB = TRUE;
		fileOperator.saveAllData(false);
	}
	else if (AnimatorPluginInfo::shared()->getBatchForeverCommandID() == command)
	{
		*handledPB = TRUE;
		fileOperator.saveAllData(true);
	}
	
	return err;
}

A_Err animatorPlugin_UpdateMenuHook(AEGP_GlobalRefcon			plugin_refconPV,
									AEGP_UpdateMenuRefcon		refconPV,
									AEGP_WindowType				active_window)
{
	A_Err err = A_Err_NONE;
	AnimatorPluginInfo * info = AnimatorPluginInfo::shared();
	AEGP_SuiteHandler suites(info->getBasicSuite());
	
	if (info->getCommandID() != 0)
	{
		if (info->getActiveLayerIndex() != -1)
		{
			suites.CommandSuite1()->AEGP_EnableCommand(info->getCommandID());
			suites.CommandSuite1()->AEGP_EnableCommand(info->getForeverCommandID());
		}
		else
		{
			suites.CommandSuite1()->AEGP_DisableCommand(info->getCommandID());
			suites.CommandSuite1()->AEGP_DisableCommand(info->getForeverCommandID());
		}
	}
	
	if (info->getActiveCompH() != NULL)
	{
		suites.CommandSuite1()->AEGP_EnableCommand(info->getMoudleCommandID());
		suites.CommandSuite1()->AEGP_EnableCommand(info->getBatchCommandID());
		suites.CommandSuite1()->AEGP_EnableCommand(info->getBatchForeverCommandID());
	}
	else
	{
		suites.CommandSuite1()->AEGP_DisableCommand(info->getMoudleCommandID());
		suites.CommandSuite1()->AEGP_DisableCommand(info->getBatchCommandID());
		suites.CommandSuite1()->AEGP_DisableCommand(info->getBatchForeverCommandID());
	}
	
	info->printInfomation();
	
	return err;
}


