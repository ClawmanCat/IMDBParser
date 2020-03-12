cmake_minimum_required (VERSION 3.12)

project ("IMDBParser")


# Unzip Data Files
set(PARSER_DATASRC ${CMAKE_CURRENT_SOURCE_DIR}/data/compressed)
set(PARSER_DATADST ${CMAKE_CURRENT_SOURCE_DIR}/data/decompressed)

if (EXISTS ${PARSER_DATASRC})
     if (NOT EXISTS ${PARSER_DATADST})
          file(MAKE_DIRECTORY ${PARSER_DATADST})
     endif()

     file(GLOB_RECURSE packed_data CONFIGURE_DEPENDS LIST_DIRECTORIES false "${PARSER_DATASRC}" "*.gz")

     foreach (srcpath ${packed_data})
          get_filename_component(srcfile ${srcpath} NAME_WLE)
          message(STATUS "Checking archive ${srcfile}...")

          if (NOT EXISTS "${PARSER_DATADST}/${srcfile}.list")
               message(STATUS "   ... Unpacked archive does not exist and will be created.")

               execute_process(COMMAND gzip -dkv ${srcpath})
               file(RENAME "${PARSER_DATASRC}/${srcfile}" "${PARSER_DATADST}/${srcfile}")
          else()
               message(STATUS "   ... Unpacked archive already exists.")
          endif()
     endforeach()
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory ("IMDBParser")
