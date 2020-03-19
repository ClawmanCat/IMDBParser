cmake_minimum_required (VERSION 3.12)

project ("IMDBParser")


# Setup Conan Dependencies
if (WIN32)
     # Conan does not work with compilers other than MSVC on Windows.
     set(PREV_CMAKE_C_COMPILER ${CMAKE_C_COMPILER})
     set(PREV_CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER})

     set(CMAKE_C_COMPILER MSVC)
     set(CMAKE_CXX_COMPILER MSVC)

     set(CONAN_DISABLE_CHECK_COMPILER TRUE)
endif()


include(FindPythonInterp)
execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/run_conan.py)

include(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/${CMAKE_BUILD_TYPE}/conanbuildinfo.cmake)
conan_basic_setup()


if (WIN32)
     set(CMAKE_C_COMPILER ${PREV_CMAKE_C_COMPILER})
     set(CMAKE_CXX_COMPILER ${PREV_CMAKE_CXX_COMPILER})
endif()



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


add_compile_definitions(PARSER_DATASOURCE_DIR=${PARSER_DATADST})
add_compile_definitions(PARSER_CSV_DIR=${CMAKE_CURRENT_SOURCE_DIR}/csv)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("IMDBParser")
