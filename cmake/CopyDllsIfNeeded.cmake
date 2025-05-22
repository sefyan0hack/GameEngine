# CopyDllsIfNeeded.cmake
if(DEFINED ENV{DLL_PATHS})
  # Split the environment variable into a list
  string(REPLACE ";" " " DLL_PATHS "$ENV{DLL_PATHS}")
  separate_arguments(DLL_PATHS)

  # Copy only if DLL_PATHS is not empty
  if(DLL_PATHS)
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy ${DLL_PATHS} "$ENV{DEST_DIR}"
      RESULT_VARIABLE result
    )
    if(NOT result EQUAL 0)
      message(FATAL_ERROR "Failed to copy DLLs: ${result}")
    endif()
  endif()
endif()