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
	
	if (AnimatorPluginInfo::shared()->getCommandID() == command)
	{
		*handledPB = TRUE;
		
		FileOperatorProxy fileOperator;
		fileOperator.saveAnimationData(false);
	}
	else if (AnimatorPluginInfo::shared()->getForeverCommandID() == command)
	{
		*handledPB = TRUE;
		
		FileOperatorProxy fileOperator;
		fileOperator.saveAnimationData(true);
	}
	else if (AnimatorPluginInfo::shared()->getMoudleCommandID() == command)
	{
		*handledPB = TRUE;
		
		FileOperatorProxy fileOperator;
		fileOperator.saveMoudleData();
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
	}
	else
	{
		suites.CommandSuite1()->AEGP_DisableCommand(info->getMoudleCommandID());
	}
	
	info->printInfomation();
	
	return err;
}


