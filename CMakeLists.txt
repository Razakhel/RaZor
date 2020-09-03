cmake_minimum_required(VERSION 3.10)
project(RaZor)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# If the build type hasn't been specified, defaulting it to Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

#######################
# RaZor - Application #
#######################

add_executable(RaZor)

# Using C++17
target_compile_features(RaZor PRIVATE cxx_std_17)

# Defining CMake's automatic MOC/UIC/RCC generations for Qt
set_target_properties(
    RaZor

    PROPERTIES

    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

############################
# RaZor - Useful variables #
############################

if (MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang") # Finding exclusively MSVC, not clang-cl
    set(RAZOR_COMPILER "MSVC")
    set(RAZOR_COMPILER_MSVC ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_COMPILER_MSVC)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (MSVC)
        # Using clang-cl, for which both MSVC & Clang are found
        set(RAZOR_COMPILER "Clang-cl")
        set(RAZOR_COMPILER_CLANG_CL ON)
    else ()
        set(RAZOR_COMPILER "Clang")
    endif ()

    set(RAZOR_COMPILER_CLANG ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_COMPILER_CLANG)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(RAZOR_COMPILER "GCC")
    set(RAZOR_COMPILER_GCC ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_COMPILER_GCC)
endif ()

if (WIN32 OR CYGWIN)
    set(RAZOR_PLATFORM "Windows")
    set(RAZOR_PLATFORM_WINDOWS ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_PLATFORM_WINDOWS)

    if (CYGWIN)
        set(RAZOR_PLATFORM "${RAZOR_PLATFORM} (Cygwin)")
        set(RAZOR_PLATFORM_CYGWIN ON)
        target_compile_definitions(RaZor PUBLIC RAZOR_PLATFORM_CYGWIN)
    endif ()
elseif (APPLE)
    set(RAZOR_PLATFORM "macOS")
    set(RAZOR_PLATFORM_MAC ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_PLATFORM_MAC)
elseif (RAZOR_USE_EMSCRIPTEN)
    set(RAZOR_PLATFORM "Emscripten")
    set(RAZOR_PLATFORM_EMSCRIPTEN ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_PLATFORM_EMSCRIPTEN USE_OPENGL_ES)
elseif (UNIX)
    set(RAZOR_PLATFORM "Linux")
    set(RAZOR_PLATFORM_LINUX ON)
    target_compile_definitions(RaZor PUBLIC RAZOR_PLATFORM_LINUX)
endif ()

if (RAZOR_COMPILER_MSVC)
    set(RAZOR_CONFIG_DEBUG   "$<IF:$<CONFIG:Debug>,ON,OFF>")
    set(RAZOR_CONFIG_RELEASE "$<IF:$<CONFIG:Debug>,OFF,ON>")
    set(RAZOR_CONFIG_SHORT   "$<IF:$<CONFIG:Debug>,Debug,Release>")

    target_compile_definitions(RaZor PUBLIC $<IF:$<CONFIG:Debug>,RAZOR_CONFIG_DEBUG,RAZOR_CONFIG_RELEASE>)

    set(CONFIG_STR_UPPER $<UPPER_CASE:$<CONFIG>>)
else ()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(RAZOR_CONFIG_DEBUG   ON)
        set(RAZOR_CONFIG_RELEASE OFF)
        set(RAZOR_CONFIG_SHORT   "Debug")

        target_compile_definitions(RaZor PUBLIC RAZOR_CONFIG_DEBUG)
    else ()
        set(RAZOR_CONFIG_DEBUG   OFF)
        set(RAZOR_CONFIG_RELEASE ON)
        set(RAZOR_CONFIG_SHORT   "Release")

        target_compile_definitions(RaZor PUBLIC RAZOR_CONFIG_RELEASE)
    endif ()

    string(TOUPPER ${CMAKE_BUILD_TYPE} CONFIG_STR_UPPER)
endif ()

set(RAZOR_BUILD_FLAGS "${CMAKE_CXX_FLAGS_${CONFIG_STR_UPPER}}")

option(ENABLE_DEBUG_INFO "Creates a debug target which prints useful values" OFF)

if (ENABLE_DEBUG_INFO)
    cmake_host_system_information(RESULT PLATFORM_RELEASE QUERY OS_RELEASE)
    cmake_host_system_information(RESULT PLATFORM_VERSION QUERY OS_VERSION)

    # Trimming potential leading & trailing space characters (happens for Windows' values)
    string(STRIP ${PLATFORM_RELEASE} PLATFORM_RELEASE)
    string(STRIP ${PLATFORM_VERSION} PLATFORM_VERSION)

    add_custom_target(
        RaZor_PrintDebugInfo

        ${CMAKE_COMMAND} -E echo "" &&
        ${CMAKE_COMMAND} -E echo "######################" &&
        ${CMAKE_COMMAND} -E echo "# [RaZor] Debug info #" &&
        ${CMAKE_COMMAND} -E echo "######################" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Platform (found ${RAZOR_PLATFORM})" &&
        ${CMAKE_COMMAND} -E echo "  Name:     ${CMAKE_SYSTEM_NAME}" &&
        ${CMAKE_COMMAND} -E echo "  Release:  ${PLATFORM_RELEASE}" &&
        ${CMAKE_COMMAND} -E echo "  Version:  ${PLATFORM_VERSION}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Compiler (found ${RAZOR_COMPILER})" &&
        ${CMAKE_COMMAND} -E echo "  ID:       ${CMAKE_CXX_COMPILER_ID}" &&
        ${CMAKE_COMMAND} -E echo "  Version:  ${CMAKE_CXX_COMPILER_VERSION}" &&
        ${CMAKE_COMMAND} -E echo "  Path:     ${CMAKE_CXX_COMPILER}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Configuration" &&
        ${CMAKE_COMMAND} -E echo "  Name:               $<CONFIG>" &&
        ${CMAKE_COMMAND} -E echo "  RAZOR_CONFIG_DEBUG:   ${RAZOR_CONFIG_DEBUG}" &&
        ${CMAKE_COMMAND} -E echo "  RAZOR_CONFIG_RELEASE: ${RAZOR_CONFIG_RELEASE}" &&
        ${CMAKE_COMMAND} -E echo "  RAZOR_CONFIG_SHORT:   ${RAZOR_CONFIG_SHORT}" &&
        ${CMAKE_COMMAND} -E echo "" &&

        ${CMAKE_COMMAND} -E echo "--- Build flags: '${RAZOR_BUILD_FLAGS}'" &&
        ${CMAKE_COMMAND} -E echo ""
    )
endif ()

target_compile_definitions(RaZor PRIVATE RAZOR_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")

##########################
# RaZor - Compiler flags #
##########################

if (RAZOR_COMPILER_GCC)
    set(
        RAZOR_COMPILER_FLAGS

        -pedantic
        -pedantic-errors
        -Wall
        -Wextra

        -Warray-bounds
        -Wcast-align
        -Wcast-qual
        -Wconditionally-supported
        -Wconversion
        -Wdisabled-optimization
        -Wdouble-promotion
        -Wfloat-conversion
        -Wformat=2
        -Wformat-security
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnoexcept
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wopenmp-simd
        -Woverloaded-virtual
        -Wpacked
        -Wredundant-decls
        -Wstrict-aliasing
        -Wstrict-null-sentinel
        #-Wsuggest-final-methods
        #-Wsuggest-final-types
        -Wtrampolines
        -Wundef
        -Wuninitialized
        -Wunused-macros
        -Wuseless-cast
        -Wvector-operation-performance
        -Wvla
        -Wzero-as-null-pointer-constant

        -Wno-comment
        -Wno-format-nonliteral
    )

    # Enabling some other warnings available since GCC 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            -fsized-deallocation
            -Warray-bounds=2
            -Wformat-signedness
            -Wsized-deallocation
        )
    endif ()

    # Enabling some other warnings available since GCC 6
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            -Wduplicated-cond
            -Wnull-dereference
        )
    endif ()

    # Enabling some other warnings available since GCC 7
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            -Waligned-new
            -Walloca
            -Walloc-zero
            -Wformat-overflow
            -Wshadow
        )
    endif ()

    # Enabling code coverage
    option(RAZOR_ENABLE_COVERAGE "Enable code coverage (GCC only)" OFF)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND RAZOR_ENABLE_COVERAGE)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            -g
            -O0
            -fno-inline
            -fno-inline-small-functions
            -fno-default-inline
            -fprofile-arcs
            -ftest-coverage
        )

        set(
            RAZOR_LINKER_FLAGS

            gcov
        )
    endif ()
