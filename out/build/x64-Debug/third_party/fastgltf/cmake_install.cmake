# Install script for directory: D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "D:/Github/vulkan-guide-starting-point-2/out/install/x64-Debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/deps/simdjson/simdjson.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/fastgltf_simdjson.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltf_simdjsonTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltf_simdjsonTargets.cmake"
         "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltf_simdjsonTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltf_simdjsonTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltf_simdjsonTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltf_simdjsonTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltf_simdjsonTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fastgltf" TYPE FILE FILES
    "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/include/fastgltf/base64.hpp"
    "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/include/fastgltf/glm_element_traits.hpp"
    "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/include/fastgltf/parser.hpp"
    "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/include/fastgltf/tools.hpp"
    "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/include/fastgltf/types.hpp"
    "D:/Github/vulkan-guide-starting-point-2/third_party/fastgltf/include/fastgltf/util.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/fastgltf.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig.cmake"
         "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf/fastgltfConfig.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fastgltf" TYPE FILE FILES "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/CMakeFiles/Export/14b504216a8c2b5661a6a7c88f3023ae/fastgltfConfig-debug.cmake")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/Github/vulkan-guide-starting-point-2/out/build/x64-Debug/third_party/fastgltf/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
