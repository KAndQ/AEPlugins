//
//  AnimatorPluginHooks.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-1.
//
//

#ifndef __Projector__AnimatorPluginHooks__
#define __Projector__AnimatorPluginHooks__

#include "AE_GeneralPlug.h"

extern A_Err animatorPlugin_CommandHook(AEGP_GlobalRefcon		plugin_refconPV,
										AEGP_CommandRefcon		refconPV,
										AEGP_Command 			command,
										AEGP_HookPriority 		hook_priority,
										A_Boolean 				already_handledB,
										A_Boolean * 			handledPB);

extern A_Err animatorPlugin_UpdateMenuHook(AEGP_GlobalRefcon			plugin_refconPV,
										   AEGP_UpdateMenuRefcon		refconPV,
										   AEGP_WindowType				active_window);


#endif /* defined(__Projector__AnimatorPluginHooks__) */