elseif (RAZOR_COMPILER_CLANG)
    set(
        RAZOR_COMPILER_FLAGS

        -Weverything

        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-documentation
        -Wno-documentation-unknown-command
        -Wno-exit-time-destructors
        -Wno-float-equal
        -Wno-format-nonliteral
        -Wno-global-constructors
        -Wno-mismatched-tags
        -Wno-missing-braces
        -Wno-padded
        -Wno-reserved-id-macro
        -Wno-sign-conversion
        -Wno-switch-enum
        -Wno-weak-vtables
    )

    if (RAZOR_COMPILER_CLANG_CL)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            # Disabling warnings triggered in externals
            -Wno-language-extension-token
            -Wno-nonportable-system-include-path
            -Wno-zero-as-null-pointer-constant
        )
    else ()
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            # Other warning flags not recognized by clang-cl
            -pedantic
            -pedantic-errors
        )
    endif ()

    # Disabling some warnings available since Clang 5
    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            -Wno-unused-template
        )
    endif ()
elseif (RAZOR_COMPILER_MSVC)
    set(
        RAZOR_COMPILER_FLAGS

        /Wall

        /wd4061 # Enum value in a switch not explicitly handled by a case label
        /wd4571 # SEH exceptions aren't caught since Visual C++ 7.1
        /wd5045 # Spectre mitigation

        # Warnings triggered by the FBX SDK
        /wd4266 # No override available (function is hidden)
        /wd4619 # Unknown warning number
        /wd4625 # Copy constructor implicitly deleted
        /wd4626 # Copy assignment operator implicitly deleted

        # Warnings triggered by Catch
        /wd4365 # Signed/unsigned mismatch (implicit conversion)
        /wd4388 # Signed/unsigned mismatch (equality comparison)
        /wd4583 # Destructor not implicitly called
        /wd4623 # Default constructor implicitly deleted
        /wd4868 # Evaluation order not guaranteed in braced initializing list
        /wd5204 # Class with virtual functions but no virtual destructor

        # Warnings triggered by MSVC's standard library
        /wd4355 # 'this' used in base member initializing list
        /wd4514 # Unreferenced inline function has been removed
        /wd4548 # Expression before comma has no effect
        /wd4668 # Preprocessor macro not defined
        /wd4710 # Function not inlined
        /wd4711 # Function inlined
        /wd4774 # Format string is not a string literal
        /wd4820 # Added padding to members
        /wd5026 # Move constructor implicitly deleted
        /wd5027 # Move assignment operator implicitly deleted
        /wd5039 # Pointer/ref to a potentially throwing function passed to an 'extern "C"' function (with -EHc)
    )

    # To automatically export all the classes & functions
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    # CMake automatically appends /W3 to the standard flags, which produces a warning with MSVC when adding another level; this has to be removed
    # TODO: if possible, this should be done per target, not globally
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

