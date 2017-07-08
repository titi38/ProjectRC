#
# Find the omniORB libraries and include dir
#

# OMNIORB4_INCLUDE_DIR  - Directories to include to use omniORB
# OMNIORB4_LIBRARIES    - Files to link against to use omniORB
# OMNIORB4_CLIENT_LIBRARIES    - Files to link against if you included an omniORB header
# OMNIORB4_IDL_COMPILER
# OMNIORB4_FOUND        - When false, don't try to use omniORB
# OMNIORB4_DIR          - (optional) Suggested installation directory to search
# OMNIORB_IDL_FLAGS     - (optional) Set flags for to use corba
# OMNIORB_CPP_FLAGS     - (optional) Set flags for to use corba
#
# OMNIORB4_DIR can be used to make it simpler to find the various include
# directories and compiled libraries when omniORB was not installed in the
# usual/well-known directories (e.g. because you made an in tree-source
# compilation or because you installed it in an "unusual" directory).
# Just set OMNIORB4_DIR it to your specific installation directory
#

FIND_PATH( OMNIORB4_INCLUDE_DIR omniORB4/CORBA.h
  PATHS
  ${OMNIORB4_DIR}/include
  /usr/include
  /usr/local/include
)

SET( OMNIORB4_DEFAULT_LIB_PATH /usr/lib /usr/local/lib )
#### For the list of required libraries for omniORB see
# http://www.omniorb-support.com/pipermail/omniorb-list/2005-May/026666.html
# Basically, look at
#  - omniORB-4.0.5/README.*
#  - omniORB-4.0.5/readmes/*
# Platfrom dependencies might (only?) happen for Win32/VC++ (sigh):
# "Because of a bug in VC++, unless you require the dynamic interfaces, it
#  is suggested that you use a smaller stub library called msvcstub.lib."

IF(WIN32)
  SET (OMNIORB4_LIB_DIR_WIN32 ${OMNIORB4_DIR}/lib/x86_win32)
  SET (OMNIORB4_BIN_DIR_WIN32 ${OMNIORB4_DIR}/bin/x86_win32)
  IF(BUILD_SHARED_LIBS)
     SET (OMNIORB4_LIB_EXT 10_rt)
     SET (OMNITHREAD_LIB_EXT 33_rt)
  ENDIF(BUILD_SHARED_LIBS)
#  MESSAGE(${OMNITHREAD_LIB_EXT})
ENDIF(WIN32)

FIND_LIBRARY(  OMNIORB4_LIBRARY_omniORB4 omniORB4${OMNIORB4_LIB_EXT}
  PATHS ${OMNIORB4_DIR}/lib
        ${OMNIORB4_LIB_DIR_WIN32}
        ${OMNIORB4_DEFAULT_LIB_PATH}
)

FIND_LIBRARY( OMNIORB4_LIBRARY_omnithread omnithread${OMNITHREAD_LIB_EXT}
  PATHS ${OMNIORB4_DIR}/lib
        ${OMNIORB4_LIB_DIR_WIN32}
        ${OMNIORB4_DEFAULT_LIB_PATH}
)

FIND_LIBRARY( OMNIORB4_LIBRARY_COS4 COS4${OMNIORB4_LIB_EXT}
  PATHS ${OMNIORB4_DIR}/lib
        ${OMNIORB4_LIB_DIR_WIN32}
        ${OMNIORB4_DEFAULT_LIB_PATH}
)

FIND_LIBRARY( OMNIORB4_LIBRARY_COSDynamic4 COSDynamic4${OMNIORB4_LIB_EXT}
  PATHS ${OMNIORB4_DIR}/lib
        ${OMNIORB4_LIB_DIR_WIN32}
        ${OMNIORB4_DEFAULT_LIB_PATH}
)

FIND_LIBRARY( OMNIORB4_LIBRARY_Dynamic4 omniDynamic4${OMNIORB4_LIB_EXT}
  PATHS ${OMNIORB4_DIR}/lib
        ${OMNIORB4_LIB_DIR_WIN32}
        ${OMNIORB4_DEFAULT_LIB_PATH}
)

FIND_PROGRAM(OMNIORB4_IDL_COMPILER
  NAMES omniidl
  PATHS ${OMNIORB4_DIR}/bin
        ${OMNIORB4_LIB_DIR_WIN32}
        /usr/bin
        /usr/local/bin
  DOC "What is the path where omniidl (the idl compiler) can be found"
)

