#include <stdio.h>
void init_fs(void) {
	return ;
}

void init_fs_with_mem(unsigned long memsize) {
	return ;
}

void shutdown_fs(void) {
	return ;
}

void __attribute__((weak)) make_digest_request_sync(int nr_digest)
{
    printf("[sekwonlee_warning] sync\n");
	return ;
}

int make_digest_request_async(int percent)
{
    printf("[sekwonlee_warning] async\n");
	return 0;
}

void wait_on_digesting(void)
{
	return ;
}