if (RAZOR_COMPILER_MSVC OR RAZOR_COMPILER_CLANG_CL)
    set(
        RAZOR_COMPILER_FLAGS

        ${RAZOR_COMPILER_FLAGS}
        /permissive- # Improving standard compliance
        /EHsc # Enabling exceptions
        /utf-8 # Forcing MSVC to actually handle files as UTF-8
    )

    target_compile_definitions(
        RaZor

        PRIVATE

        NOMINMAX # Preventing definitions of min & max macros
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # Ignoring std::codecvt deprecation warnings
    )
endif ()

########################
# RaZor - Source files #
########################

set(
    RAZOR_SRC

    main.cpp

    src/RaZor/*.cpp
    src/RaZor/Interface/*.cpp

    include/RaZor/*.hpp
    include/RaZor/Interface/*.hpp

    assets/resources/resources.qrc
)

# Adding every file to be compiled
file(
    GLOB
    RAZOR_FILES

    ${RAZOR_SRC}
)

#####################
# RaZor - RaZ usage #
#####################

# Finding RaZ
option(RAZOR_BUILD_RAZ "Build RaZ alongside RaZor (requires downloading the submodule)" OFF)

if (NOT RAZOR_BUILD_RAZ)
    set(RAZ_ROOT "C:/RaZ" CACHE PATH "Path to the RaZ installation")

    set(RAZ_LIB_DIR "${RAZ_ROOT}/lib")

    # Visual Studio having all the configurations from within the project, CMAKE_BUILD_TYPE is unknown at generation time
    # Adding a link directory automatically creates anoter path to which is appended the $(Configuration) macro, which contains the build type
    if (NOT RAZOR_COMPILER_MSVC)
        set(RAZ_LIB_DIR "${RAZ_LIB_DIR}/${CMAKE_BUILD_TYPE}")
    endif ()

    target_link_directories(RaZor PRIVATE "${RAZ_LIB_DIR}")
else ()
    set(RAZ_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/extern/RaZ")

    if (EXISTS "${RAZ_ROOT}")
        # No need to keep the examples, unit test & documentation generation
        set(RAZ_BUILD_EXAMPLES OFF)
        set(RAZ_BUILD_TESTS OFF)
        set(RAZ_GEN_DOC OFF)

        # Qt will handle all the windowing part; GLFW & ImGui must not be used
        set(RAZ_USE_GLFW OFF)
        set(RAZ_USE_IMGUI OFF)

        add_subdirectory("${RAZ_ROOT}")
    else ()
        message(FATAL_ERROR "Failed to find RaZ; the submodule must be downloaded")
    endif ()
endif ()

####################
# RaZor - Qt usage #
####################

# Finding Qt
if (RAZOR_PLATFORM_WINDOWS)
    set(QT_LOCATION "" CACHE PATH "Path to the Qt5 installation")

    if (NOT QT_LOCATION STREQUAL "")
        if (MINGW)
            set(QT_LOCATION "${QT_LOCATION}/mingw81_64") # Previously mingw73_64
        else ()
            set(QT_LOCATION "${QT_LOCATION}/msvc2019_64") # Previously msvc2017_64
        endif ()

        list(APPEND CMAKE_PREFIX_PATH "${QT_LOCATION}")
    endif ()
endif ()

find_package(Qt5 COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

# Creating translation files
qt5_create_translation(TRANSLATION_FILES ${RAZOR_FILES} assets/translations/english.ts assets/translations/french.ts)

set(
    RAZOR_LINKER_FLAGS

    ${RAZOR_LINKER_FLAGS}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Deploying Qt
if (RAZOR_PLATFORM_WINDOWS)
    get_target_property(QT5_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_QMAKE_EXECUTABLE}" PATH)
    set(QT5_WINDEPLOYQT_EXECUTABLE "${QT5_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

    # Specifying the preferred deploy mode's hint for MinGW
    if (MINGW)
        if (CMAKE_BUILD_TYPE MATCHES "(D|d)eb")
            set(DEPLOY_TYPE "--debug")
        else ()
            set(DEPLOY_TYPE "--release")
        endif ()
    endif ()

    # Run windeployqt to copy DLLs in the executable folder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                       COMMAND "${QT5_WINDEPLOYQT_EXECUTABLE}" ${DEPLOY_TYPE} $<TARGET_FILE:${PROJECT_NAME}>)
endif ()

###########################
# RaZor - FBX file format #
###########################

## Finding FBX SDK
#if (RAZOR_COMPILER_MSVC OR RAZOR_COMPILER_GCC AND NOT MINGW) # FBX SDK unavailable for MinGW, which is considered as GCC
#    option(RAZOR_USE_FBX "Allows to import/export FBX models (requires the FBX SDK to be installed)" OFF)
#
#    if (RAZOR_USE_FBX)
#        find_package(FBX)
#
#        if (FBX_FOUND)
#            set(RAZOR_LINKER_FLAGS ${RAZOR_LINKER_FLAGS} ${FBX_LIB})
#
#            message("FBX SDK linked.")
#        endif ()
#    endif ()
#endif ()
#
## If FBX SDK used, copy the DLL into the binary folder to link it properly
#if (RAZOR_USE_FBX AND FBX_FOUND AND RAZOR_COMPILER_MSVC)
#    add_custom_command(TARGET RaZor PRE_LINK
#                       COMMAND ${CMAKE_COMMAND} -E copy "${FBX_CONFIG_DLL}" "${CMAKE_BINARY_DIR}/${FBX_LIB_NAME}.dll")
#endif ()

#################
# RaZor - Build #
#################

target_sources(RaZor PRIVATE ${RAZOR_FILES} ${TRANSLATION_FILES})

target_include_directories(
    RaZor

    SYSTEM
    PRIVATE

    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${QT_LOCATION}/include"
    "${RAZ_ROOT}/include"
)

target_compile_options(RaZor PRIVATE ${RAZOR_COMPILER_FLAGS})
target_link_libraries(RaZor PUBLIC RaZ ${RAZOR_LINKER_FLAGS})
