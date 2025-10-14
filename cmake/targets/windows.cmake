# windows specific target definitions
set_target_properties(sunshine PROPERTIES LINK_SEARCH_START_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
find_library(ZLIB ZLIB1)
list(APPEND SUNSHINE_EXTERNAL_LIBRARIES
        Windowsapp.lib
        Wtsapi32.lib)

# GUI build (Tauri)
find_program(CARGO cargo)
if(CARGO)
    message(STATUS "Found Cargo: ${CARGO}")
    add_custom_target(sunshine-control-panel ALL
            WORKING_DIRECTORY "${SUNSHINE_SOURCE_ASSETS_DIR}/common/sunshine-control-panel"
            COMMENT "Building Sunshine Control Panel (Tauri GUI)"
            COMMAND "$<$<BOOL:${WIN32}>:cmd;/C>" "${NPM}" install ${NPM_INSTALL_FLAGS}
            COMMAND "$<$<BOOL:${WIN32}>:cmd;/C>" "${NPM}" run build:renderer
            COMMAND "${CARGO}" build --manifest-path "${CMAKE_SOURCE_DIR}/src_assets/common/sunshine-control-panel/src-tauri/Cargo.toml" --release
            COMMAND_EXPAND_LISTS
            VERBATIM)
else()
    message(WARNING "Cargo not found. Sunshine Control Panel (Tauri GUI) will not be built. Install Rust to enable GUI build.")
endif()
