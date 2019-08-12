#pragma once
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkAlgorithmOutput.h>
#include <vtkImageData.h>
#include <vtkDataSetAlgorithm.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include "DicomSeriesWriterFromVtkImageDataClass.h"
#include "npy.hpp"

class MathUtilsClass
{
public:
	MathUtilsClass();
	~MathUtilsClass();
	static vtkMatrix4x4 * getRotateMatrix(double angle_x, double angle_y, double angle_z);
	static void getRotateTrasform(
		vtkAlgorithmOutput* outputPort,
		vtkDICOMMetaData * MetaData,
		double angle_x, 
		double angle_y, 
		double angle_z);

	static vtkDataSetAlgorithm *  LoadArrayFromNumpy(string fileName,
				int ScalarType,
				int NumberOfScalarComponents,
				double * dataSpacing,
				double * dataRange,
				vtkInformation * info
				
				);
	static  int xyindextocount(int xindex, int yindex, int xlen);
	
};

