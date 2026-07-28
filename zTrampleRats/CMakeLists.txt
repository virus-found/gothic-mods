cmake_minimum_required(VERSION 3.25)

cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0141 NEW)

project(zTrampleRats LANGUAGES CXX RC)

add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
add_compile_options(/wd4996 /wd4244)

set(UNION_SDK_ROOT "$ENV{HOME}/my_msvc/opt/Union/1.0m" CACHE PATH
    "Root containing Union/v1.0m and DirectX7")
set(UNION_GAME "G2A" CACHE STRING
    "Union target: G1, G1A, G2, G2A, MP_X2, or MP_X4")
set(UNION_RUNTIME "MT" CACHE STRING "MSVC runtime: MT or MD")

set(_plugin_dir "${CMAKE_CURRENT_SOURCE_DIR}/zTrampleRats")
set(_union_include "${UNION_SDK_ROOT}/Union/v1.0m/Include")
set(_union_lib "${UNION_SDK_ROOT}/Union/v1.0m/Lib")
set(_directx_include "${UNION_SDK_ROOT}/DirectX7/include")
set(_directx_lib "${UNION_SDK_ROOT}/DirectX7/lib")

set(_project_sources
    "${_plugin_dir}/DllMain.cpp"
    "${_plugin_dir}/Plugin.cpp"
    "${_plugin_dir}/ZenGin/Gothic_II_Addon/Globals_G2A.cpp"
    "${_plugin_dir}/ZenGin/Gothic_II_Addon/ClassDeclarators_G2A.cpp"
    "${_plugin_dir}/ZenGin/Gothic_II_Addon/Statics_G2A.cpp"
)

add_library(zTrampleRats SHARED ${_project_sources})

set_property(TARGET zTrampleRats PROPERTY CXX_STANDARD 17)

set(_project_includes
    "${_plugin_dir}"
    "${_plugin_dir}/ZenGin/Gothic_UserAPI"
    "${_union_include}"
    "${_directx_include}"
)

target_include_directories(zTrampleRats PRIVATE ${_project_includes})
target_link_directories(zTrampleRats PRIVATE "${_union_lib}" "${_directx_lib}")

target_compile_definitions(zTrampleRats PRIVATE WIN32 _WINDOWS _USRDLL GOTHICPLUGIN_EXPORTS _WINDLL _MBCS __G2A)
set_property(TARGET zTrampleRats PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded")

target_compile_options(zTrampleRats PRIVATE
    /W3 /Oi /Oy- /GS /Gy /fp:precise
    /Zc:wchar_t /Zc:forScope /Zc:inline /wd4244 /wd4996
    "/FI${_plugin_dir}/UnionAfx.h")

target_link_options(zTrampleRats PRIVATE
    /SUBSYSTEM:WINDOWS
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>)

target_link_libraries(zTrampleRats PRIVATE Shw32 Vdfs32g Psapi Gdiplus Ws2_32)

set_target_properties(zTrampleRats PROPERTIES
    PREFIX ""
    OUTPUT_NAME "zTrampleRats"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
