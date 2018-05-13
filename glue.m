#ifdef _x86_64_
//
//  glue.m
//  ropnroll_final
//
//  Created by jndok on 14/11/15.
//  Copyright © 2015 jndok. All rights reserved.
//

#include <mach/vm_types.h>
#import <Foundation/Foundation.h>

extern CFDictionaryRef OSKextCopyLoadedKextInfo(CFArrayRef, CFArrayRef);

vm_offset_t KextUnslidBaseAddress(const char *KextBundleName)
{
    return (vm_offset_t)[((NSNumber*)(((__bridge NSDictionary*)OSKextCopyLoadedKextInfo(NULL, NULL))[[NSString stringWithUTF8String:KextBundleName]][@"OSBundleLoadAddress"])) unsignedLongLongValue];
}
#endif
