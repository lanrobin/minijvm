# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("MiniJVM")

include_directories("include")

# Include sub-projects.
#add_subdirectory ("MiniJVM")

set(Clazz_File "./Clazz/clazz_reader.cpp")

set(Main_File "./MiniJVM/MiniJVM.cpp" "./MiniJVM/MiniJVM.h")

add_executable(${PROJECT_NAME} ${Main_File} ${Clazz_File} "Clazz/compressed_file_reader.cpp" "ThirdParties/zip/zip.c" "Util/buffer.cpp")