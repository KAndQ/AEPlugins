//
//  AnimatorPluginInitiator+Private.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-1.
//
//

#include "AnimatorPluginInitiator+Private.h"
#include "AnimatorPluginInfo.h"
#include "AnimatorPluginHooks.h"

#include "AEGP_SuiteHandler.h"
#include "AE_Macros.h"

void AnimatorPluginInitiator::initialize(struct SPBasicSuite *	pica_basicP,
										 A_long 				major_versionL,
										 A_long 				minor_versionL,
										 AEGP_PluginID 			aegp_plugin_id,
										 AEGP_GlobalRefcon *	global_refconP)
{
	A_Err err = A_Err_NONE;

	AnimatorPluginInfo * info = AnimatorPluginInfo::shared();
	
	info->m_basicSuite = pica_basicP;
	
	info->m_pluginID = aegp_plugin_id;
	
	// 初始化菜单
	AEGP_SuiteHandler suites(pica_basicP);
	AEGP_Command commandSeperator;
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&commandSeperator));
	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(commandSeperator, "-", AEGP_Menu_FILE, AEGP_MENU_INSERT_AT_BOTTOM));
	ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(info->m_pluginID, animatorPlugin_UpdateMenuHook, NULL));
	
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&info->m_commandID));
	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(info->m_commandID, "Export Animator File", AEGP_Menu_FILE, AEGP_MENU_INSERT_AT_BOTTOM));
	ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(info->m_pluginID, AEGP_HP_BeforeAE, info->m_commandID, animatorPlugin_CommandHook, NULL));
	
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&info->m_foreverCommandID));
	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(info->m_foreverCommandID, "Export Forever File", AEGP_Menu_FILE, AEGP_MENU_INSERT_AT_BOTTOM));
	ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(info->m_pluginID, AEGP_HP_BeforeAE, info->m_foreverCommandID, animatorPlugin_CommandHook, NULL));
	
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&info->m_moudleCommandID));
	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(info->m_moudleCommandID, "Export Moudle File", AEGP_Menu_FILE, AEGP_MENU_INSERT_AT_BOTTOM));
	ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(info->m_pluginID, AEGP_HP_BeforeAE, info->m_moudleCommandID, animatorPlugin_CommandHook, NULL));
	
	info->printInfomation();
}


