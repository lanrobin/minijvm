# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("minijava")

# check compiler version
if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0)
    message( SEND_ERROR  "You need g++ version newer than 8.0")
elseif(MSVC AND MSVC_VERSION VERSION_LESS 1920)
    message(SEND_ERROR  "You need visual studio 2017 and newer.")
endif()

# https://stackoverflow.com/questions/2368811/how-to-set-warning-level-in-cmake
if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

IF(WIN32)

    # we use filesystem for vs 17
    OPTION(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING "use filesystem in stdc++=14" ON)
    set(PTHREAD_LIB_INCLUDE "./windows/pthread/include")

    if("${CMAKE_SIZEOF_VOID_P}" EQUAL 8)
       set(PTHREAD_LIB_AND_DLL_DIR "${PROJECT_SOURCE_DIR}/windows/pthread/Win64")
    else()
       set(PTHREAD_LIB_AND_DLL_DIR "${PROJECT_SOURCE_DIR}/windows/pthread/Win32")
    endif()
    link_directories("${PTHREAD_LIB_AND_DLL_DIR}" "Shlwapi.lib")

    #https://stackoverflow.com/questions/10671916/how-to-copy-dll-files-into-the-same-folder-as-the-executable-using-cmake

    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

ELSE()
    SET(PTHREAD_LIB_INCLUDE "")
    add_compile_options(-pthread)
ENDIF()

include_directories("include" ${PTHREAD_LIB_INCLUDE})

# Include sub-projects.
#add_subdirectory ("MiniJVM")

set(Clazz_File "./clazz/clazz_reader.cpp" "./clazz/compressed_file_reader.cpp" )

set(Main_File "./main.cpp" "./vm/vm.cpp" "vm/vm_thread.cpp" "vm/vm_method_area.cpp" "vm/vm_class.cpp" "vm/vm_classloader.cpp" "vm/vm_heap.cpp" "vm/vm_engine.cpp" "vm/vm_native_methods.cpp" "vm/vm_engine_private.cpp" "vm/vm_engine_definition.cpp")

Set(Utils "./util/buffer.cpp" "./util/string_utils.cpp" "util/system.cpp"  "util/configurations.cpp")

Set(Java_Natives "natives/java_lang_System.cpp" "natives/java_lang_Object.cpp")

set(ThirdParty "./thirdparties/zip/zip.c")

#get_directory_property(_my_link_dirs LINK_DIRECTORIES)
#message(STATUS "_my_link_dirs = ${_my_link_dirs}") 

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


add_executable(${PROJECT_NAME} ${Main_File} ${Clazz_File} ${Utils} ${ThirdParty} ${Java_Natives})

IF(WIN32)
    
    target_link_libraries(${PROJECT_NAME} "${PTHREAD_LIB_AND_DLL_DIR}/pthreads.lib")

    add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PTHREAD_LIB_AND_DLL_DIR}/pthreads.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path
ELSE()
        target_link_libraries(${PROJECT_NAME} pthread)
ENDIF()

# config config file.
add_custom_command(TARGET "${PROJECT_NAME}" POST_BUILD        # Adds a post-build event to MyTest
COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/config.txt"      # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)                 # <--this is out-file path

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
