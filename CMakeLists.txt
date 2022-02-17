# ==== Define cmake build policies that affect compilation and linkage default behaviors
#
# Set the NEWEST_VALIDATED_POLICIES_VERSION string to the newest cmake version
# policies that provide successful builds. By setting NEWEST_VALIDATED_POLICIES_VERSION
# to a value greater than the oldest policies, all policies between
# OLDEST_VALIDATED_POLICIES_VERSION and CMAKE_VERSION (used for this build)
# are set to their NEW behaivor, thereby suppressing policy warnings related to policies
# between the OLDEST_VALIDATED_POLICIES_VERSION and CMAKE_VERSION.
#
# CMake versions greater than the NEWEST_VALIDATED_POLICIES_VERSION policies will
# continue to generate policy warnings "CMake Warning (dev)...Policy CMP0XXX is not set:"
#
set(OLDEST_VALIDATED_POLICIES_VERSION "3.16.3")
set(NEWEST_VALIDATED_POLICIES_VERSION "3.19.7")
cmake_minimum_required(VERSION ${OLDEST_VALIDATED_POLICIES_VERSION}...${NEWEST_VALIDATED_POLICIES_VERSION} FATAL_ERROR)

#-----------------------------------------------------------------------------
# Enable C++14
#-----------------------------------------------------------------------------
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
message(STATUS "${_msg} - C++${CMAKE_CXX_STANDARD}")
if(NOT CMAKE_CXX_STANDARD MATCHES "^(14|17|20)$")
  message(FATAL_ERROR "CMAKE_CXX_STANDARD must be set to 14, 17, or 20")
endif()

project( ITKTransformTools )



if(NOT SETIFEMPTY)
macro(SETIFEMPTY)
  set(KEY ${ARGV0})
  set(VALUE ${ARGV1})
  if(NOT ${KEY})
    set(${KEY} ${VALUE})
  endif(NOT ${KEY})
endmacro(SETIFEMPTY KEY VALUE)
endif(NOT SETIFEMPTY)
###
SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)


find_package(ITK 5.3 REQUIRED)
include(${ITK_USE_FILE})

SET( ITKTransformTools_SRC
AffineTransformCentered2NotCentered.h
AffineTransformCentered2NotCentered.cpp
CombineITKTransforms.h
CombineITKTransforms.cpp
ComposeAffineTransforms.h
ComposeAffineTransforms.cpp
GetNewSizeAndOrigin.h
GetNewSizeAndOrigin.cxx
DirectionToTransformMatrix.h
DirectionToTransformMatrix.cpp
ITKTransformTools.cpp
FreeSurferTransforms.cpp
FreeSurferTransforms.h
MatrixOffsetToAffine.h
MatrixOffsetToAffine.cpp
PrintITKTransform.cpp
PrintITKTransform.h
RotationExtraction.h
RotationExtraction.cpp
ConvertTransform.h
ConvertTransform.cpp
ConcatenateTransforms.cpp
ConcatenateTransforms.h
Invert.h
Invert.cpp
dtiprocessFiles/deformationfieldio.h
dtiprocessFiles/deformationfieldio.cxx
dtiprocessFiles/itkHFieldToDeformationFieldImageFilter.h
dtiprocessFiles/itkHFieldToDeformationFieldImageFilter.txx
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( ITKTransformTools ${ITKTransformTools_SRC} )

TARGET_LINK_LIBRARIES( ITKTransformTools ${ITK_LIBRARIES} )

install(TARGETS ITKTransformTools RUNTIME DESTINATION ${INSTALL_RUNTIME_DESTINATION} )
