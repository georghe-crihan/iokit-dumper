#ifdef KERNEL
int
kas_info(struct proc *p,
			  struct kas_info_args *uap,
			  int *retval __unused)
{
#ifdef SECURE_KERNEL
	(void)p;
	(void)uap;
	return ENOTSUP;
#else /* !SECURE_KERNEL */
	int			selector = uap->selector;
	user_addr_t	valuep = uap->value;
	user_addr_t	sizep = uap->size;
	user_size_t size;
	int			error;

	if (!kauth_cred_issuser(kauth_cred_get())) {
		return EPERM;
	}

#if CONFIG_MACF
	error = mac_system_check_kas_info(kauth_cred_get(), selector);
	if (error) {
		return error;
	}
#endif

	if (IS_64BIT_PROCESS(p)) {
		user64_size_t size64;
		error = copyin(sizep, &size64, sizeof(size64));
		size = (user_size_t)size64;
	} else {
		user32_size_t size32;
		error = copyin(sizep, &size32, sizeof(size32));
		size = (user_size_t)size32;
	}
	if (error) {
		return error;
	}

	switch (selector) {
		case KAS_INFO_KERNEL_TEXT_SLIDE_SELECTOR:
			{
				uint64_t slide = vm_kernel_slide;

				if (sizeof(slide) != size) {
					return EINVAL;
				}
				
				if (IS_64BIT_PROCESS(p)) {
					user64_size_t size64 = (user64_size_t)size;
					error = copyout(&size64, sizep, sizeof(size64));
				} else {
					user32_size_t size32 = (user32_size_t)size;
					error = copyout(&size32, sizep, sizeof(size32));
				}
				if (error) {
					return error;
				}
				
				error = copyout(&slide, valuep, sizeof(slide));
				if (error) {
					return error;
				}
			}
			break;
		default:
			return EINVAL;
	}

	return 0;
#endif /* !SECURE_KERNEL */
}
#endif
