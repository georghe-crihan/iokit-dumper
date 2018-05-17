//
//  kernel.h
//  tree-parser-priv-final
//
//  Created by jndok on 29/12/15.
//  Copyright © 2015 jndok. All rights reserved.
//

#ifndef kernel_h
#define kernel_h

#define KAS_INFO_KERNEL_TEXT_SLIDE_SELECTOR (0)
#define KAS_INFO_MAX_SELECTOR (1)

#define SECT_CONST "__const"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>

#include "../helper.h"

#define SLIDE_POINTER(pointer)      (pointer)+kslide
#define UNSLIDE_POINTER(pointer)    (pointer)-kslide

#define IS_VALID_POINTER(pointer, start_addr, end_addr) ((pointer) >= (start_addr) && (pointer) <= (end_addr))

typedef struct MetaClass_Object {
    xvm_offset_t VTable_ptr;
    xvm_offset_t __unknown;
    xvm_offset_t Parent_gMetaClass_ptr;
    xvm_offset_t OSSymbol_ptr;
    uint32_t class_size;
} MetaClass_Object_t;

typedef struct OSSymbol_Object {
    xvm_offset_t VTable_ptr;
    xvm_offset_t __padding;
    xvm_offset_t name_length;
    xvm_offset_t name_ptr;
} OSSymbol_Object_t;

/***/

xvm_offset_t get_kslide(void);

mach_port_t task_for_pid_workaround(int Pid);
mach_port_t get_kernel_task(void);

__attribute__((always_inline)) pointer_t read_kernel_memory(vm_map_t task, xvm_offset_t addr, mach_vm_size_t size);
__attribute__((always_inline)) void read_kernel_memory_in_buffer(task_t task, xvm_offset_t addr, uint32_t size, void *buffer);

/***/

#endif /* kernel_h */
