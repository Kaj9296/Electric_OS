#include "STL/String/cstr.h"
#include "STL/Graphics/Graphics.h"

#include "Kernel/Renderer/Renderer.h"
#include "Kernel/Interrupts/IDT.h"
#include "Kernel/Memory/GDT/GDT.h"
#include "Kernel/Memory/Heap.h"
#include "Kernel/Memory/Paging/PageAllocator.h"
#include "Kernel/Memory/Paging/PageTable.h"
#include "Kernel/Input/KeyBoard.h"
#include "Kernel/Input/Mouse.h"
#include "Kernel/PIT/PIT.h"
#include "Kernel/RTC/RTC.h"
#include "Kernel/Debug/Debug.h"
#include "Kernel/System/System.h"
#include "Kernel/ProcessHandler/ProcessHandler.h"
#include "Kernel/ProcessHandler/Compositor.h"
#include "Kernel/ACPI/ACPI.h"
#include "Kernel/AHCI/AHCI.h"
#include "Kernel/PCI/PCI.h"
#include "Kernel/UEFI/UEFI.h"

/// <summary>
/// Compiler definitions.
/// </summary>
extern "C" void __stack_chk_fail(void);
extern "C" void* __dso_handle;
extern "C" void* __cxa_atexit;

/// <summary>
/// The struct passed to the kernel from the bootloader.
/// </summary>
struct BootLoaderInfo
{
	STL::Framebuffer* ScreenBuffer;
	STL::PSF_FONT** PSFFonts;
	uint8_t FontAmount;
	EFI_MEMORY_MAP* MemoryMap;
	RSDP2* RSDP;
	EfiRuntimeServices* RT;
};

/// <summary>
/// The first and last address used by the kernel.
/// </summary>
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;
