#include "kernel.h"

/// <summary>
/// Compiler definitions.
/// </summary>
void __stack_chk_fail(void)
{

}
void* __dso_handle = (void*) &__dso_handle;
void* __cxa_atexit = (void*) &__cxa_atexit;

/// <summary>
/// The function called by the bootloader when it leaves boot services.
/// </summary>
extern "C" void KernelMain(BootLoaderInfo* BootInfo)
{
	InitGDT();

	//Runtime services setup.
	UEFI::Init(BootInfo->RT);

	//Heap setup.
	PageAllocator::Init(BootInfo->MemoryMap, BootInfo->ScreenBuffer);
	PageTableManager::Init(BootInfo->ScreenBuffer);
	Heap::Init();

	//Font setup.
	STL::SetFonts(BootInfo->PSFFonts, BootInfo->FontAmount);
	Renderer::Init(BootInfo->ScreenBuffer);

	//Interrupt setup.
	PIT::SetFrequency(100);
	RTC::Update();
	IDT::SetupInterrupts();
	
	//AHCI setup.
	ACPI::Init(BootInfo->RSDP);
	PCI::Init();
	AHCI::Init();

	//Process start.
	Compositor::Init(BootInfo->ScreenBuffer);
	ProcessHandler::Loop();

	Debug::Error("End of kernel code reached");

	//Catch to make sure control never returns to the bootloader.
	while(true)
	{
		asm("HLT");
	}
}
