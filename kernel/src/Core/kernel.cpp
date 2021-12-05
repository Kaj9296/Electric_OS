#include "kernelUtil.h"

extern "C" void KernelMain(BootLoaderInfo* BootInfo)
{
	Renderer::Init(BootInfo->ScreenBuffer, BootInfo->PSF1Font);
	PageFrameAllocator::Init(BootInfo->MemoryMap, BootInfo->MemoryMapSize, BootInfo->MemoryMapDescSize);
	PageFrameAllocator::LockPages(&_KernelStart, ((uint64_t)&_KernelEnd - (uint64_t)&_KernelStart) / 4096 + 1);
	PageFrameAllocator::LockPages(BootInfo->ScreenBuffer->Base, BootInfo->ScreenBuffer->Size / 0x1000 + 1);
	PageTableManager::Init(BootInfo->ScreenBuffer);

	Renderer::Clear();

	Renderer::Print("Hello, World!");

	while(true)
	{
		asm("hlt");
	}
}
