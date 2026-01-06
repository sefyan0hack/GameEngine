
if(DOCS)
    find_package(Doxygen QUIET)
    set_package_properties(Doxygen PROPERTIES
        TYPE OPTIONAL
        PURPOSE "Used to generate developer documentation."
    )

    set(DOXY_IN    "${CMAKE_SOURCE_DIR}/Doxyfile.in")
    set(DOXY_OUT   "${CMAKE_BINARY_DIR}/Doxyfile")
   
    configure_file(${DOXY_IN} ${DOXY_OUT} @ONLY)
   
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

endif()