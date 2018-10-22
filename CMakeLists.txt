cmake_minimum_required(VERSION 2.6)

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


find_package(ITK REQUIRED)
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

file(GLOB LIBRARY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.txx ${CMAKE_CURRENT_SOURCE_DIR}/*.hxx ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set_target_properties(ITKTransformTools PROPERTIES
  PUBLIC_HEADER "${LIBRARY_HEADERS}"
  )

set(INSTALL_CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake/ITKTransformTools)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)

install( TARGETS ITKTransformTools 
  EXPORT ITKTransformToolsTargets
  RUNTIME DESTINATION ${INSTALL_RUNTIME_DESTINATION} 
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}"
  )

### This is the config file for niral_utilities
# Create the FooBarConfig.cmake and FooBarConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")

set(CONF_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
# ... for the build tree
configure_file(ITKTransformToolsConfig.cmake.in
  "${PROJECT_BINARY_DIR}/ITKTransformToolsConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${ITKTransformTools_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(ITKTransformToolsConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ITKTransformToolsConfig.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ITKTransformToolsConfig.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

export(TARGETS ITKTransformTools
  FILE "${PROJECT_BINARY_DIR}/ITKTransformToolsTargets.cmake"
  )

install(EXPORT ITKTransformToolsTargets DESTINATION
"${INSTALL_CMAKE_DIR}" COMPONENT dev)
