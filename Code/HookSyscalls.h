#pragma once

#include <map>
#include "pin.H"

namespace W{
	#include "windows.h"
	#include "Winternl.h"
}

#include "ProcInfo.h"

#define MAXSYSCALLS 0x200	// this should be enough for Win7 (up to 419)

//--------------- HELPER DATA STRUCTURES --------------//

#define SYSTEM_PROCESS_INFORMATION 5
#define SYSTEM_HANDLE_INFORMATION 16

/* [Suppli] */
#define SYSTEM_KERNEL_DEBUGGER_NFORMATION 35
#define SYSTEM_FIRMWARE_TABLE_INFO 76

#define STATUS_PRIVILEGE_NOT_HELD ((W::NTSTATUS)0xC0000061L)
#define STATUS_PORT_NOT_SET ((W::NTSTATUS)0xC0000353L)
#define STATUS_HANDLE_NOT_CLOSABLE ((W::NTSTATUS)0xC0000235L)
#define STATUS_INSUFFICIENT_RESOURCES ((W::NTSTATUS)0xC000009AL)
#define PROCESS_DEBUG_INHERIT 0x00000001 // default for a non-debugged process
#define PROCESS_NO_DEBUG_INHERIT 0x00000002 // default for a debugged process
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER 0x00000004

#define TIMER 100 // TODO

#define RTL_CONSTANT_STRING(s) { sizeof(s) - sizeof((s)[0]), sizeof(s), (W::PWSTR)s }


typedef struct _PROCESS_BASIC_INFORMATION /* [Suppli] */
{
	W::NTSTATUS ExitStatus;
	W::PPEB PebBaseAddress;
	W::ULONG_PTR AffinityMask;
	W::LONG BasePriority;
	W::HANDLE UniqueProcessId;
	W::HANDLE InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

//informations returned by the NtSystemQueryInformation
typedef struct _SYSTEM_PROCESS_INFO
{
    W::ULONG                   NextEntryOffset;
    W::ULONG                   NumberOfThreads;
    W::LARGE_INTEGER           Reserved[3];
    W::LARGE_INTEGER           CreateTime;
    W::LARGE_INTEGER           UserTime;
    W::LARGE_INTEGER           KernelTime;
    W::UNICODE_STRING          ImageName;
    W::ULONG                   BasePriority;
    W::HANDLE                  ProcessId;
	W::HANDLE                  InheritedFromProcessId;
} SYSTEM_PROCESS_INFO, *PSYSTEM_PROCESS_INFO;

typedef struct _OBJECT_TYPE_INFORMATION { /* [Suppli] */
	W::UNICODE_STRING TypeName;
	W::ULONG TotalNumberOfHandles;
	W::ULONG TotalNumberOfObjects;
}OBJECT_TYPE_INFORMATION, *POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_ALL_INFORMATION { /* [Suppli] */
	W::ULONG NumberOfObjects;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
}OBJECT_ALL_INFORMATION, *POBJECT_ALL_INFORMATION;

typedef enum _OBJECT_INFORMATION_CLASS { /* [Suppli] */
	ObjectBasicInformation = 0,
	ObjectTypeInformation = 2
} OBJECT_INFORMATION_CLASS;

typedef enum _KEY_INFORMATION_CLASS {  /* [Suppli] */
	KeyBasicInformation = 0,
	KeyNodeInformation = 1,
	KeyFullInformation = 2,
	KeyNameInformation = 3,
	KeyCachedInformation = 4,
	KeyFlagsInformation = 5,
	KeyVirtualizationInformation = 6,
	KeyHandleTagsInformation = 7,
	MaxKeyInfoClass = 8
} KEY_INFORMATION_CLASS;

typedef enum _KEY_VALUE_INFORMATION_CLASS {  /* [Suppli] */
	KeyValueBasicInformation = 0,
	KeyValueFullInformation,
	KeyValuePartialInformation,
	KeyValueFullInformationAlign64,
	KeyValuePartialInformationAlign64,
	MaxKeyValueInfoClass
} KEY_VALUE_INFORMATION_CLASS;

typedef struct _SYSTEM_KERNEL_DEBUGGER_INFORMATION {  /* [Suppli] */
	W::BOOLEAN KernelDebuggerEnabled;
	W::BOOLEAN KernelDebuggerNotPresent;
} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

typedef enum _SYSTEM_FIRMWARE_TABLE_ACTION {  /* [Suppli] */
	SystemFirmwareTable_Enumerate,
	SystemFirmwareTable_Get
} SYSTEM_FIRMWARE_TABLE_ACTION, *PSYSTEM_FIRMWARE_TABLE_ACTION;

typedef struct _SYSTEM_FIRMWARE_TABLE_INFORMATION { /* [Suppli] */
	W::ULONG ProviderSignature;
	SYSTEM_FIRMWARE_TABLE_ACTION Action;
	W::ULONG TableID;
	W::ULONG TableBufferLength;
	W::UCHAR TableBuffer[ANYSIZE_ARRAY];
} SYSTEM_FIRMWARE_TABLE_INFORMATION, *PSYSTEM_FIRMWARE_TABLE_INFORMATION;

typedef struct _SYSTEM_HANDLE
{
    W::ULONG ProcessId;
    W::BYTE ObjectTypeNumber;
    W::BYTE Flags;
    W::USHORT Handle;
    W::PVOID Object;
    W::ACCESS_MASK GrantedAccess;
} SYSTEM_HANDLE, *PSYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    W::ULONG HandleCount;
    SYSTEM_HANDLE Handles[1];

} SYSTEM_HANDLE_INFORMATION_STRUCT, *PSYSTEM_HANDLE_INFORMATION_STRUCT;

