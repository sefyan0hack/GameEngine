# Usage:
#   cmake -D OUT_DIR=build -D DOXY_IN=/path/to/Doxyfile.in -P GenDocs.cmake

if(NOT DEFINED DOXYGEN_EXECUTABLE)
  find_program(DOXYGEN_EXECUTABLE doxygen)
endif()

if(NOT DOXYGEN_EXECUTABLE)
  message(FATAL_ERROR "Doxygen executable not found. Install Doxygen or set -D DOXYGEN_EXECUTABLE=/path/to/doxygen")
endif()

set(DOXY_OUT "${OUT_DIR}/Doxyfile")

file(MAKE_DIRECTORY "${OUT_DIR}")

if(NOT EXISTS "${DOXY_IN}")
  message(FATAL_ERROR "Doxyfile.in not found: ${DOXY_IN}")
endif()

configure_file("${DOXY_IN}" "${DOXY_OUT}" @ONLY)

message(STATUS "Running: ${DOXYGEN_EXECUTABLE} ${DOXY_OUT} (working dir: ${OUT_DIR})")

execute_process(
  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXY_OUT}"
  WORKING_DIRECTORY "${OUT_DIR}"
  RESULT_VARIABLE _doxygen_result
  OUTPUT_VARIABLE _doxygen_stdout
  ERROR_VARIABLE  _doxygen_stderr
)

if(NOT _doxygen_result EQUAL 0)
  message(FATAL_ERROR "Doxygen failed with exit code ${_doxygen_result}\n--- STDOUT ---\n${_doxygen_stdout}\n--- STDERR ---\n${_doxygen_stderr}")
else()
  message(STATUS "Doxygen finished successfully.")
  if(_doxygen_stdout)
    message(STATUS "${_doxygen_stdout}")
  endif()
endif()