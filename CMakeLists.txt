# This CMAKE file builds the library libinference.
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(dummy_interface VERSION 0.1 LANGUAGES CXX)

# Find torch
find_package(Torch REQUIRED)

# Include custom FindMatlab.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
# set(Matlab_ROOT_DIR "/Applications/MATLAB_R2019a.app")
find_package(Matlab REQUIRED)

# Build the shared library
# the next set is so that I can actually link against it, who does not love windows ... :DD
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS YES CACHE BOOL "Export all symbols")
add_library(dummy_lib SHARED src/arbitrary_torch.cpp include/arbitrary_torch.hpp)
target_compile_features(dummy_lib PUBLIC cxx_std_11)
target_link_libraries(dummy_lib "${TORCH_LIBRARIES}")
target_include_directories(dummy_lib PUBLIC include)

# Build the MATLAB Mex Target
matlab_add_mex(
  NAME mat_torch_interface
  SHARED
  SRC src/mex_interface.cpp
  LINK_TO dummy_lib
  R2018a)


# For Windows one needs to copy the .dll to where the target is being built since there is no such thing
# as rpath on windows.
# Note: As of know, while running the mex file,
# you have the be in the same folder as the .mex and all the dll's are, otherwise it does not work ...
if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET dummy_lib
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:dummy_lib>)
endif (MSVC)
