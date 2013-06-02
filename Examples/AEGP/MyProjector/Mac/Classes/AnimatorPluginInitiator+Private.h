//
//  AnimatorPluginInitiator+Private.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-1.
//
//

#ifndef __Projector__AnimatorPluginInitiator_Private__
#define __Projector__AnimatorPluginInitiator_Private__

#include "AE_GeneralPlug.h"

class AnimatorPluginInfo;

/**
 * 插件初始器, 客户端代码不应该调用该类
 * @author Zhenyu Yao
 */
class AnimatorPluginInitiator
{
	
public:
	
	/**
	 * 初始化插件, 参数对应 AEGP_PluginInitFuncPrototype 
	 */
	static void initialize(struct SPBasicSuite		*pica_basicP,	
						   A_long				 	major_versionL,	
						   A_long					minor_versionL,	
						   AEGP_PluginID			aegp_plugin_id,	
						   AEGP_GlobalRefcon		*global_refconP);
	
};

#endif /* defined(__Projector__AnimatorPluginInitiator_Private__) */
