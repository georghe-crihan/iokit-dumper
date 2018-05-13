//
//  helper.h
//  tree-parser-priv-final
//
//  Created by jndok on 29/12/15.
//  Copyright © 2015 jndok. All rights reserved.
//

#ifndef helper_h
#define helper_h

#ifdef _x86_64_
#define _B(f) f##_64
#else
#define _B(f) f
#endif

#define __dbg(msg, ...) fprintf(stderr, "[_%s]: " msg "\n", __func__, ##__VA_ARGS__)

#define DO_TIMES(x)             for(uint32_t __cnt=0; __cnt<x; ++__cnt)
#define DO_TIMES_COUNT(x, inc)  for(uint32_t __cnt=0; __cnt<x; __cnt+=inc)

#ifdef _x86_64_
#define PRINT_POINTERS(n, buffer) {   \
    for(uint32_t i=0; i<n*8; i+=8) {   \
        printf("%#llx\n", *(vm_offset_t*)(buffer+i));  \
    }   \
}

#define PRINT_UNSLID_POINTERS(n, buffer, slide) {   \
    for(uint32_t i=0; i<n*8; i+=8) {   \
        printf("%#llx --> %#llx\n", *(vm_offset_t*)(buffer+i), (*(vm_offset_t*)(buffer+i))-kslide);  \
    }   \
}
#else
#define PRINT_POINTERS(n, buffer) {   \
    for(uint32_t i=0; i<n*4; i+=4) {   \
        printf("%#llx\n", *(vm_offset_t*)(buffer+i));  \
    }   \
}

#define PRINT_UNSLID_POINTERS(n, buffer, slide) {   \
    for(uint32_t i=0; i<n*4; i+=4) {   \
        printf("%#llx --> %#llx\n", *(vm_offset_t*)(buffer+i), (*(vm_offset_t*)(buffer+i))-kslide);  \
    }   \
}
#endif

#define PRINT_BYTES(n, buffer) {    \
    DO_TIMES(n) {   \
        printf("%#02x\n", *(char*)(buffer+__cnt) & 0x000000ff);  \
    }   \
}

#endif /* helper_h */
