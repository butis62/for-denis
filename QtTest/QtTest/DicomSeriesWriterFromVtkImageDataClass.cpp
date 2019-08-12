#include "DicomSeriesWriterFromVtkImageDataClass.h"



DicomSeriesWriterFromVtkImageDataClass::DicomSeriesWriterFromVtkImageDataClass(
	vtkAlgorithmOutput* outputPort,
	//vtkImageData * outputPort,
	vtkDICOMMetaData * metaData,
	string filePrefix,
	string filePattern
	)
{
	generator = vtkDICOMCTGenerator::New();
	writer = vtkDICOMWriter::New();
	
	

	writer->SetInputConnection(outputPort);
	writer->SetGenerator(generator);
		//writer->SetPatientMatrix(PatientMatrix)
	writer->SetMemoryRowOrderToFileNative();
	writer->SetMetaData(metaData);
	writer->SetFilePrefix(filePrefix.c_str());
	writer->SetFilePattern(filePattern.c_str());
	writer->Write();



}


DicomSeriesWriterFromVtkImageDataClass::~DicomSeriesWriterFromVtkImageDataClass()
{
	generator->Delete();
	writer->Delete();
}
