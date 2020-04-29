# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)



project ("MiniJVM")

include_directories("include")

# Include sub-projects.
#add_subdirectory ("MiniJVM")

set(Clazz_File "./Clazz/clazz_reader.cpp" "./Clazz/compressed_file_reader.cpp")

set(Main_File "./main.cpp")

Set(Utils "./Util/buffer.cpp" "./Util/string_utils.cpp")

set(ThirdParty "ThirdParties/zip/zip.c")

add_executable(${PROJECT_NAME} ${Main_File} ${Clazz_File} ${Utils} ${ThirdParty})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)