IF( OMNIORB4_INCLUDE_DIR )
IF( OMNIORB4_LIBRARY_omniORB4 )
IF( OMNIORB4_LIBRARY_omnithread )
IF( OMNIORB4_IDL_COMPILER )

  SET( OMNIORB4_FOUND "YES" )
  SET(CORBA_IS_OMNIORB 1)
  # OMNIORB4_CLIENT_LIBRARIES
  list(APPEND OMNIORB4_CLIENT_LIBRARIES ${OMNIORB4_LIBRARY_omniORB4} ${OMNIORB4_LIBRARY_omnithread} )

  SET(OMNIORB4_IDL_FLAGS "-bcxx -Wba -Wbh=.hh -Wbs=SK.cc -Wbd=DynSK.cc" CACHE STRING "Option for the omniorb4 idl compiler")
  SET(OMNIORB4_CPP_FLAGS "-D__OMNIORB4__ -DOMNIORB_POA" CACHE STRING "Additional option for the linking with omniorb4")

  IF(WIN32)
    SET(OMNIORB4_CPP_FLAGS "${OMNIORB4_CPP_FLAGS} -mthreads -D__WIN32__ -D_WIN32_WINNT=0x0400 -D__x86__ -D__NT__  -D__OSVERSION__=4")
  ELSE (WIN32)  
  IF(${APPLE})
    SET(OMNIORB4_CPP_FLAGS "${OMNIORB4_CPP_FLAGS} -D__OSVERSION__=1 -D__darwin__ -D__x86__")
  ELSE (${APPLE})
  IF(${UNIX})
    SET(OMNIORB4_CPP_FLAGS "${OMNIORB4_CPP_FLAGS} -D__OSVERSION__=2 -D__linux__ -DLINUX -D__x86__")
  ENDIF(${UNIX})
  ENDIF(${APPLE})
  ENDIF(WIN32)

  SEPARATE_ARGUMENTS(OMNIORB_IDL_FLAGS)
  SEPARATE_ARGUMENTS(OMNIORB_CPP_FLAGS)

  MARK_AS_ADVANCED( OMNIORB4_DIR )
  MARK_AS_ADVANCED( OMNIORB4_INCLUDE_DIR )
  MARK_AS_ADVANCED( OMNIORB4_LIBRARY_omniORB4 )
  MARK_AS_ADVANCED( OMNIORB4_LIBRARY_omnithread )
  MARK_AS_ADVANCED( OMNIORB4_LIBRARY_COS4 )
  MARK_AS_ADVANCED( OMNIORB4_LIBRARY_COSDynamic4 )
  MARK_AS_ADVANCED( OMNIORB4_LIBRARY_Dynamic4 )
  MARK_AS_ADVANCED( OMNIORB4_IDL_COMPILER )
  MARK_AS_ADVANCED( OMNIORB4_IDL_FLAGS )
  MARK_AS_ADVANCED( OMNIORB4_CPP_FLAGS )

  LIST(APPEND OMNIORB4_LIBRARIES
    ${OMNIORB4_LIBRARY_omniORB4}
    ${OMNIORB4_LIBRARY_Dynamic4}
    ${OMNIORB4_LIBRARY_omnithread})
  IF(OMNIORB4_LIBRARY_COS4)
    LIST(APPEND OMNIORB4_LIBRARIES
        ${OMNIORB4_LIBRARY_COS4}            # mandatory on FC2 and graal
        ${OMNIORB4_LIBRARY_COSDynamic4}     # mandatory on FC2 and graal
        )
  ENDIF(OMNIORB4_LIBRARY_COS4)
ENDIF( OMNIORB4_IDL_COMPILER )
ENDIF( OMNIORB4_LIBRARY_omnithread )
ENDIF( OMNIORB4_LIBRARY_omniORB4 )
ENDIF( OMNIORB4_INCLUDE_DIR )

IF( OMNIORB4_FOUND )
  SET(CORBA_LIBRARIES ${OMNIORB4_LIBRARIES})
  SET(CORBA_LDFLAGS -l${OMNIORB4_LIBRARY_omniORB4} -l${OMNIORB4_LIBRARY_Dynamic4} -l${OMNIORB4_LIBRARY_omnithread})
  IF(OMNIORB4_LIBRARY_COS4)
    LIST(APPEND OMNIORB4_LDFLAGS
        -l${OMNIORB4_LIBRARY_COS4}            # mandatory on FC2 and graal
        -l${OMNIORB4_LIBRARY_COSDynamic4}     # mandatory on FC2 and graal
        )
  ENDIF(OMNIORB4_LIBRARY_COS4)
  SET(CORBA_CFLAGS ${OMNIORB4_CPP_FLAGS})
  SET(CORBA_INCLUDE_DIRS ${OMNIORB4_INCLUDE_DIR})
  SET(CORBA_DEFINES "") #-DCORBA_IS_OMNIORB)

