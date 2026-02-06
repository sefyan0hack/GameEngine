
if(WITH_DOCS)
    find_package(Doxygen REQUIRED)

    set(DOXY_IN    "${CMAKE_SOURCE_DIR}/Doxyfile.in")
    set(DOXY_OUT   "${CMAKE_BINARY_DIR}/Doxyfile")
   
    configure_file(${DOXY_IN} ${DOXY_OUT} @ONLY)
   
    add_custom_target(docs
        VERBATIM
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_OUT}
        COMMENT "Generating Documentation with Doxygen"
    )

endif()