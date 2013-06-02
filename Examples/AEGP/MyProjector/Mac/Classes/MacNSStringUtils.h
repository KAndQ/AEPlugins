//
//  MacNSStringUtils.h
//  Projector
//
//  Created by Zhenyu Yao on 13-4-2.
//
//

#ifndef __Projector__MacNSStringUtils__
#define __Projector__MacNSStringUtils__

#include "AEConfig.h"
#include "AE_GeneralPlug.h"

#ifdef AE_OS_MAC

/// 将 UTF16 字符串转化为 NSString 对象, UTF16 必须采用 Little Endian 编码序列
extern NSString * NSStringFromA_UTF16Char(const A_UTF16Char * utf16);

#endif

#endif /* defined(__Projector__MacNSStringUtils__) */
