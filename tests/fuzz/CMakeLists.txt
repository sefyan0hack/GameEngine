
add_executable(fuzz_tests 
    Utils.cpp
)

target_link_libraries(fuzz_tests PRIVATE Engine)

target_compile_options(fuzz_tests PRIVATE -fsanitize=fuzzer)
target_link_options(fuzz_tests PRIVATE -fsanitize=fuzzer)

if(WIN32)
    target_compile_options(fuzz_tests PRIVATE
        $<$<CONFIG:DEBUG>:/MTd>
        $<$<NOT:$<CONFIG:DEBUG>>:/MT>
    )
    
    target_link_options(fuzz_tests PRIVATE
        $<$<CONFIG:DEBUG>:-Xlinker /NODEFAULTLIB:msvcrtd.lib>
        $<$<NOT:$<CONFIG:DEBUG>>:-Xlinker /NODEFAULTLIB:msvcrt.lib>
    )
endif()