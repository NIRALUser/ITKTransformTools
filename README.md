# ITKTransformTools

## What is it?

This is an open source package to make operations with ITKTransforms it can be used in the command line to 
combine, compose, concatenate, invert, or do operations such as scaling. 
The detail of the operations are listed. 

### List of operations

* ITKTransformTools combine ... Combines ITK transforms from several files into one file
* ITKTransformTools compose ... Composes 2 affine ITK transforms into one transform
* ITKTransformTools centered ... Adds an offset equivalent to centering the source and the target image to a transform
* ITKTransformTools size ... Computes the size and the origin of the transformed image
* ITKTransformTools iso ... Computes the size and the origin of the isotropic image (based on the smallest spacing)
* ITKTransformTools scale ... Computes the grids to resample an image and an atlas with the same isotropic spacing
* ITKTransformTools spacing ... Computes the grid to resample an image to a reference image using the spacing of another image
* ITKTransformTools direction ... Computes the transform to resample an image with an identity direction matrix
* ITKTransformTools freesurfer ... Converts FreeSurfer affine transforms to itkTransforms
* ITKTransformTools MO2Aff ... Modifies the transform file created by ANTs from a MatrixOffsetTransformBase_double_3_3 type to an AffineTransform_double_3_3
* ITKTransformTools print ... Prints the transformation matrix and the offset of the transform
* ITKTransformTools rotation ... Extracts the rotation from the affine transform (Finite Strain)
* ITKTransformTools convert ... Converts transforms from double->float or float->double
* ITKTransformTools concatenate ... Concatenates transforms
* ITKTransformTools invert ... inverts a transform
* ITKTransformTools version ... prints version number