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
    PrintStackTrace();
    ExitProcess(1);
    return EXCEPTION_EXECUTE_HANDLER;
}

} // anonym space

auto setup_crach_handler() -> void
{
    AddVectoredExceptionHandler(1, ExceptionHandler);
}