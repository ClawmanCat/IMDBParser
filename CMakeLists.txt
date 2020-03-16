cmake_minimum_required (VERSION 3.12)

project ("IMDBParser")


set(PARSER_DATASRC ${CMAKE_CURRENT_SOURCE_DIR}/data/compressed/)
set(PARSER_DATADST ${CMAKE_CURRENT_SOURCE_DIR}/data/decompressed/)

# Add data sources here to automatically download and extract them:
set(PARSER_DATAFILES
     "actors.list.gz\;http://srv-7.directserver.us/?file=mega_12494fec62"
     "actresses.list.gz\;http://srv-15.directserver.us/?file=mega_10ee6a135b"
     "locations.list.gz\;http://srv-9.directserver.us/?file=mega_3f8fbd3363"
     "language.list.gz\;http://srv-14.directserver.us/?file=mega_5e27708caf"
     "directors.list.gz\;http://srv-15.directserver.us/?file=mega_7c44d01519"
     "ratings.list.gz\;http://srv-16.directserver.us/?file=mega_55083da50f"
     "movies.list.gz\;http://srv-11.directserver.us/?file=mega_15f3d8b5be"
)


# Download Data Files
if (NOT EXISTS ${PARSER_DATASRC})
     file(MAKE_DIRECTORY ${PARSER_DATASRC})
endif()

foreach(pair ${PARSER_DATAFILES})
     list(GET pair 0 PD_FILENAME)
     list(GET pair 1 PD_URL)

     if (NOT EXISTS "${PARSER_DATASRC}/${PD_FILENAME}")
          message(STATUS "Downloading file ${PD_FILENAME}. (Source: ${PD_URL})")

          file(DOWNLOAD "${PD_URL}" "${PARSER_DATASRC}/${PD_FILENAME}")
     else()
          message(STATUS "Skipping downloading of file ${PD_FILENAME}: File already exists.")
     endif()
endforeach()


# Unzip Data Files
if (EXISTS ${PARSER_DATASRC})
     if (NOT EXISTS ${PARSER_DATADST})
          file(MAKE_DIRECTORY ${PARSER_DATADST})
     endif()

     file(GLOB_RECURSE packed_data CONFIGURE_DEPENDS LIST_DIRECTORIES false "${PARSER_DATASRC}/*.gz")

     foreach (srcpath ${packed_data})
          get_filename_component(srcfile ${srcpath} NAME_WLE)
          message(STATUS "Checking archive ${srcfile}...")

          if (NOT EXISTS "${PARSER_DATADST}/${srcfile}")
               message(STATUS "   ... Unpacked archive does not exist and will be created.")

               execute_process(COMMAND gzip -dkv ${srcpath})
               file(RENAME "${PARSER_DATASRC}/${srcfile}" "${PARSER_DATADST}/${srcfile}")
          else()
               message(STATUS "   ... Unpacked archive already exists.")
          endif()
     endforeach()
endif()


add_compile_definitions(PARSER_DATASOURCE_DIR="${PARSER_DATADST}")
add_compile_definitions(PARSER_CSV_DIR="${CMAKE_CURRENT_SOURCE_DIR}/csv")
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("IMDBParser")
