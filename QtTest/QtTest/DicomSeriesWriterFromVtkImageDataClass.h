#pragma once
//#include <string.h>
#include <vtkDICOMCTGenerator.h>
#include <vtkDICOMWriter.h>
#include <vtkDICOMMetaData.h>

using namespace std;

class DicomSeriesWriterFromVtkImageDataClass
{
public:
	DicomSeriesWriterFromVtkImageDataClass(
					vtkAlgorithmOutput* outputPort,
					//vtkImageData * outputPort,
					vtkDICOMMetaData * metaData,
					string filePrefix,
					string filePattern
	);
	~DicomSeriesWriterFromVtkImageDataClass();

	vtkDICOMCTGenerator * generator;
	vtkDICOMWriter * writer;

};

