#include <core/Log.hpp>
#include <csignal>
#include <fstream>
#include <iostream>

namespace {

auto PrintStackTrace() -> void
{
    const auto st = std::stacktrace::current(2);
    std::cerr << std::format("Stack frame :\n");
    for(auto &&frame: st){
        auto line = frame.source_line();

        if(line != 0){
            std::cerr << std::format("  {} , {}:{}\n", frame.description(), frame.source_file(), line);
        }
    }
    std::cin.get();
}

auto WINAPI ExceptionHandler([[maybe_unused]] PEXCEPTION_POINTERS ex) -> LONG
{

    Log::Info("\n--- Exception Caught ---\n");
    Log::Info("Exception Code: {}\n", ex->ExceptionRecord->ExceptionCode);
    Log::Info("Exception Address: {}\n", ex->ExceptionRecord->ExceptionAddress);
    

    if (ex->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION ||  ex->ExceptionRecord->ExceptionCode == 1073807370L) {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    PrintStackTrace();
    return EXCEPTION_EXECUTE_HANDLER;
}

} // anonym space

auto setup_crach_handler() -> void
{
    AddVectoredExceptionHandler(1, ExceptionHandler);
}