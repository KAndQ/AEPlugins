//
//  MacNSStringUtils.cpp
//  Projector
//
//  Created by Zhenyu Yao on 13-4-2.
//
//

#include "MacNSStringUtils.h"

#ifdef AE_OS_MAC

NSString * NSStringFromA_UTF16Char(const A_UTF16Char * utf16)
{
	assert(utf16 != NULL);
	
	size_t index = 0;
	while (utf16[index++] != 0);
	NSString * string = [[[NSString alloc] initWithBytes:utf16
												  length:(index - 1) * 2
												encoding:NSUTF16LittleEndianStringEncoding] autorelease];
	return string;
}

#endif


