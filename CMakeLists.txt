cmake_minimum_required(VERSION 2.6)

project( ITKTransformTools )

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
)

add_executable( ITKTransformTools ${ITKTransformTools_SRC} )

TARGET_LINK_LIBRARIES( ITKTransformTools ${ITK_LIBRARIES} )
