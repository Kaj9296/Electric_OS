#pragma once

#include <stdint.h>

#define EFIAPI __attribute__((ms_abi))

enum class EfiResetType 
{
	Cold,
	Warm,
	Shutdown
};

struct EfiTableHeader 
{
	uint64_t Signature;
	uint32_t Rev;
	uint32_t Size;
	uint32_t Crc;
	uint32_t Reserved;
} __attribute__((packed));

struct EFI_GUID
{          
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8]; 
} __attribute__((packed));

struct EFI_CAPSULE_HEADER
{
    EFI_GUID CapsuleGuid;
    uint32_t HeaderSize;
    uint32_t Flags;
    uint32_t CapsuleImageSize;
} __attribute__((packed));

typedef uint64_t EFI_STATUS;
typedef uint64_t EFI_PHYSICAL_ADDRESS;

typedef EFI_STATUS(EFIAPI *EfGetTime)
(
    void* Time,
    void* Capabilities
);

typedef EFI_STATUS(EFIAPI *EfiSetTime) 
(
    void* Time
);

typedef EFI_STATUS(EFIAPI *EfiGetWakeUpTime) 
(
    bool* Enabled,
    bool* Pending,
    void* Time
);

typedef EFI_STATUS(EFIAPI *EfiSetWakeUpTime) 
(
	bool Enable,
	void* Time
);

typedef EFI_STATUS(EFIAPI *EfiSetVirtualAddressMap) 
(
    uint64_t MemoryMapSize,
    uint64_t DescriptorSize,
    uint32_t DescriptorVersion,
    void* VirtualMap
);

typedef EFI_STATUS(EFIAPI *EfiConvertPointer) 
(
    uint64_t DebugDisposition,
    void** Address
);

typedef EFI_STATUS(EFIAPI *EfiGetVariable) 
(
    uint16_t* VariableName,
    EFI_GUID* VendorGuid,
    uint32_t* Attributes,
    uint64_t* DataSize,
    void* Data
);

typedef EFI_STATUS(EFIAPI *EfiGetNextVariableName) 
(
    uint64_t* VariableNameSize,
    uint16_t* VariableName,
    EFI_GUID* VendorGuid
);

typedef EFI_STATUS(EFIAPI *EfiSetVariable) 
(
    uint16_t* VariableName,
    EFI_GUID* VendorGuid,
    uint32_t Attributes,
    uint64_t DataSize,
    void* Data
);

typedef EFI_STATUS(EFIAPI *EfiGetNextHighMonotonicCount) 
(
    uint64_t* Count
);

typedef EFI_STATUS(EFIAPI *EfiResetSystem)
(
	EfiResetType ResetType, 
	uint64_t ResetStatus, 
	uint64_t DataSize,
	uint16_t* ResetData
);

typedef EFI_STATUS(EFIAPI *EfiUpdateCapsule) 
(
    EFI_CAPSULE_HEADER** CapsuleHeaderArray,
    uint64_t CapsuleCount,
    EFI_PHYSICAL_ADDRESS ScatterGatherList
);

typedef EFI_STATUS(EFIAPI *EfiQueryCapsuleCapabilities) 
(
    EFI_CAPSULE_HEADER** CapsuleHeaderArray,
    uint64_t CapsuleCount,
    uint64_t* MaximumCapsuleSize,
    EfiResetType* ResetType
);

typedef EFI_STATUS(EFIAPI *EfiQueryVariableInfo) 
(
    uint32_t Attributes,
    uint64_t* MaximumVariableStorageSize,
    uint64_t* RemainingVariableStorageSize,
    uint64_t* MaximumVariableSize
);

struct EfiRuntimeServices 
{
	EfiTableHeader Header;
	EfGetTime* GetTime;
	EfiSetTime* SetTime;
	EfiGetWakeUpTime* GetWakeupTime;
	EfiSetWakeUpTime* SetWakeupTime;
	EfiSetVirtualAddressMap* SetVirtualAddressMap;
	EfiConvertPointer* ConvertPointer;
	EfiGetVariable* GetVariable;
	EfiGetNextVariableName* GetNextVariableName;
	EfiSetVariable* SetVariable;
	EfiGetNextHighMonotonicCount* GetNextHighMonotonicCount;
	EfiResetSystem ResetSystem;
	EfiUpdateCapsule* UpdateCapsule;
	EfiQueryCapsuleCapabilities* QueryCapsuleCapabilities;
	EfiQueryVariableInfo* QueryVariableInfo;
} __attribute__((packed));

namespace UEFI
{
    void Init(EfiRuntimeServices* In_RT);

    EfiRuntimeServices* GetRT();    
}