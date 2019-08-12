#pragma once
#include <vtkSTLWriter.h>
#include <vtkDataSetAlgorithm.h>
#include <vtkFlyingEdges3D.h>
#include <vtkAlgorithmOutput.h>
#include <vtkDICOMMetaData.h>
#include <vtkDICOMCTGenerator.h>
#include <vtkDICOMWriter.h>
#include <vtkPNGWriter.h>
#include <vtkImageCast.h>
#include <vtkImageShiftScale.h>

using namespace std;

class vtkWriterClass
{
public:
	vtkWriterClass();
	~vtkWriterClass();
	static void writeToStl(string fileName, vtkAlgorithmOutput* inputData,double dataRange[2], int numContours = 2,bool binaryFile = true);
	static void writeToDicom(
		vtkAlgorithmOutput* outputPort,
		vtkDICOMMetaData * metaData,
		string filePrefix,
		string filePattern,
		vtkMatrix4x4 * PatientMatrix
		);

	static void writeToPng(string fileName, double range[2],vtkAlgorithmOutput* inputData);
};

