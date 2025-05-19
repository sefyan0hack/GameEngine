function(print_target_compile_options target)
    foreach(config IN ITEMS ${CMAKE_BUILD_TYPE})
        get_target_property(flags ${target} COMPILE_OPTIONS)
        if(flags)
            string(REPLACE ";" " " flags_with_spaces "${flags}")
            message(STATUS "  + ${target} (${config}): ${flags_with_spaces}\n")
        else()
            message(STATUS "  + ${target} (${config}): No flags set\n")
        endif()
    endforeach()
endfunction()

function(apply_compile_options)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(target IN LISTS ARG_TARGETS)
        # check if the target is an ALIAS
        get_target_property(original_target ${target} ALIASED_TARGET)
            
        if (original_target)
            message(STATUS "Applying compile options to alias target '${target}' (original: '${original_target}')")
            set(target ${original_target})
        endif()
        
        if(MSVC)
            target_compile_options(${target} PRIVATE
                /W4
                /bigobj
                # Debug flags
                "$<$<CONFIG:Debug>:/Z7>"
                "$<$<CONFIG:Debug>:/Od>"
                "$<$<CONFIG:Debug>:/RTC1>"
                "$<$<CONFIG:Debug>:/EHsc>"
                "$<$<CONFIG:Debug>:/DYNAMICBASE>"
                "$<$<CONFIG:Debug>:/analyze>"
                "$<$<CONFIG:Debug>:/Oy->"

                # Release flags
                "$<$<CONFIG:Release>:/O2>"
                "$<$<CONFIG:Release>:/Zi>"
                "$<$<CONFIG:Release>:/Zo>"
                "$<$<CONFIG:Release>:/Oy->"
            )
            target_link_options(${target} PRIVATE
                "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"       # no terminale
                "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>"    # entry
                "$<$<CONFIG:Release>:/OPT:REF>"                 # Optimize unused functions
                "$<$<CONFIG:Release>:/OPT:ICF>"                 # Identical COMDAT folding
                "$<$<CONFIG:Release>:/INCREMENTAL:NO>"          # Disable incremental linking
            )
            add_definitions(/FI"${CMAKE_SOURCE_DIR}/include/core/Global_H.hpp")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -Wno-cast-function-type -Winit-self -Wcast-qual
                -Wsuggest-final-types -Wsuggest-final-methods
                -fdevirtualize -ftree-vectorize 
               
            )
            add_link_options(
                -static-libasan -static-libtsan -static-liblsan -static-libubsan 
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${target} PRIVATE -Wno-language-extension-token)
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wwrite-strings -Wno-sign-conversion
            -Wnull-dereference -Wswitch-enum
            -Wuninitialized -Wpointer-arith -Wreturn-type -Wredundant-decls
            -fno-operator-names
            -frtti

            # Debug flags
            "$<$<CONFIG:Debug>:-ggdb>"
            "$<$<CONFIG:Debug>:-g>"
            "$<$<CONFIG:Debug>:-O0>"
            # Release flags
            "$<$<CONFIG:Release>:-O3>"
            "$<$<CONFIG:Release>:-g1>"
            "$<$<CONFIG:Release>:-march=native>"  #enable when dnt need to send the app
            "$<$<CONFIG:Release>:-funwind-tables>"
            # "$<$<CONFIG:Release>:-fasynchronous-unwind-tables>" # seems to be causing issues
            "$<$<CONFIG:Release>:-ffunction-sections>"
            "$<$<CONFIG:Release>:-fdata-sections>"
        )
        target_link_options(${target} PRIVATE
            "$<$<AND:$<CONFIG:Release>,$<STREQUAL:$<PLATFORM_ID>,Windows>>:-Wl,--subsystem,windows>"
        )
        add_link_options(
            -static-libgcc -static-libstdc++
            -Wl,--as-needed
            -Wl,--no-undefined
        )
        add_definitions(-include "${CMAKE_SOURCE_DIR}/include/core/Global_H.hpp")
        endif()
    endforeach()
