cmake_minimum_required(VERSION 3.1)
project(Sample CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# C++ Compiler Version Requirements
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.4)
    message(FATAL_ERROR "GCC version must be at least 4.4!")
  endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.1)
    message(FATAL_ERROR "Clang version must be at least 3.1!")
  endif()
endif()


set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


file(GLOB SRCS *.c *.cpp *.cxx *.cc)
add_executable(
  main
  ${SRCS})

include_directories(../include/)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "i686.*|i386.*|x86.*")
  set(SYSTEM_PROCESSOR_IS_X86 TRUE)
endif()
if (CMAKE_SYSTEM_PROCESSOR MATCHES "amd64.*|x86_64.*|AMD64.*")
  set(SYSTEM_PROCESSOR_IS_X64 TRUE)
endif()


if(MSVC)
  set(USE_RELATIVE_PATHS ON)

  foreach(TARGET_FLAG
      CXX_FLAGS
      CXX_FLAGS_DEBUG
      CXX_FLAGS_RELEASE
      CXX_FLAGS_MINSIZEREL
      CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/RTC[^ ]*" "" ${TARGET_FLAG} "${CMAKE_${TARGET_FLAG}}")
  endforeach(TARGET_FLAG)

  if(CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" ${CXX_FLAGS} "${${CXX_FLAGS}}")
  else()
    set("${CXX_FLAGS}" "${${CXX_FLAGS}} /W4")
  endif()

  set(DEFLIST "-DWIN32_LEAN_AND_MEAN;-DNOMINMAX;-D_USE_MATH_DEFINES;-D_CRT_NONSTDC_NO_WARNINGS;-D_CRT_SECURE_NO_WARNINGS")

  set(CXX_FLAGS_DEBUG "${CXX_FLAGS_DEBUG} /Oi /Oy")
  set(CXX_FLAGS_RELEASE "${CXX_FLAGS_RELEASE} /Ob2 /Oi /Ot /Ox /Oy /GL")
  set(CXX_FLAGS_MINSIZEREL "${CXX_FLAGS_MINSIZEREL} /Os")

  set(EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
  foreach(WARNING_FLAG
      -Wall
      -Wextra
      -Wabi
      -Wcast-align
      -Wcast-qual
      -Wconversion
      -Wdisabled-optimization
      -Wfloat-equal
      -Wformat=2
      -Winit-self
      -Winvalid-pch
      -Wmissing-declarations
      -Wpointer-arith
      -Wredundant-decls
      -Wstack-protector
      -Wstrict-aliasing=2
      -Wstrict-overflow=5
      -Wswitch-enum
      -Wswitch-default
      -Wunknown-pragmas
      -Wunreachable-code
      -Wwrite-strings
      -pedantic)
    set(GNU_CLANG_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} ${WARNING_FLAG}")
  endforeach(WARNING_FLAG)

  option(ENABLE_ADDITIONAL_WARNING_FLAGS "Enable additional warning flags." OFF)
  if(ENABLE_ADDITIONAL_WARNING_FLAGS)
    set(GNU_CLANG_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Winline -Wsign-conversion")
  endif()

  set(GNU_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Wlogical-op")
  if(ENABLE_ADDITIONAL_WARNING_FLAGS)
    set(GNU_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Wunsafe-loop-optimizations")
  endif()

  set(CLANG_COMMON_WARNING_FLAGS "${GNU_CLANG_COMMON_WARNING_FLAGS} -Wsign-promo")

  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CXX_WARNING_FLAGS "${GNU_COMMON_WARNING_FLAGS}")
    foreach(WARNING_FLAG
        -Weffc++
        -Wctor-dtor-privacy
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wreorder
        -Wsign-promo
        -Wstrict-null-sentinel)
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
    endforeach(WARNING_FLAG)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.3 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.3)
      message("-- Add warning flags implemented in g++ 4.3")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wc++0x-compat")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.6 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.6)
      message("-- Add warning flags implemented in g++ 4.6")
      foreach(WARNING_FLAG
          -Wdouble-promotion
          -Wnoexcept
          -Wsuggest-attribute=const
          -Wsuggest-attribute=noreturn
          -Wsuggest-attribute=pure
          -Wtrampolines)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.7 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.7)
      message("-- Add warning flags implemented in g++ 4.7")
      string(REGEX REPLACE "-Wc\\+\\+0x-compat" "-Wc++11-compat" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      foreach(WARNING_FLAG
          -Wdelete-non-virtual-dtor
          -Wvector-operation-performance
          -Wno-return-local-addr
          -Wzero-as-null-pointer-constant)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.8 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.8)
      message("-- Add warning flags implemented in g++ 4.8")
      foreach(WARNING_FLAG
          -Wsuggest-attribute=format
          -Wuseless-cast)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9)
      message("-- Add warning flags implemented in g++ 4.9")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wconditionally-supported")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5.1)
      message("-- Add warning flags implemented in g++ 5.1")
      foreach(WARNING_FLAG
          -Wc++14-compat
          -Wformat-signedness
          -Wsuggest-final-methods
          -Wsuggest-final-types
          -Wsuggest-override)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6.0)
      message("-- Add warning flags implemented in g++ 6.0")
      foreach(WARNING_FLAG
          -Wduplicated-cond
          -Wplacement-new=2
          -Wshift-overflow=2)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 7.0)
      message("-- Add warning flags implemented in g++ 7.0")
      string(REGEX REPLACE "-Wabi" "-Wabi=11" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      foreach(WARNING_FLAG
          -Wc++17-compat
          -Walloc-zero
          -Wduplicated-branches
          -Wformat-overflow=2
          -Wformat-truncation=2
          -Wregister
          -Wrestrict
          -Wshadow-local
          -Wstringop-overflow=4)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 8.0)
      message("-- Add warning flags implemented in g++ 8.0")
      string(REGEX REPLACE "-Wcast-align" "-Wcast-align=strict" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wsuggest-attribute=malloc")
    endif()

    set(CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe ${CXX_WARNING_FLAGS}")
    set(CXX_FLAGS_DEBUG "-g3 -O0 -ftrapv -fstack-protector-all -D_FORTIFY_SOURCE=2 -D_GLIBCXX_DEBUG")
    set(CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CXX_FLAGS_MINSIZEREL "-s -DNDEBUG")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
      set(CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG")
    else()
      set(CXX_FLAGS_RELWITHDEBINFO "-g -Og -DNDEBUG")
    endif()
    if(SYSTEM_PROCESSOR_IS_X86 OR SYSTEM_PROCESSOR_IS_X64)
      set(CXX_FLAGS_RELEASE "${CXX_FLAGS_RELEASE} -mtune=native -march=native")
      set(CXX_FLAGS_MINSIZEREL "${CXX_FLAGS_MINSIZEREL} -mtune=native -march=native")
    endif()
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CXX_WARNING_FLAGS "${CLANG_COMMON_WARNING_FLAGS}")
    foreach(WARNING_FLAG
        -Wc++11-compat
        -Weffc++
        -Wctor-dtor-privacy
        -Wdelete-non-virtual-dtor
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wreorder)
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
    endforeach(WARNING_FLAG)

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.5 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.5)
      message("-- Add warning flags implemented in clang++ 3.5")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wc++14-compat")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.8 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 3.8)
      message("-- Add warning flags implemented in clang++ 3.8")
      foreach(WARNING_FLAG
          -Wc++1z-compat
          -Wdouble-promotion)
        set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} ${WARNING_FLAG}")
      endforeach(WARNING_FLAG)
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.0 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 5.0)
      message("-- Add warning flags implemented in clang++ 5.0")
      string(REGEX REPLACE "-Wc\\+\\+1z-compat" "-Wc++17-compat" CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS}")
      set(CXX_WARNING_FLAGS "${CXX_WARNING_FLAGS} -Wzero-as-null-pointer-constant")
    endif()

    set(CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe ${CXX_WARNING_FLAGS}")
    set(CXX_FLAGS_DEBUG "-g3 -O0 -ftrapv -fstack-protector-all -D_FORTIFY_SOURCE=2 -D_GLIBCXX_DEBUG")
    set(CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CXX_FLAGS_MINSIZEREL "-s -DNDEBUG")
    if(SYSTEM_PROCESSOR_IS_X86 OR SYSTEM_PROCESSOR_IS_X64)
      set(CXX_FLAGS_RELEASE "${CXX_FLAGS_RELEASE} -mtune=native -march=native")
      set(CXX_FLAGS_MINSIZEREL "${CXX_FLAGS_MINSIZEREL} -mtune=native -march=native")
    endif()
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.0)
      set(CXX_FLAGS_RELWITHDEBINFO "-g -O2 -DNDEBUG")
    else()
      set(CXX_FLAGS_RELWITHDEBINFO "-g -Og -DNDEBUG")
    endif()
  else()
    set(CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    set(CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    set(CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
    set(CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
  endif()

  set(EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -Wl,-O1")
  set(EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} -s")
endif()

if(DEFLIST)
  foreach(DEF "${DEFLIST}")
    add_definitions(${DEF})
  endforeach(DEF)
endif()

foreach(TARGET_FLAG
    CXX_FLAGS
    CXX_FLAGS_DEBUG
    CXX_FLAGS_RELEASE
    CXX_FLAGS_RELWITHDEBINFO
    CXX_FLAGS_MINSIZEREL
    EXE_LINKER_FLAGS
    EXE_LINKER_FLAGS_DEBUG
    EXE_LINKER_FLAGS_RELEASE
    EXE_LINKER_FLAGS_RELWITHDEBINFO
    EXE_LINKER_FLAGS_MINSIZEREL)
  string(REGEX REPLACE "^ +" "" "${TARGET_FLAG}" "${${TARGET_FLAG}}")
  string(REGEX REPLACE "  +" " " "CMAKE_${TARGET_FLAG}" "${${TARGET_FLAG}}")
endforeach(TARGET_FLAG)


option(ENABLE_TESTING "Enable testing with Google Test." OFF)
if(ENABLE_TESTING)
  enable_testing()
  include(cmake/gtest.cmake)
  add_subdirectory(test)
endif()

option(ENABLE_DOXYGEN "Enable to generate document with Doxygen." OFF)
if(ENABLE_DOXYGEN)
  include(cmake/doxygen.cmake)
  add_doxygen(main)
endif()


message(STATUS "Variables")

foreach(VARNAME
    CMAKE_SYSTEM_NAME
    CMAKE_SYSTEM_PROCESSOR
    CMAKE_SOURCE_DIR
    CMAKE_BINARY_DIR
    CMAKE_BUILD_TYPE
    CMAKE_CONFIGURATION_TYPES
    CMAKE_COMPILER_IS_GNUCC
    CMAKE_COMPILER_IS_GNUCXX
    CMAKE_CXX_COMPILER
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_CXX_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_MINSIZEREL
    CMAKE_EXE_LINKER_FLAGS
    CMAKE_EXE_LINKER_FLAGS_DEBUG
    CMAKE_EXE_LINKER_FLAGS_RELEASE
    CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
    CMAKE_EXE_LINKER_FLAGS_MINSIZEREL)
  message(STATUS "${VARNAME}: ${${VARNAME}}")
endforeach(VARNAME)
