# Install script for directory: D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/SRT")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt_static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt_static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt_static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt_static.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/srt" TYPE FILE FILES
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/version.h"
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/srtcore/srt.h"
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/srtcore/logging_api.h"
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/srtcore/access_control.h"
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/srtcore/platform_sys.h"
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/srtcore/udt.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/srt/win" TYPE FILE FILES
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/common/win/syslog_defs.h"
    "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/common/win/unistd.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/haisrt.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/srt.pc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt-live-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt-live-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt-live-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt-live-transmit.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt-live-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt-live-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt-live-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt-live-transmit.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt-file-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt-file-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt-file-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt-file-transmit.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt-file-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt-file-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt-file-transmit.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt-file-transmit.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt-tunnel.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt-tunnel.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt-tunnel.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt-tunnel.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Debug/srt-tunnel.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/Release/srt-tunnel.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/MinSizeRel/srt-tunnel.exe")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/RelWithDebInfo/srt-tunnel.exe")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/scripts/srt-ffplay")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/UE_Plugin_Develop/CineBroadcastProject/Plugins/CineSRTStream/Source/ThirdParty/SRT_Source/build_win64/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
