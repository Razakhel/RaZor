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

    AUTOUIC_SEARCH_PATHS "interface/"
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
        /MP # Enabling multi-processes compilation

        /wd4061 # Enum value in a switch not explicitly handled by a case label
        /wd4571 # SEH exceptions aren't caught since Visual C++ 7.1
        /wd5045 # Spectre mitigation
        /wd5246 # Initialization of subobject should be wrapped in braces

        # Warnings triggered by Qt
        /wd4365 # Signed/unsigned mismatch (implicit conversion)
        /wd4371 # Class layout may have changed from a previous compiler version
        /wd4619 # Unknown warning number
        /wd4625 # Copy constructor implicitly deleted
        /wd4626 # Copy assignment operator implicitly deleted
        /wd4946 # reinterpret_cast used between related classes
        /wd5219 # Implicit conversion from int to float
        /wd5220 # Non-static member with volatile type doesn't imply non-trivial copy/move ctor/operator=
        /wd5240 # nodiscard ignored

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
        /wd5204 # Class with virtual functions but no virtual destructor
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
        /Zc:__cplusplus # Forcing the __cplusplus definition to be of the proper value
    )

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16.5)
        set(
            RAZOR_COMPILER_FLAGS

            ${RAZOR_COMPILER_FLAGS}
            /Zc:preprocessor # Forcing the preprocessor to be compliant with C++11 and above
        )
    endif ()
endif ()

if (WIN32 OR CYGWIN)
    target_compile_definitions(
        RaZor

        PRIVATE

        NOMINMAX # Preventing definitions of min & max macros
        NOGDI # Preventing definition of the 'ERROR' macro
        _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING # Ignoring std::codecvt deprecation warnings
    )
endif ()

if (RAZOR_PLATFORM_LINUX)
    set(RAZOR_LINKER_FLAGS ${RAZOR_LINKER_FLAGS} pthread)
endif ()

if (RAZOR_PLATFORM_WINDOWS)
    set_target_properties(RaZor PROPERTIES WIN32_EXECUTABLE TRUE) # Removing external console
endif ()

########################
# RaZor - Source files #
########################

