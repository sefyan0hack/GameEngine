#include <core/Log.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
#include <dbghelp.h>
#ifdef DEBUG
std::string resolveSymbol(void* addr, HANDLE proc) {

    SymInitialize(proc, nullptr, TRUE);
    
    char buffer[sizeof(SYMBOL_INFO) + 256 * sizeof(char)];
    SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;

    DWORD64 displacement = 0;
    if (SymFromAddr(proc, reinterpret_cast<DWORD64>(addr), &displacement, symbol)) {
        return std::string(symbol->Name);
    }
    return std::string("<unknown>");
}

inline auto EXCEPTION_RECORD_to_str(const DWORD& exr) -> const char*{
    switch(exr){
        case(STILL_ACTIVE): return "STILL_ACTIVE"; break;
        case(EXCEPTION_ACCESS_VIOLATION): return "EXCEPTION_ACCESS_VIOLATION"; break;
        case(EXCEPTION_DATATYPE_MISALIGNMENT): return "EXCEPTION_DATATYPE_MISALIGNMENT"; break;
        case(EXCEPTION_BREAKPOINT): return "EXCEPTION_BREAKPOINT"; break;
        case(EXCEPTION_SINGLE_STEP): return "EXCEPTION_SINGLE_STEP"; break;
        case(EXCEPTION_ARRAY_BOUNDS_EXCEEDED): return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED"; break;
        case(EXCEPTION_FLT_DENORMAL_OPERAND): return "EXCEPTION_FLT_DENORMAL_OPERAND"; break;
        case(EXCEPTION_FLT_DIVIDE_BY_ZERO): return "EXCEPTION_FLT_DIVIDE_BY_ZERO"; break;
        case(EXCEPTION_FLT_INEXACT_RESULT): return "EXCEPTION_FLT_INEXACT_RESULT"; break;
        case(EXCEPTION_FLT_INVALID_OPERATION): return "EXCEPTION_FLT_INVALID_OPERATION"; break;
        case(EXCEPTION_FLT_OVERFLOW): return "EXCEPTION_FLT_OVERFLOW"; break;
        case(EXCEPTION_FLT_STACK_CHECK): return "EXCEPTION_FLT_STACK_CHECK"; break;
        case(EXCEPTION_FLT_UNDERFLOW): return "EXCEPTION_FLT_UNDERFLOW"; break;
        case(EXCEPTION_INT_DIVIDE_BY_ZERO): return "EXCEPTION_INT_DIVIDE_BY_ZERO"; break;
        case(EXCEPTION_INT_OVERFLOW): return "EXCEPTION_INT_OVERFLOW"; break;
        case(EXCEPTION_PRIV_INSTRUCTION): return "EXCEPTION_PRIV_INSTRUCTION"; break;
        case(EXCEPTION_IN_PAGE_ERROR): return "EXCEPTION_IN_PAGE_ERROR"; break;
        case(EXCEPTION_ILLEGAL_INSTRUCTION): return "EXCEPTION_ILLEGAL_INSTRUCTION"; break;
        case(EXCEPTION_NONCONTINUABLE_EXCEPTION): return "EXCEPTION_NONCONTINUABLE_EXCEPTION"; break;
        case(EXCEPTION_STACK_OVERFLOW): return "EXCEPTION_STACK_OVERFLOW"; break;
        case(EXCEPTION_INVALID_DISPOSITION): return "EXCEPTION_INVALID_DISPOSITION"; break;
        case(EXCEPTION_GUARD_PAGE): return "EXCEPTION_GUARD_PAGE"; break;
        case(EXCEPTION_INVALID_HANDLE): return "EXCEPTION_INVALID_HANDLE"; break;
        case(CONTROL_C_EXIT): return "CONTROL_C_EXIT"; break;
        default: return "<unknown>";
    }
}
auto PrintStackTrace(unsigned short skip) -> void
{
    constexpr ULONG maxFrames = 64;
    DWORD skipFtames = 2 + skip;

    PVOID stack[maxFrames];
    
    USHORT framesCaptured = RtlCaptureStackBackTrace(skipFtames, maxFrames, stack, NULL);
    auto process = GetCurrentProcess();

    std::string msg = std::format("Captured {} stack frames:\n", framesCaptured);

    for (USHORT i = 0; i < framesCaptured; i++) {
        auto addr = stack[i];

        std::string modulename = "<unknown>";
        std::string fname = "<unknown>";
        std::string file = "file";
        DWORD line = 0;
        
        auto moduleBase = SymGetModuleBase64(process, reinterpret_cast<DWORD64>(addr));
        char buffer[MAX_PATH];
        if (GetModuleFileNameA(reinterpret_cast<HMODULE>(moduleBase), buffer, MAX_PATH)) {
            auto path = std::filesystem::path(buffer);
            modulename = path.lexically_relative(std::filesystem::current_path()).string();
        }
        
        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = {};
        PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        DWORD64 displacement = 0;
        SymInitialize(process, NULL, TRUE);
        
        DWORD lineDisplacement = 0;
        if (SymFromAddr(process,  reinterpret_cast<DWORD64>(addr), &displacement, pSymbol)) {
            fname = pSymbol->Name;

            IMAGEHLP_LINE64 lineInfo{};
            lineInfo.SizeOfStruct = sizeof(lineInfo);

            if (SymGetLineFromAddr64(process, reinterpret_cast<DWORD64>(addr), &lineDisplacement, &lineInfo)) {
                auto path = std::filesystem::path(lineInfo.FileName);
                auto strpath = path.lexically_relative(std::filesystem::current_path()).string();
                file = !strpath.empty() ? strpath : lineInfo.FileName;
                line = lineInfo.LineNumber;
            }
        }
        msg += std::format("{:p} {}+{:#x} -> {}:{} `{}` \n", addr, fname, lineDisplacement, file, line, modulename);
    }
    std::cerr << msg;
    MessageBox(GetForegroundWindow(), msg.c_str(), "Stack Trace", MB_OK | MB_ICONWARNING);
}
namespace {

auto PrintStackTracectx(CONTEXT* context) -> void {
    HANDLE process = GetCurrentProcess();
    
    STACKFRAME64 stackFrame{};
    DWORD machineType = 0;

    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    #ifdef _M_IX86
        machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = context->Eip;   
        stackFrame.AddrStack.Offset = context->Esp;
        stackFrame.AddrFrame.Offset = context->Ebp;
    #elif _M_X64
        machineType = IMAGE_FILE_MACHINE_AMD64;
        stackFrame.AddrPC.Offset = context->Rip;
        stackFrame.AddrStack.Offset = context->Rsp;
        stackFrame.AddrFrame.Offset = context->Rbp;
    #endif

    std::string msg;
    const int maxFrames = 64;
    for (int frameNum = 0; frameNum < maxFrames; ++frameNum) {
        BOOL result = StackWalk64(
            machineType,
            process,
            GetCurrentThread(),
            &stackFrame,
            context,
            nullptr,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            nullptr);

        auto addr = stackFrame.AddrPC.Offset;
        std::string modulename = "<unknown>";
        std::string fname = "<unknown>";
        std::string file = "file";
        DWORD line = 0;

        if (!result){
            break;
        }

        char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)] = {};
        PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;
        DWORD64 displacement = 0;
        SymInitialize(process, NULL, TRUE);
        
