//
//  AnimatorPluginInfo.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-1.
//
//

#ifndef __Projector__AnimatorPluginInfo__
#define __Projector__AnimatorPluginInfo__

#include "AE_GeneralPlug.h"

/**
 * 动画插件基础信息
 * @author Zhenyu Yao
 */
class AnimatorPluginInfo
{
	
	friend class AnimatorPluginInitiator;
	
// static public
public:
	/// 单例模式
	static AnimatorPluginInfo * shared();
	
// Public
public:
	~AnimatorPluginInfo() {}
	
	/// Export Animator File 命令菜单 ID
	AEGP_Command			getCommandID() const { return m_commandID; }
	
	/// Export Forever File 命令菜单 ID
	AEGP_Command			getForeverCommandID() const { return m_foreverCommandID; }
	
	/// Export Moudle File 命令菜单 ID
	AEGP_Command			getMoudleCommandID() const { return m_moudleCommandID; }
	
	/// 插件 ID
	AEGP_PluginID			getPluginID() const { return m_pluginID; }
	
	/// 获得 SPBasicSuite
	SPBasicSuite *			getBasicSuite() const { return m_basicSuite; }
	
	/// 获得当前选择图层的名字, 注意: 使用完后 delete
	A_UTF16Char *			getActiveLayerName();
	
	/// 当前激活图层的索引
	A_long					getActiveLayerIndex();
	
	/// 当前项目路径, 如果项目还没有保存, 将返回空字符串, 注意: 使用完后 delete
	A_UTF16Char *			getProjectPath();
	
	/// 打印插件信息
	void					printInfomation();
	
	/// 获得当前激活的 Item
	AEGP_ItemH				getActiveItemH();
	
	/// 获得当前激活的 Composition
	AEGP_CompH				getActiveCompH();
	
	/// 获得当前激活的 Layer
	AEGP_LayerH				getActiveLayerH();
	
private:
	
	AnimatorPluginInfo();
	
	AEGP_Command			m_commandID;
	AEGP_Command			m_foreverCommandID;
	AEGP_Command			m_moudleCommandID;
	AEGP_PluginID			m_pluginID;
	SPBasicSuite *			m_basicSuite;
};

#endif /* defined(__Projector__AnimatorPluginInfo__) */
