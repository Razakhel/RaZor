cmake_minimum_required(VERSION 3.6)
project(RaZor)

set(CMAKE_CXX_STANDARD 14)

# Define build type to Release if not automatically set
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_COMPILER_IS_GNUCC)
    set(
        GCC_FLAGS

        "-pedantic -pedantic-errors \
         -Wall -Wextra \
         -Wcast-align -Wcast-qual \
         -Wdisabled-optimization \
         -Wfloat-equal \
         -Wformat-security -Wformat=2 \
         -Wmissing-declarations -Wmissing-include-dirs \
         -Wnon-virtual-dtor \
         -Wold-style-cast \
         -Woverloaded-virtual \
         -Wpacked \
         -Wredundant-decls \
         -Wundef -Wuninitialized \
         -Wunused-macros \
         -Wvla \
         -Wconversion \
         -Wswitch-default -Wswitch-enum \
         -Wzero-as-null-pointer-constant \
         -Wlogical-op \
         -Wvector-operation-performance \
         -Wdouble-promotion \
         -Wtrampolines \
         -Wuseless-cast \
         -Wconditionally-supported \
         -Wfloat-conversion \
         -Wopenmp-simd \
         -Warray-bounds \
         -Wconditionally-supported \
         -Wnoexcept \
         -Wstrict-null-sentinel \
         -Wno-format-security -Wno-format-nonliteral \
         -Wno-switch-default \
         \
         -Wno-conversion \
         -Wno-useless-cast \
         -Wno-noexcept \
         -Wno-unused-parameter \
         -Wno-return-type" # Disabling some warnings triggered by Qt
    )

    # Enabling some other warnings available since GCC 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
        set(
            GCC_FLAGS

            "${GCC_FLAGS} \
             -Wformat-signedness \
             -fsized-deallocation \
             -Wsized-deallocation \
             -Wsuggest-override \
             -Warray-bounds=2"
        )
    endif ()

    # Enabling some other warnings available since GCC 6
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6)
        set(
            GCC_FLAGS

            "${GCC_FLAGS} \
             -Wduplicated-cond \
             -Wnull-dereference"
        )
    endif ()

    # Enabling some other warnings available since GCC 7
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 7 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7)
        set(
            GCC_FLAGS

            "${GCC_FLAGS} \
             -Walloc-zero -Walloca \
             -Wformat-overflow \
             -Wshadow=compatible-local \
             -Waligned-new"
        )
    endif ()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(
        CLANG_FLAGS

        "-pedantic -pedantic-errors \
         -Weverything \
         -Wno-c++98-compat -Wno-c++98-compat-pedantic \
         -Wno-mismatched-tags \
         -Wno-padded \
         -Wno-shadow \
         -Wno-global-constructors \
         -Wno-weak-vtables \
         -Wno-exit-time-destructors \
         -Wno-covered-switch-default \
         -Wno-reserved-id-macro \
         -Wno-missing-braces \
         -Wno-documentation -Wno-documentation-unknown-command"
    )

    # Disabling some warnings available since Clang 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5)
        set(
            CLANG_FLAGS

            "${CLANG_FLAGS} \
             -Wno-unused-template"
        )
    endif ()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_FLAGS}")
elseif (MSVC)
    set(
        MSVC_FLAGS

        "/W4"
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_FLAGS}")
endif ()

# Finding Qt
if (WIN32)
    set(QT_LOCATION "" CACHE PATH "Path to the Qt5 installation")

    if (NOT ${QT_LOCATION} STREQUAL "")
        if (MINGW)
            set(QT_LOCATION "${QT_LOCATION}/mingw73_64")
        else ()
            set(QT_LOCATION "${QT_LOCATION}/msvc2017_64")
        endif ()

        set(CMAKE_PREFIX_PATH ${QT_LOCATION})
    endif ()
endif ()

find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

link_directories(${QT_LOCATION}/lib)

set(
    LIBS

    ${LIBS}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${QT_LOCATION}/include
)

set(
    SRC

    src/RaZor/*.cpp
    src/RaZor/Interface/*.cpp

    include/RaZor/*.hpp
    include/RaZor/Interface/*.hpp

    assets/resources/resources.qrc
)

# Defining preprocessor macros and selecting files to be removed
if (APPLE)
    # ...
elseif (UNIX)
    option(RAZOR_USE_SANITIZERS "Use sanitizers" OFF)

    if (RAZOR_USE_SANITIZERS)
        add_definitions(
            -fsanitize=undefined
            -fsanitize=address
            -fsanitize=leak
        )

        set(
            LIBS

            ${LIBS}
            asan
            ubsan
        )
    endif ()
elseif (WIN32)
    # ...
endif ()

# Adding recursively every file to be compiled
file(
    GLOB
    SOURCE_FILES

    ${SRC}
)

# Creating translation files
qt5_create_translation(TRANSLATION_FILES ${SOURCE_FILES} assets/translations/english.ts assets/translations/french.ts)

add_executable(RaZor main.cpp ${SOURCE_FILES} ${TRANSLATION_FILES})
target_link_libraries(RaZor ${LIBS})

# Deploying Qt
if (WIN32)
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE ${QT5_QMAKE_EXECUTABLE} PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

    # Specifying the preferred deploy mode's hint for MinGW
    if (MINGW)
        set(DEPLOY_TYPE "--")

        if (CMAKE_BUILD_TYPE MATCHES "(D|d)eb")
            set(DEPLOY_TYPE "${DEPLOY_TYPE}debug")
        else ()
            set(DEPLOY_TYPE "${DEPLOY_TYPE}release")
        endif ()
    endif ()

    # Run windeployqt to copy DLLs in the executable folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND ${QT5_WINDEPLOYQT_EXECUTABLE} ${DEPLOY_TYPE} $<TARGET_FILE:${PROJECT_NAME}>)
endif ()