//information about the process that the malware wants to open
typedef struct _CLIENT_ID
{
     W::PVOID UniqueProcess;
     W::PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

//function signature of our hook function
typedef void (* syscall_hook)(syscall_t *sc, CONTEXT *ctx, SYSCALL_STANDARD std);

//binding between syscall ordinal and hook to be executed
static syscall_hook sysEntryHooks[MAXSYSCALLS]; // entries NULL by default
static syscall_hook sysExitHooks[MAXSYSCALLS];

//binding between the ordinal of the syscall and the name of the syscall
static char* syscallIDs[MAXSYSCALLS];


enum _SYSTEM_INFORMATION_CLASS /* [Suppli] */
{
	SystemBasicInformation = 0x0000,
	SystemProcessorInformation = 0x0001,
	SystemPerformanceInformation = 0x0002,
	SystemTimeOfDayInformation = 0x0003,
	SystemPathInformation = 0x0004,
	SystemProcessInformation = 0x0005,
	SystemCallCountInformation = 0x0006,
	SystemDeviceInformation = 0x0007,
	SystemProcessorPerformanceInformation = 0x0008,
	SystemFlagsInformation = 0x0009,
	SystemCallTimeInformation = 0x000A,
	SystemModuleInformation = 0x000B,
	SystemLocksInformation = 0x000C,
	SystemStackTraceInformation = 0x000D,
	SystemPagedPoolInformation = 0x000E,
	SystemNonPagedPoolInformation = 0x000F,
	SystemHandleInformation = 0x0010,
	SystemObjectInformation = 0x0011,
	SystemPageFileInformation = 0x0012,
	SystemVdmInstemulInformation = 0x0013,
	SystemVdmBopInformation = 0x0014,
	SystemFileCacheInformation = 0x0015,
	SystemPoolTagInformation = 0x0016,
	SystemInterruptInformation = 0x0017,
	SystemDpcBehaviorInformation = 0x0018,
	SystemFullMemoryInformation = 0x0019,
	SystemLoadGdiDriverInformation = 0x001A,
	SystemUnloadGdiDriverInformation = 0x001B,
	SystemTimeAdjustmentInformation = 0x001C,
	SystemSummaryMemoryInformation = 0x001D,
	SystemMirrorMemoryInformation = 0x001E,
	SystemPerformanceTraceInformation = 0x001F,
	SystemCrashDumpInformation = 0x0020,
	SystemExceptionInformation = 0x0021,
	SystemCrashDumpStateInformation = 0x0022,
	SystemKernelDebuggerInformation = 0x0023,
	SystemContextSwitchInformation = 0x0024,
	SystemRegistryQuotaInformation = 0x0025,
	SystemExtendServiceTableInformation = 0x0026,
	SystemPrioritySeperation = 0x0027,
	SystemVerifierAddDriverInformation = 0x0028,
	SystemVerifierRemoveDriverInformation = 0x0029,
	SystemProcessorIdleInformation = 0x002A,
	SystemLegacyDriverInformation = 0x002B,
	SystemCurrentTimeZoneInformation = 0x002C,
	SystemLookasideInformation = 0x002D,
	SystemTimeSlipNotification = 0x002E,
	SystemSessionCreate = 0x002F,
	SystemSessionDetach = 0x0030,
	SystemSessionInformation = 0x0031,
	SystemRangeStartInformation = 0x0032,
	SystemVerifierInformation = 0x0033,
	SystemVerifierThunkExtend = 0x0034,
	SystemSessionProcessInformation = 0x0035,
	SystemLoadGdiDriverInSystemSpace = 0x0036,
	SystemNumaProcessorMap = 0x0037,
	SystemPrefetcherInformation = 0x0038,
	SystemExtendedProcessInformation = 0x0039,
	SystemRecommendedSharedDataAlignment = 0x003A,
	SystemComPlusPackage = 0x003B,
	SystemNumaAvailableMemory = 0x003C,
	SystemProcessorPowerInformation = 0x003D,
	SystemEmulationBasicInformation = 0x003E,
	SystemEmulationProcessorInformation = 0x003F,
	SystemExtendedHandleInformation = 0x0040,
	SystemLostDelayedWriteInformation = 0x0041,
	SystemBigPoolInformation = 0x0042,
	SystemSessionPoolTagInformation = 0x0043,
	SystemSessionMappedViewInformation = 0x0044,
	SystemHotpatchInformation = 0x0045,
	SystemObjectSecurityMode = 0x0046,
	SystemWatchdogTimerHandler = 0x0047,
	SystemWatchdogTimerInformation = 0x0048,
	SystemLogicalProcessorInformation = 0x0049,
	SystemWow64SharedInformationObsolete = 0x004A,
	SystemRegisterFirmwareTableInformationHandler = 0x004B,
	SystemFirmwareTableInformation = 0x004C,
	SystemModuleInformationEx = 0x004D,
	SystemVerifierTriageInformation = 0x004E,
	SystemSuperfetchInformation = 0x004F,
	SystemMemoryListInformation = 0x0050,
	SystemFileCacheInformationEx = 0x0051,
	SystemThreadPriorityClientIdInformation = 0x0052,
	SystemProcessorIdleCycleTimeInformation = 0x0053,
	SystemVerifierCancellationInformation = 0x0054,
	SystemProcessorPowerInformationEx = 0x0055,
	SystemRefTraceInformation = 0x0056,
	SystemSpecialPoolInformation = 0x0057,
	SystemProcessIdInformation = 0x0058,
	SystemErrorPortInformation = 0x0059,
	SystemBootEnvironmentInformation = 0x005A,
	SystemHypervisorInformation = 0x005B,
	SystemVerifierInformationEx = 0x005C,
	SystemTimeZoneInformation = 0x005D,
	SystemImageFileExecutionOptionsInformation = 0x005E,
	SystemCoverageInformation = 0x005F,
	SystemPrefetchPatchInformation = 0x0060,
	SystemVerifierFaultsInformation = 0x0061,
	SystemSystemPartitionInformation = 0x0062,
	SystemSystemDiskInformation = 0x0063,
	SystemProcessorPerformanceDistribution = 0x0064,
	SystemNumaProximityNodeInformation = 0x0065,
	SystemDynamicTimeZoneInformation = 0x0066,
	SystemCodeIntegrityInformation = 0x0067,
	SystemProcessorMicrocodeUpdateInformation = 0x0068,
	SystemProcessorBrandString = 0x0069,
	SystemVirtualAddressInformation = 0x006A,
	SystemLogicalProcessorAndGroupInformation = 0x006B,
	SystemProcessorCycleTimeInformation = 0x006C,
	SystemStoreInformation = 0x006D,
	SystemRegistryAppendString = 0x006E,
	SystemAitSamplingValue = 0x006F,
	SystemVhdBootInformation = 0x0070,
	SystemCpuQuotaInformation = 0x0071,
	SystemNativeBasicInformation = 0x0072,
	SystemErrorPortTimeouts = 0x0073,
	SystemLowPriorityIoInformation = 0x0074,
	SystemBootEntropyInformation = 0x0075,
	SystemVerifierCountersInformation = 0x0076,
	SystemPagedPoolInformationEx = 0x0077,
	SystemSystemPtesInformationEx = 0x0078,
	SystemNodeDistanceInformation = 0x0079,
	SystemAcpiAuditInformation = 0x007A,
	SystemBasicPerformanceInformation = 0x007B,
	SystemQueryPerformanceCounterInformation = 0x007C,
	SystemSessionBigPoolInformation = 0x007D,
	SystemBootGraphicsInformation = 0x007E,
	SystemScrubPhysicalMemoryInformation = 0x007F,
	SystemBadPageInformation = 0x0080,
	SystemProcessorProfileControlArea = 0x0081,
	SystemCombinePhysicalMemoryInformation = 0x0082,
	SystemEntropyInterruptTimingInformation = 0x0083,
	SystemConsoleInformation = 0x0084,
	SystemPlatformBinaryInformation = 0x0085,
	SystemThrottleNotificationInformation = 0x0086,
	SystemHypervisorProcessorCountInformation = 0x0087,
	SystemDeviceDataInformation = 0x0088,
	SystemDeviceDataEnumerationInformation = 0x0089,
	SystemMemoryTopologyInformation = 0x008A,
	SystemMemoryChannelInformation = 0x008B,
	SystemBootLogoInformation = 0x008C,
	SystemProcessorPerformanceInformationEx = 0x008D,
	SystemSpare0 = 0x008E,
	SystemSecureBootPolicyInformation = 0x008F,
	SystemPageFileInformationEx = 0x0090,
	SystemSecureBootInformation = 0x0091,
	SystemEntropyInterruptTimingRawInformation = 0x0092,
	SystemPortableWorkspaceEfiLauncherInformation = 0x0093,
	SystemFullProcessInformation = 0x0094,
	MaxSystemInfoClass = 0x0095
};

typedef enum _PROCESSINFOCLASS /* [Suppli] */
{
	ProcessBasicInformation, // q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
	ProcessQuotaLimits, // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
	ProcessIoCounters, // q: IO_COUNTERS
	ProcessVmCounters, // q: VM_COUNTERS, VM_COUNTERS_EX, VM_COUNTERS_EX2
	ProcessTimes, // q: KERNEL_USER_TIMES
	ProcessBasePriority, // s: KPRIORITY
	ProcessRaisePriority, // s: ULONG
	ProcessDebugPort, // q: HANDLE
	ProcessExceptionPort, // s: HANDLE
	ProcessAccessToken, // s: PROCESS_ACCESS_TOKEN
	ProcessLdtInformation, // qs: PROCESS_LDT_INFORMATION // 10
	ProcessLdtSize, // s: PROCESS_LDT_SIZE
	ProcessDefaultHardErrorMode, // qs: ULONG
	ProcessIoPortHandlers, // (kernel-mode only)
	ProcessPooledUsageAndLimits, // q: POOLED_USAGE_AND_LIMITS
	ProcessWorkingSetWatch, // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
	ProcessUserModeIOPL,
	ProcessEnableAlignmentFaultFixup, // s: BOOLEAN
	ProcessPriorityClass, // qs: PROCESS_PRIORITY_CLASS
	ProcessWx86Information,
	ProcessHandleCount, // q: ULONG, PROCESS_HANDLE_INFORMATION // 20
	ProcessAffinityMask, // s: KAFFINITY
	ProcessPriorityBoost, // qs: ULONG
	ProcessDeviceMap, // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
	ProcessSessionInformation, // q: PROCESS_SESSION_INFORMATION
	ProcessForegroundInformation, // s: PROCESS_FOREGROUND_BACKGROUND
	ProcessWow64Information, // q: ULONG_PTR
	ProcessImageFileName, // q: UNICODE_STRING
	ProcessLUIDDeviceMapsEnabled, // q: ULONG
	ProcessBreakOnTermination, // qs: ULONG
	ProcessDebugObjectHandle, // q: HANDLE // 30
	ProcessDebugFlags, // qs: ULONG
	ProcessHandleTracing, // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
	ProcessIoPriority, // qs: IO_PRIORITY_HINT
	ProcessExecuteFlags, // qs: ULONG
	ProcessResourceManagement,
	ProcessCookie, // q: ULONG
	ProcessImageInformation, // q: SECTION_IMAGE_INFORMATION
	ProcessCycleTime, // q: PROCESS_CYCLE_TIME_INFORMATION // since VISTA
	ProcessPagePriority, // q: ULONG
	ProcessInstrumentationCallback, // 40
	ProcessThreadStackAllocation, // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
	ProcessWorkingSetWatchEx, // q: PROCESS_WS_WATCH_INFORMATION_EX[]
	ProcessImageFileNameWin32, // q: UNICODE_STRING
	ProcessImageFileMapping, // q: HANDLE (input)
	ProcessAffinityUpdateMode, // qs: PROCESS_AFFINITY_UPDATE_MODE
	ProcessMemoryAllocationMode, // qs: PROCESS_MEMORY_ALLOCATION_MODE
	ProcessGroupInformation, // q: USHORT[]
	ProcessTokenVirtualizationEnabled, // s: ULONG
	ProcessConsoleHostProcess, // q: ULONG_PTR
	ProcessWindowInformation, // q: PROCESS_WINDOW_INFORMATION // 50
	ProcessHandleInformation, // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
	ProcessMitigationPolicy, // s: PROCESS_MITIGATION_POLICY_INFORMATION
	ProcessDynamicFunctionTableInformation,
	ProcessHandleCheckingMode,
	ProcessKeepAliveCount, // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
	ProcessRevokeFileHandles, // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
	ProcessWorkingSetControl, // s: PROCESS_WORKING_SET_CONTROL
	ProcessHandleTable, // since WINBLUE
	ProcessCheckStackExtentsMode,
	ProcessCommandLineInformation, // q: UNICODE_STRING // 60
	ProcessProtectionInformation, // q: PS_PROTECTION
	ProcessMemoryExhaustion, // PROCESS_MEMORY_EXHAUSTION_INFO // since THRESHOLD
	ProcessFaultInformation, // PROCESS_FAULT_INFORMATION
	ProcessTelemetryIdInformation, // PROCESS_TELEMETRY_ID_INFORMATION
	ProcessCommitReleaseInformation, // PROCESS_COMMIT_RELEASE_INFORMATION
	ProcessDefaultCpuSetsInformation,
	ProcessAllowedCpuSetsInformation,
	ProcessSubsystemProcess,
	ProcessJobMemoryInformation, // PROCESS_JOB_MEMORY_INFO
	ProcessInPrivate, // since THRESHOLD2 // 70
	ProcessRaiseUMExceptionOnInvalidHandleClose,
	ProcessIumChallengeResponse,
	ProcessChildProcessInformation, // PROCESS_CHILD_PROCESS_INFORMATION
	ProcessHighGraphicsPriorityInformation,
	ProcessSubsystemInformation, // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
	ProcessEnergyValues, // PROCESS_ENERGY_VALUES, PROCESS_EXTENDED_ENERGY_VALUES
	ProcessActivityThrottleState, // PROCESS_ACTIVITY_THROTTLE_STATE
	ProcessActivityThrottlePolicy, // PROCESS_ACTIVITY_THROTTLE_POLICY
	ProcessWin32kSyscallFilterInformation,
	ProcessDisableSystemAllowedCpuSets,
	ProcessWakeInformation, // PROCESS_WAKE_INFORMATION
	ProcessEnergyTrackingState, // PROCESS_ENERGY_TRACKING_STATE
	ProcessManageWritesToExecutableMemory, // MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
	ProcessCaptureTrustletLiveDump,
	ProcessTelemetryCoverage,
	ProcessEnclaveInformation,
	ProcessEnableReadWriteVmLogging, // PROCESS_READWRITEVM_LOGGING_INFORMATION
	ProcessUptimeInformation, // PROCESS_UPTIME_INFORMATION
	ProcessImageSection,
	MaxProcessInfoClass
} PROCESSINFOCLASS;

typedef struct _OBJECT_DIRECTORY_INFORMATION { /* [Suppli] */
	W::UNICODE_STRING Name;
	W::UNICODE_STRING TypeName;
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

typedef struct _OBJECT_ATTRIBUTES { /* [Suppli] */
	W::ULONG Length;
	W::HANDLE RootDirectory;
	W::PUNICODE_STRING ObjectName;
	W::ULONG Attributes;
	W::PVOID SecurityDescriptor;
	W::PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef struct _KEY_BASIC_INFORMATION { /* [Suppli] */
	W::LARGE_INTEGER LastWriteTime;
	W::ULONG         TitleIndex;
	W::ULONG         NameLength;
	W::WCHAR         Name[1];
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION { /* [Suppli] */
	W::ULONG TitleIndex;
	W::ULONG Type;
	W::ULONG DataLength;
	W::UCHAR Data[1];
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _OBJECT_HANDLE_FLAG_INFORMATION /* [Suppli] */
{
	W::BOOLEAN Inherit;
	W::BOOLEAN ProtectFromClose;
} OBJECT_HANDLE_FLAG_INFORMATION, *POBJECT_HANDLE_FLAG_INFORMATION;

typedef struct _RTL_PROCESS_MODULE_INFORMATION /* [Suppli] */
{
	W::HANDLE Section; // DCD was: HANDLE
	W::PVOID MappedBase;
	W::PVOID ImageBase;
	W::ULONG ImageSize;
	W::ULONG Flags;
	W::USHORT LoadOrderIndex;
	W::USHORT InitOrderIndex;
	W::USHORT LoadCount;
	W::USHORT OffsetToFileName;
	W::UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES /* [Suppli] */
{
	W::ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;




//--------------- END HELPER DATA STRUCTURES --------------//

class HookSyscalls
{
public:
	static void enumSyscalls();
	static void initHooks(syscall_t *callbackArray);

private:

	//Helpers
	static void syscallEntry(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, void *v);
	static void syscallExit(THREADID thread_id, CONTEXT *ctx, SYSCALL_STANDARD std, void *v);
	static void syscallGetArguments(CONTEXT *ctx, SYSCALL_STANDARD std, int count, ...);
	
	//DEBUG
	static void printArgs(syscall_t * sc);
	//static void printRegs(CONTEXT * ctx);
};

