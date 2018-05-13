// Shamelessly stolen from here:
// https://opensource.apple.com/source/kext_tools/kext_tools-59/kextstat_main.c

#ifndef _x86_64_
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>

#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/mach_host.h>

static const char * progname = "iokit-dumper";

static int kmod_compare(const void * a, const void * b);
static int kmod_ref_compare(const void * a, const void * b);

vm_offset_t KextUnslidBaseAddress(const char *kext_id)
{
    int exit_code = 0;
    int optchar = 0;
    kern_return_t mach_result = KERN_SUCCESS;
    host_name_port_t host_port = MACH_PORT_NULL;
    kmod_info_t * kmod_list = NULL;
    unsigned int kmod_bytecount;  // not really used
    int kmod_count;
    kmod_info_t *this_kmod;
    kmod_reference_t * kmod_ref;
    int ref_count;
    vm_offset_t this_kmod_address;
    int i, j;

    int skip_kernel_comps = 0; // -k
	
   /* Get the list of loaded kmods from the kernel.
    */
    host_port = mach_host_self();
    mach_result = kmod_get_info(host_port, (void *)&kmod_list,
        &kmod_bytecount);
    if (mach_result != KERN_SUCCESS) {
        fprintf(stderr,
            "%s: couldn't get list of loaded kexts from kernel - %s\n",
            "iokit-dumper", mach_error_string(mach_result));
        exit_code = 1;
        goto finish;
    }

   /* kmod_get_info() doesn't return a proper count so we have
    * to scan the array checking for a NULL next pointer.
    */
    this_kmod = kmod_list;
    kmod_count = 0;
    while (this_kmod) {
        kmod_count++;
        this_kmod = (this_kmod->next) ? (this_kmod + 1) : 0;
    }

   /* rebuild the reference lists from their serialized pileup
    * after the list of kmod_info_t structs.
    */
    this_kmod = kmod_list;
    kmod_ref = (kmod_reference_t *)(kmod_list + kmod_count);
    while (this_kmod) {

       /* How many refs does this kmod have? Again, kmod_get_info ovverrides
        * a field. Here what is the actual reference list in the kernel becomes
        * the count of references tacked onto the end of the kmod_info_t list.
        */
        ref_count = (int)this_kmod->reference_list;
        if (ref_count) {
            this_kmod->reference_list = kmod_ref;

            for (i = 0; i < ref_count; i++) {
                int foundit = 0;
                for (j = 0; j < kmod_count; j++) {
                   /* kmod_get_info() made each kmod_info_t struct's .next field
                    * point to itself IN KERNEL SPACE, so this is a sort of id
                    * for the reference list. Here we replace the ref's
                    * info field, a here-useless KERNEL SPACE ADDRESS,
                    * with the list id of the kmod_info_t struct.
                    * Gross, gross hack.
                    */
                    if (kmod_ref->info == kmod_list[j].next) {
                        kmod_ref->info = (kmod_info_t *)kmod_list[j].id;
                        foundit++;
                        break;
                    }
                }

               /* If we didn't find it, that's because the last entry's next
                * pointer is SET TO ZERO to signal the end of the kmod_info_t
                * list, even though the same field is used for other purposes
                * in every other entry in the list. So set the ref's info
                * field to the id of the last entry in the list.
                */
                if (!foundit) {
                    kmod_ref->info =
                        (kmod_info_t *)kmod_list[kmod_count - 1].id;
                }

                kmod_ref++;
            }

           /* Sort the references in descending order of reference index.
            */
            qsort(this_kmod->reference_list, ref_count,
                  sizeof(kmod_reference_t), kmod_ref_compare);

           /* Patch up the links between ref structs and move on to the
            * next one.
            */
            for (i = 0; i < ref_count - 1; i++) {
                this_kmod->reference_list[i].next =
                    &this_kmod->reference_list[i+1];
            }
            this_kmod->reference_list[ref_count - 1].next = 0;
        }
        this_kmod  = (this_kmod->next) ? (this_kmod + 1) : 0;
    }

    if (!kmod_count) {
        goto finish;
    }

    qsort(kmod_list, kmod_count, sizeof(kmod_info_t), kmod_compare);

   /* The user just wants to know about a particular kext, fiddle with
    * the sorted list so that it only contains the desired one. This is
    * modifying the list as it scans, but in a way that guarantees the
    * modifications don't catch up to the scan location.
    */
    {
        this_kmod = &kmod_list[0];
        int match_count = 0;
        for (i = 0; i < kmod_count; i++, this_kmod++) {
            if (this_kmod->name && !strcmp(this_kmod->name, kext_id)) {
                kmod_list[match_count++] = *this_kmod;
				this_kmod_address = this_kmod->address;
				goto finish;
            }
        }
        kmod_count = match_count;
    }

finish:

   /* Dispose of the host port to prevent security breaches and port
    * leaks. We don't care about the kern_return_t value of this
    * call for now as there's nothing we can do if it fails.
    */
    if (MACH_PORT_NULL != host_port) {
        mach_port_deallocate(mach_task_self(), host_port);
    }

    if (kmod_list != NULL) {
        vm_deallocate(mach_task_self(), (vm_address_t)kmod_list,
            kmod_bytecount);
    }
	return this_kmod_address;
}

static int kmod_compare(const void * a, const void * b)
{
    kmod_info_t * k1 = (kmod_info_t *)a;
    kmod_info_t * k2 = (kmod_info_t *)b;
    // these are load indices, not CFBundleIdentifiers
    return (k1->id - k2->id);
}

static int kmod_ref_compare(const void * a, const void * b)
{
    kmod_reference_t * r1 = (kmod_reference_t *)a;
    kmod_reference_t * r2 = (kmod_reference_t *)b;
    // these are load indices, not CFBundleIdentifiers
    // sorting high-low.
    return ((int)r2->info - (int)r1->info);
}
#endif
