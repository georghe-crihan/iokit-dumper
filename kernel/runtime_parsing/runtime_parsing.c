//
//  runtime_parsing.c
//  tree-parser-priv-final
//
//  Created by jndok on 29/12/15.
//  Copyright © 2015 jndok. All rights reserved.
//

#include "xtypes.h" 
#include "runtime_parsing.h"

__attribute__((always_inline)) struct _B(mach_header) *find_mach_header_kmem_addr(task_t task, xvm_offset_t addr)
{
    struct _B(mach_header) *header = (struct _B(mach_header) *)read_kernel_memory(task, addr, sizeof(struct _B(mach_header)));
    if (header->magic != _B(MH_MAGIC)) {
        return NULL;
    }
    
    return header;
}

__attribute__((always_inline)) struct _B(segment_command) *find_segment_command_kmem(struct _B(mach_header) *header, pointer_t cmds_buffer, const char *seg_name)
{
    if (!header) {
        return NULL;
    }
    
    struct load_command *lcmd=(struct load_command*)cmds_buffer;
    for (uint32_t i=0; i<header->ncmds; ++i) {
        if (lcmd->cmd == _B(LC_SEGMENT)) {
            struct _B(segment_command) *seg_cmd=(struct _B(segment_command)*)lcmd;
            
            if (strcmp(seg_cmd->segname, seg_name) == 0) {
                return seg_cmd;
            }
        }
        
        lcmd = ((void*)lcmd + lcmd->cmdsize);
    }
    return NULL;
}

__attribute__((always_inline)) struct _B(section) *find_section_command_kmem(struct _B(segment_command) *seg_cmd, const char *sect_name)
{
    if (!seg_cmd) {
        return NULL;
    }
    
    struct _B(section) *sect_cmd = (struct _B(section)*)((void*)seg_cmd + sizeof(struct _B(segment_command)));
    for (uint32_t i=0; i<seg_cmd->nsects; ++i) {
        if (strcmp(sect_cmd->sectname, sect_name) == 0) {
            return sect_cmd;
        }
        sect_cmd = ((void*)sect_cmd + sizeof(struct _B(section)));
    }
    return NULL;
}