set(
    RAZOR_SRC

    main.cpp

    src/RaZor/*.cpp
    src/RaZor/Interface/*.cpp
    src/RaZor/Interface/Component/*.cpp

    include/RaZor/*.hpp
    include/RaZor/*.inl
    include/RaZor/Interface/*.hpp
    include/RaZor/Interface/*.inl
    include/RaZor/Interface/Component/*.hpp
    include/RaZor/Interface/Component/*.inl

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

    target_include_directories(RaZor SYSTEM PRIVATE "${RAZ_ROOT}/include")
    target_link_directories(RaZor PRIVATE "${RAZ_LIB_DIR}")
else ()
    set(RAZ_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/extern/RaZ")

    if (EXISTS "${RAZ_ROOT}")
        # No need to keep the examples, unit tests & documentation generation
        set(RAZ_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(RAZ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(RAZ_GEN_DOC OFF CACHE BOOL "" FORCE)

        # Qt will handle all the UI part; disabling windowing & overlay capabilities
        set(RAZ_USE_WINDOW OFF CACHE BOOL "" FORCE)
        set(RAZ_USE_IMGUI OFF CACHE BOOL "" FORCE)

        # Tracy's GPU profiling doesn't seem to report anything and produces a crash (in GpuCtx::NextQueryId() from ~GpuCtxScope()) for yet unknown reasons
        set(RAZ_USE_PROFILING OFF CACHE BOOL "" FORCE)

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

file(
    GLOB
    RAZOR_TRANSLATION_FILES

    assets/translations/*.ts
    interface/*.ui
)

# Creating translation files
qt5_create_translation(
    RAZOR_TRANSLATION_FILES

    ${RAZOR_FILES}
    ${RAZOR_TRANSLATION_FILES}

    OPTIONS

    -noobsolete
)

set(
    RAZOR_LINKER_FLAGS

    ${RAZOR_LINKER_FLAGS}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# Deploying Qt
if (RAZOR_PLATFORM_WINDOWS)
    get_target_property(QMAKE_EXE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(WINDEPLOYQT_PATH "${QMAKE_EXE}" PATH)
    set(WINDEPLOYQT_EXE "${WINDEPLOYQT_PATH}/windeployqt.exe")

    # Run windeployqt to copy DLLs in the executable folder
    add_custom_command(TARGET RaZor POST_BUILD
                       COMMAND "${WINDEPLOYQT_EXE}" --$<LOWER_CASE:${RAZOR_CONFIG_SHORT}> "$<TARGET_FILE:${PROJECT_NAME}>")
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
if (RAZOR_COMPILER_MSVC AND RAZOR_USE_FBX AND FBX_FOUND)
    file(COPY "${FBX_DLL}" DESTINATION "${CMAKE_BINARY_DIR}")
endif ()

#################
# RaZor - Build #
#################

target_sources(RaZor PRIVATE ${RAZOR_FILES} ${RAZOR_TRANSLATION_FILES})
target_include_directories(RaZor SYSTEM PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(RaZor PRIVATE ${RAZOR_COMPILER_FLAGS})

target_link_libraries(RaZor PUBLIC RaZ ${RAZOR_LINKER_FLAGS})

# Under Linux, linking to GL is necessary
if (RAZOR_PLATFORM_LINUX)
    target_link_libraries(RaZor PUBLIC GL)
endif ()

########################
# RaZor - Installation #
########################

# Installing the executable
if (RAZOR_PLATFORM_WINDOWS)
    set(CMAKE_INSTALL_PREFIX "C:/RaZor")
endif ()

install(TARGETS RaZor DESTINATION "${CMAKE_INSTALL_PREFIX}")

# Installing translation files
file(
    GLOB
    RAZOR_QM_FILES

    "${CMAKE_BINARY_DIR}/*.qm"
)

install(FILES ${RAZOR_QM_FILES} DESTINATION "${CMAKE_INSTALL_PREFIX}")

if (RAZOR_PLATFORM_WINDOWS)
    # Copying all necessary DLLs to launch RaZor

    set(RAZOR_DLLS_LOCATION "${CMAKE_BINARY_DIR}")

    # Visual Studio's binary directory doesn't contain the config folder suffix; it is needed & thus manually appended
    # This probably needs more research to find a better way for this...
    if (CMAKE_GENERATOR MATCHES "Visual Studio*")
        set(RAZOR_DLLS_LOCATION "${RAZOR_DLLS_LOCATION}/$<CONFIG>")
    endif ()

    if (RAZOR_COMPILER_MSVC)
        set(DLLS_SUFFIX "$<IF:$<CONFIG:Debug>,d,>") # windeployqt appends a 'd' in Debug at the end of each dll with MSVC
    endif ()

    install(
        FILES

        "${RAZOR_DLLS_LOCATION}/Qt5Core${DLLS_SUFFIX}.dll"
        "${RAZOR_DLLS_LOCATION}/Qt5Gui${DLLS_SUFFIX}.dll"
        "${RAZOR_DLLS_LOCATION}/Qt5Widgets${DLLS_SUFFIX}.dll"

        DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )

    # RaZor links itself against OpenAL32.dll
    # The system's may be automatically found, but OpenAL-soft's is installed in case the former doesn't exist
    install(FILES "${OPENAL_DLL}" RENAME "OpenAL32.dll" DESTINATION "${CMAKE_INSTALL_PREFIX}")

    # Deploying Qt adds several folders & DLLs; some are necessary for RaZor to run
    install(
        DIRECTORY

        #"${RAZOR_DLLS_LOCATION}/iconengines"
        #"${RAZOR_DLLS_LOCATION}/imageformats"
        "${RAZOR_DLLS_LOCATION}/platforms"
        "${RAZOR_DLLS_LOCATION}/styles"
        #"${RAZOR_DLLS_LOCATION}/translations"

        DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )

    # If the FBX SDK is used, install the DLL file into the install root folder
    if (RAZOR_COMPILER_MSVC AND RAZOR_USE_FBX AND FBX_FOUND)
        install(FILES "${FBX_DLL}" DESTINATION "${CMAKE_INSTALL_PREFIX}")
    endif ()
endif ()