ELSE( OMNIORB4_FOUND )
  MESSAGE("No (complete) omniORB installation was not found. Please provide OMNIORB4_DIR:")
  MESSAGE("  - through the GUI when working with ccmake, ")
  MESSAGE("  - as a command line argument when working with cmake e.g. ")
  MESSAGE("    cmake .. -DOMNIORB4_DIR:PATH=/usr/local/omniORB-4.0.5 ")
  MESSAGE("Reason:")
  MESSAGE("  OMNIORB4_INCLUDE_DIR: ${OMNIORB4_INCLUDE_DIR}")
  MESSAGE("  OMNIORB4_LIBRARY_omniORB4: ${OMNIORB4_LIBRARY_omniORB4}" )
  MESSAGE("  OMNIORB4_LIBRARY_omnithread: ${OMNIORB4_LIBRARY_omnithread}" )
  MESSAGE("  OMNIORB4_IDL_COMPILER: ${OMNIORB4_IDL_COMPILER}" )

  SET( OMNIORB4_DIR "" CACHE PATH "Root of omniORB instal tree." )
ENDIF( OMNIORB4_FOUND )

# Bail if we were required to find all components and missed at least one
IF (OMNIORB4_FIND_REQUIRED AND NOT OMNIORB4_FOUND)
    MESSAGE (FATAL_ERROR "Could not find OMNIORB4")
    MESSAGE("Reason:")
    MESSAGE("  OMNIORB4_INCLUDE_DIR: ${OMNIORB4_INCLUDE_DIR}")
    MESSAGE("  OMNIORB4_LIBRARY_omniORB4: ${OMNIORB4_LIBRARY_omniORB4}" )
    MESSAGE("  OMNIORB4_LIBRARY_omnithread: ${OMNIORB4_LIBRARY_omnithread}" )
    MESSAGE("  OMNIORB4_IDL_COMPILER: ${OMNIORB4_IDL_COMPILER}" )
ENDIF ()



set (OMNIIDL_DESTINATION_DIRECTORY ${PROJECT_SOURCE_DIR})

# OMNIIDL_INCLUDE_DIRECTORIES
# ---------------------------
#
# Set include directories for omniidl
#
# DIRECTORIES: a list of directories to search for idl files.
#
MACRO (OMNIIDL_INCLUDE_DIRECTORIES DIRECTORIES)
  SET (_OMNIIDL_INCLUDE_FLAG "")
  FOREACH (DIR ${DIRECTORIES})
    SET (_OMNIIDL_INCLUDE_FLAG ${_OMNIIDL_INCLUDE_FLAG} -I${DIR})
  ENDFOREACH ()
ENDMACRO ()

# GENERATE_IDL_FILE FILENAME DIRECTORY
# ------------------------------------
#
# Generate stubs from an idl file.
# An include directory can also be specified.
#
# FILENAME : IDL filename without the extension
# DIRECTORY : IDL directory
#
MACRO(GENERATE_IDL_FILE FILENAME DIRECTORY)

  ADD_CUSTOM_COMMAND(
    OUTPUT ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}SK.cc ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}.hh
    COMMAND ${OMNIORB4_IDL_COMPILER}
    ARGS -bcxx -C${OMNIIDL_DESTINATION_DIRECTORY} ${_OMNIIDL_INCLUDE_FLAG} ${DIRECTORY}/${FILENAME}.idl
    MAIN_DEPENDENCY ${DIRECTORY}/${FILENAME}.idl
    )
  SET(ALL_IDL_STUBS ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}SK.cc ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}.hh ${ALL_IDL_STUBS})
  SET(ALL_IDL_SKELS ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}SK.cc ${ALL_IDL_STUBS})

  # Clean generated files.
  SET_PROPERTY(
    DIRECTORY APPEND PROPERTY
    ADDITIONAL_MAKE_CLEAN_FILES
    ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}SK.o ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}SK.cc ${OMNIIDL_DESTINATION_DIRECTORY}/${FILENAME}.hh
    )

  LIST(APPEND LOGGING_WATCHED_VARIABLES OMNIIDL ALL_IDL_STUBS)
ENDMACRO(GENERATE_IDL_FILE FILENAME DIRECTORY)


# GENERATE_LIST_IDL_FILES FROM FILENAMES
# --------------------------------------

MACRO (GENERATE_LIST_IDL_FILES FILENAMES)

  FOREACH (IDL_FILE ${FILENAMES})
    GET_FILENAME_COMPONENT(dir_path ${IDL_FILE} PATH)
    GET_FILENAME_COMPONENT(file_we ${IDL_FILE} NAME_WE)
    GENERATE_IDL_FILE( ${file_we} ${dir_path} )
  ENDFOREACH ()

ENDMACRO ()





