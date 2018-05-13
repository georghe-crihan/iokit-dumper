//
//  runtime_parsing.h
//  tree-parser-priv-final
//
//  Created by jndok on 29/12/15.
//  Copyright © 2015 jndok. All rights reserved.
//

#ifndef runtime_parsing_h
#define runtime_parsing_h

#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <mach-o/loader.h>

#include "../../helper.h"
#include "../kernel.h"

/***/

__attribute__((always_inline)) struct _B(mach_header) *find_mach_header_kmem_addr(task_t task, vm_offset_t addr);
__attribute__((always_inline)) struct _B(segment_command) *find_segment_command_kmem(struct _B(mach_header) *header, pointer_t cmds_buffer, const char *seg_name);
__attribute__((always_inline)) struct _B(section) *find_section_command_kmem(struct _B(segment_command) *seg_cmd, const char *sect_name);

/***/

#endif /* runtime_parsing_h */
