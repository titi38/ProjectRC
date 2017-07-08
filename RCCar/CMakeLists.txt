#####################  CMakeLists for eSensor build   ############################
#  Written by T.DESCOMBES  (descombes@lpsc.in2p3.fr)                          #
#                                                                             #
#  To create an Xcode project:                                                #
#         cmake -G Xcode .                                                    #

###############              cmake version          ###########################
cmake_minimum_required(VERSION 2.6)

####################         Project name           ###########################
project(RCCar)

#################            version variables           ######################
set(RCCAR_VERSION "1.0")


##################           Minimal flags            #########################
IF(${UNIX})
  set (CMAKE_CXX_FLAGS "-Wall -O4 -g -std=c++11 -DDEBUG_TRACES -DLINUX -Wall -Wno-unused -fexceptions -fPIC -D_REENTRANT -DRCCAR_VERSION=\"\\\"${RCCAR_VERSION}\\\"\"")
ENDIF(${UNIX})

IF(${APPLE})
  set (CMAKE_CXX_FLAGS "-Wall -fPIC -fno-common -O3 -g -std=c++11 -DDEBUG_TRACES -D__darwin__ -D__x86__ -fPIC -D_REENTRANT -DRCCAR_VERSION=\"\\\"${RCCAR_VERSION}\\\"\"")
ENDIF(${APPLE})

IF(${ETRAX})
  set (CMAKE_CXX_COMPILER gcc-cris)
  set (CMAKE_CXX_FLAGS "-DOMNIORB_POA -mlinux -isystem /home/descombt/MCM/axis/devboard_lx/target/cris-axis-linux-gnu/include -enable-threads -Wall -O2  -D__OMNIORB4__ -D_REENTRANT -D__etrax__ -D__linux__ -D__OSVERSION__=2 -DSENSOR_SOFTWARE_VERSION=\"\\\"${RCCAR_VERSION}\\\"\" -DSENSOR_SENSOR_TYPE=\"Diaphane\"")
  set (CMAKE_EXE_LINKER_FLAGS "-mlinux -isystem /data/axis/devboard_lx/target/cris-axis-linux-gnu/include -Wall -Wno-unused  -L/home/descombt/MCM/axis/e100lx_mcm/target/cris-axis-linux-gnu/lib -s")
  set (ORB_DEFS "-D__OMNIORB4__ -D_REENTRANT -D__etrax__ -D__linux__ -D__OSVERSION__=2")
ENDIF(${ETRAX})

if(NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH 0)
endif()

#########   Check the compiler and set the compile and link flags  ############
set(CMAKE_BUILD_TYPE Debug)

##################          cmake Module directory       ######################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/../cmake.modules/")

set (PROJECT_SOURCE_DIR src)
set (CONTROLER_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../RCControler/src)
set (COMMON_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../../daqCommonParts )
set (PROJECT_SKELS_STUBS_DIR ${PROJECT_SOURCE_DIR}/genIDL )

#######################          /lib dir        ##############################

find_package(OMNIORB REQUIRED)
include_directories(${OMNIORB4_INCLUDE_DIR})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OMNIORB4_CPP_FLAGS}")

find_package(Libnavajo REQUIRED)
include_directories(${LIBNAVAJO_INCLUDE_DIRS})

find_package(Threads REQUIRED)

include_directories ( 
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/RCCarItf
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/genIDL
  ${PROJECT_SOURCE_DIR}/../..
  ${CONTROLER_SOURCE_DIR}
  )

###################             Sources files           #######################

file(GLOB sources_cpp 
  ${PROJECT_SOURCE_DIR}/../../AcquisitionORB/AcquisitionORB.cc
  ${PROJECT_SOURCE_DIR}/RCCarItf/RCCarItf_i.cc
  ${PROJECT_SOURCE_DIR}/RCCar.cc
  )

set(sources_idl 
  ${PROJECT_SOURCE_DIR}/RCCarItf/RCCarItf.idl
  ${CONTROLER_SOURCE_DIR}/RCControlerItf/RCControlerItf.idl
  )

set(include_directories_idl 
  ${PROJECT_SOURCE_DIR}/RCCarItf
  ${CONTROLER_SOURCE_DIR}/RCControlerItf
  )
  
###################          Output directory          ########################

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

#################          IDL Precompilation          ########################

OMNIIDL_INCLUDE_DIRECTORIES ( "${include_directories_idl}" )

file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/genIDL)
set (OMNIIDL_DESTINATION_DIRECTORY ${PROJECT_SOURCE_DIR}/genIDL )

GENERATE_LIST_IDL_FILES( "${sources_idl}" )

#########################  Binary generation ##################################

set_source_files_properties(${ALL_IDL_SKELS}
                            PROPERTIES GENERATED TRUE)

add_executable( rccar ${ALL_IDL_SKELS} ${sources_cpp} )

target_link_libraries(rccar ${OMNIORB4_CLIENT_LIBRARIES})
target_link_libraries(rccar ${CMAKE_THREAD_LIBS_INIT})


######################### document file generation ############################

