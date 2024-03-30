find_package(Doxygen)
set(MD_SOURCES 01_intro.md 02_memory.md 03_commands.md) 
set(IMAGE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/images")
set(DEST_IMAGE_PATH "${CMAKE_BINARY_DIR}/${IMAGE_PATH}")
set(PROJECT_LOGO "${CMAKE_CURRENT_SOURCE_DIR}/images/npn.png")
set(IMAGES "${IMAGE_PATH}/pico_adapter.png" "${IMAGE_PATH}/flash_circuit.png")
set(PLANTUML_JAR_PATH "/usr/share/java/plantuml.jar")
if(DOXYGEN_FOUND)
    configure_file(
        "doxygen.conf.in"
        "doxygen.conf"
        @ONLY
    )
    configure_file(
        "header.tex"
        "header.tex"
        COPYONLY
    )
    add_custom_target(
        doc
        "${DOXYGEN_EXECUTABLE}"
        "${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf"
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        DEPENDS "${MD_SOURCES}"
        DEPENDS "${PROJECT_LOGO}"
        DEPENDS "${IMAGES}"
        COMMENT "Generating code documentation..." VERBATIM
    )
    add_custom_target(
        pdf
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf"
    )
    add_custom_command(
        OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/latex/refman.pdf"
        COMMAND "make"
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/html/index.html"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/latex"
        COMMENT "Generating PDF format code documentation..." VERBATIM
    )

endif(DOXYGEN_FOUND)