        DWORD lineDisplacement = 0;
        if (SymFromAddr(process, addr, &displacement, pSymbol)) {
            fname = pSymbol->Name;

            IMAGEHLP_LINE64 lineInfo{};
            lineInfo.SizeOfStruct = sizeof(lineInfo);

            if (SymGetLineFromAddr64(process, addr, &lineDisplacement, &lineInfo)) {
                auto path = std::filesystem::path(lineInfo.FileName);
                auto strpath = path.lexically_relative(std::filesystem::current_path()).string();
                file =  !strpath.empty() ? strpath : lineInfo.FileName;
                line = lineInfo.LineNumber;
            }
        }

        auto moduleBase = SymGetModuleBase64(process, addr);
        char buffer[MAX_PATH];
        if (GetModuleFileNameA(reinterpret_cast<HMODULE>(moduleBase), buffer, MAX_PATH)) {
            auto path = std::filesystem::path(buffer);
            modulename = path.lexically_relative(std::filesystem::current_path()).string();
        }

        msg += std::format("{:#x} {}+{:#x} -> {}:{} `{}` \n", addr, fname, lineDisplacement, file, line, modulename);
    }

    std::cerr << msg;
    MessageBox(GetForegroundWindow(), msg.c_str(), "Stack Trace", MB_OK | MB_ICONWARNING);
}

auto WINAPI ExceptionHandler([[maybe_unused]] PEXCEPTION_POINTERS ex) -> LONG
{
    constexpr DWORD MS_CPP_EXCEPTION = 0xE06D7363; // C++ exception code
    const auto& exr = ex->ExceptionRecord;
    const auto& excode = exr->ExceptionCode;
    
    // Always allow C++ exceptions to propagate
    if (excode == MS_CPP_EXCEPTION) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    static constexpr std::array critical_exceptions = {
        EXCEPTION_ACCESS_VIOLATION,
        EXCEPTION_ILLEGAL_INSTRUCTION,
        EXCEPTION_STACK_OVERFLOW,
        EXCEPTION_IN_PAGE_ERROR,
        EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
        EXCEPTION_DATATYPE_MISALIGNMENT,
        EXCEPTION_FLT_DENORMAL_OPERAND,
        EXCEPTION_FLT_DIVIDE_BY_ZERO,
        EXCEPTION_FLT_INEXACT_RESULT,
        EXCEPTION_FLT_INVALID_OPERATION,
        EXCEPTION_FLT_OVERFLOW,
        EXCEPTION_FLT_STACK_CHECK,
        EXCEPTION_FLT_UNDERFLOW,
        EXCEPTION_INT_DIVIDE_BY_ZERO,
        EXCEPTION_INT_OVERFLOW,
        EXCEPTION_PRIV_INSTRUCTION
    };

    switch (excode) {
        case 0x3FFA000A: // Custom RendeDoc code
        case EXCEPTION_BREAKPOINT:
        case EXCEPTION_SINGLE_STEP:
            return EXCEPTION_CONTINUE_EXECUTION;
    }

    if (std::find(critical_exceptions.begin(), critical_exceptions.end(), excode) != critical_exceptions.end()) 
    {
        const auto& exaddr = exr->ExceptionAddress;
        auto symName = resolveSymbol(exaddr);
        
        Info("--- Exception Caught ---");
        Info("Exception {} (0x{:x})", EXCEPTION_RECORD_to_str(excode), excode);
        Info("Exception  [{}] - {}", exaddr, symName);
        Info("Exception Flags: {:#x}", exr->ExceptionFlags);
        Info("Number of Parameters: {}", exr->NumberParameters);
        
        for (DWORD i = 0; i < exr->NumberParameters; i++) {
            Info("Parameter[{}]: {:p}", i, (void*)exr->ExceptionInformation[i]);
        }

        if(ex->ContextRecord) {
            PrintStackTracectx(ex->ContextRecord);
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

} // anonym space

auto setup_crach_handler() -> void
{
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    AddVectoredExceptionHandler(1, ExceptionHandler);
}

#endif //DEBUG
#endif //_WIN32