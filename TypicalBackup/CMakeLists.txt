cmake_minimum_required(VERSION 3.16)
project(TypicalBackup LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 预处理
#add_definitions(-DWIN32APP)
add_definitions(-D_WINDOWS)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
    Gui Widgets OpenGL Quick QuickControls2 Qml
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    src/main.cpp
)

# 查找所有源文件和头文件
file(GLOB_RECURSE SOURCE_FILES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

# lib: TypicalTool
file(GLOB_RECURSE libTypical_libHEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/TypicalTool/*.h")
file(GLOB_RECURSE libTypical_libHEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/TypicalTool/**/*.h")
file(GLOB_RECURSE Json_libHEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/Json/*.h")

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/TypicalTool)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/Json)


if(QT_VERSION VERSION_LESS 5.15)
    qt5_add_resources(PROJECT_SOURCES qml.qrc)
elseif(QT_VERSION VERSION_LESS 6.2)
    qt_add_resources(PROJECT_SOURCES qml.qrc)
endif()

qt_add_executable(${PROJECT_NAME} ${SOURCE_FILES_LIST} ${HEADER_FILES} ${PROJECT_SOURCES} ${libTypical_libHEADER_FILES} ${Json_libHEADER_FILES})

if(QT_VERSION VERSION_GREATER_EQUAL 6.2)
    qt_add_qml_module(${PROJECT_NAME}
        URI TypicalBackup
        VERSION 1.0
        RESOURCE_PREFIX "/qt/qml/"
        QML_FILES
            main.qml
    )
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

# 设置库路径（根据构建类型选择）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TYPICALTOOL_SHARED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/TypicalTool")
    set(JSONCPP_SHARED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/Json")  
else()
    set(TYPICALTOOL_SHARED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/TypicalTool")
    set(JSONCPP_SHARED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/Json")
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    #UN_EXPORTS
)

# 链接库文件
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::OpenGL
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::QuickControls2
        
        "${TYPICALTOOL_SHARED_PATH}/$<CONFIG>/TypicalTool.lib"  # 注意链接的是 .lib 文件
        #"${JSONCPP_SHARED_PATH}/$<CONFIG>/jsoncpp.lib"
)

if(Qt6_FOUND)
    qt_import_qml_plugins(${PROJECT_NAME})
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${TYPICALTOOL_SHARED_PATH}/$<CONFIG>_Win/TypicalTool.dll"
        #"${TYPICALTOOL_SHARED_PATH}/$<CONFIG>/jsoncpp.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying Qt DLLs to output directory"
)