endfunction()

function(delete_files_by_extension DIR EXTENSION)
    file(GLOB_RECURSE FILES_TO_DELETE 
        LIST_DIRECTORIES false
        "${DIR}/**.${EXTENSION}"
        "${DIR}/**/.*.${EXTENSION}"
    )

    if(NOT FILES_TO_DELETE)
        message(STATUS "No *.${EXTENSION} files found in ${DIR}")
        return()
    endif()

    list(LENGTH FILES_TO_DELETE NUM_FILES)
    message(STATUS "Deleting ${NUM_FILES} *.${EXTENSION} files...")

    foreach(FILE IN LISTS FILES_TO_DELETE)
        if(EXISTS "${FILE}")
            file(REMOVE "${FILE}")
            if(VERBOSE)
                message(STATUS "Deleted: ${FILE}")
            endif()
        else()
            if(WARN_ON_MISSING)
                message(WARNING "File not found: ${FILE}")
            endif()
        endif()
    endforeach()
endfunction()


function(apply_harden_options)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    foreach(target IN LISTS ARG_TARGETS)
        # check if the target is an ALIAS
        get_target_property(original_target ${target} ALIASED_TARGET)
        get_target_property(type ${target} TYPE)

        if (original_target)
            message(STATUS "Applying compile options to alias target '${target}' (original: '${original_target}')")
            set(target ${original_target})
        endif()
        
        if(MSVC)
            target_compile_options(${target} PRIVATE
                /sdl /GS /SafeSEH /guard:cf /HIGHENTROPYVA /dynamicbase
            )
            target_link_options(${target} PRIVATE
                /guard:cf
            )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -mindirect-branch=thunk-inline -mindirect-branch-register
                -mno-indirect-branch-register -lvi-load-hardening -lvi-cfi
                -ehcontguard
            )

        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE
            -flto
            -fvisibility=default
            -fsanitize=cfi -fsanitize=cfi-icall -fsanitize=cfi-mfcall -fsanitize=safe-stack
            -ftrivial-auto-var-init=zero
            -mretpoline 
        )
        endif()

        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

            target_compile_options(${target} PRIVATE
                "$<$<STREQUAL:$<TARGET_PROPERTY:${target},TYPE>,EXECUTABLE>:-fPIE>"
                "$<$<NOT:$<STREQUAL:$<TARGET_PROPERTY:${target},TYPE>,EXECUTABLE>>:-fPIC>"

                -Wformat -Wformat-security -Werror=format-security -fno-strict-aliasing -fno-common
                -fzero-call-used-regs=all -mharden-sls=all -ftrivial-auto-var-init=pattern 
                -fcf-protection=none -ftrapv
                -fstack-protector -fstack-protector-strong
            )

            target_link_options(${target} PRIVATE
                -Wl,-O1 -Wl,-flto -fstack-clash-protection
                "$<$<STREQUAL:$<TARGET_PROPERTY:${target},TYPE>,EXECUTABLE>:-pie;-Wl,-pie>"
                "$<$<STREQUAL:$<PLATFORM_ID>,Windows>:-Wl,--export-all-symbols;-Wl,--nxcompat;-Wl,--dynamicbase>"
                "$<$<STREQUAL:$<PLATFORM_ID>,Linux>:-Wl,--sort-common;-Wl,--as-needed>"
                "$<$<STREQUAL:$<PLATFORM_ID>,Linux>:-Wl,-z,relro;-Wl,-z,now;-Wl,-z,ibtplt;-Wl,-z,ibt;-Wl,-z,shstk;-Wl,-z,notext;-Wl,-z-noexecstack;-Wl,-z,noexecheap>"
            )
            add_definitions(-D_FORTIFY_SOURCE=2)
            add_definitions(-D_GLIBCXX_ASSERTIONS)
        endif()

    endforeach()
endfunction()