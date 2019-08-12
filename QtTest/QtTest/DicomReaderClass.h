#pragma once
#include "vtkDICOMReader.h"
#include "vtkSmartPointer.h"
#include <iostream>
#include <string>
#include "vtkImageData.h"
#include "vtkPointdata.h"
#include "vtkDataArray.h"

#include "vtkSmartPointer.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "vtkDICOMMetaData.h"

#include <vtkDICOMLookupTable.h>

class DicomReaderClass
{
public:
	DicomReaderClass(vtkStringArray * seriesFiles);
	~DicomReaderClass();
	
	vtkAlgorithmOutput* getReaderOutputPort();
	vtkImageData * getReaderOutput();
	vtkDICOMMetaData * getMetaData();
	vtkMatrix4x4 * GetPatientMatrix();
	//vtkDataArray * get3DDataScalar();
	vtkDICOMLookupTable * getLUT();
	double * getDataRange();
	int * getDataExtention();
	double * getDataSpacing();
	int getNumberOfScalarComponents();
private:
	//vtkSmartPointer <vtkDICOMReader> reader;
	vtkDICOMReader * reader;
	vtkMatrix4x4 * PatientMatrix;
	vtkDICOMMetaData * MetaData;
	vtkAlgorithmOutput* readerOutputPort;
	//vtkDataArray * Data_Scalar;
	vtkImageData * readerOutput;
	double dataRange[2];
	double dataOrigin[3];
	int dataExtention[6];

	int NumberOfScalarComponents;
	

	vtkDICOMLookupTable * lutDicom;
	double tableRange[2];
	double valueRange[2];
	double hueRange[2];

};

