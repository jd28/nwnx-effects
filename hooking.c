#pragma once

#include <sys/mman.h>

#define PAGESIZE 4096

static void d_enable_write(unsigned long location)
{
	char *page;
	page = (char *) location;
	page = (char *)(((int) page + PAGESIZE - 1) & ~(PAGESIZE - 1));
	page -= PAGESIZE;

	if (mprotect(page, PAGESIZE, PROT_WRITE | PROT_READ | PROT_EXEC))
		perror("mprotect");
}

static inline void HookCall(DWORD src_addr, DWORD to_addr)
{
	d_enable_write(src_addr);
	*(DWORD *)(src_addr + 1) = to_addr - (src_addr + 5);
}
