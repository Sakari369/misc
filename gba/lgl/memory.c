#include "memory.h"
#include "registers.h"
#include "dma.h"

void memSet(void *dst, int c, int len)
{
	int i;
	u16 *dst_ptr = (u16 *)dst;

	for (i=0; i<len; i++)
		dst_ptr[i] = c;
}

void memCopy(void *dst, const void *src, int len)
{
	int i;

	u16 *src_ptr = (u16 *)src;
	u16 *dst_ptr = (u16 *)dst;

	for (i=0; i<len; i++)
		dst_ptr[i] = src_ptr[i];
}

void dmaCopy(void *dst, const void *src, int len)
{
	/* Shut off previous transfer */
	R32_CTRL_DMA3 = 0;
	/* Set source and dest and start copy */
	R32_SRC_DMA3 = (u32)src;
	R32_DST_DMA3 = (u32)dst;
	R32_CTRL_DMA3 = (len | (DMA_ENABLE | DMA_CS32 | DMA_START_NOW));
}
