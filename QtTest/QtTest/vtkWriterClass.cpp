#include "vtkWriterClass.h"



vtkWriterClass::vtkWriterClass()
{
}


vtkWriterClass::~vtkWriterClass()
{
}

void vtkWriterClass::writeToStl(string fileName, vtkAlgorithmOutput* inputData, double dataRange[2], int numContours, bool binaryFile)
{
	

	vtkFlyingEdges3D * isosurface = vtkFlyingEdges3D::New();
	isosurface->SetInputConnection(inputData);
	isosurface->GenerateValues(numContours, dataRange);
	isosurface->Update();
	vtkSTLWriter * writer = vtkSTLWriter::New();
	writer->SetFileName(fileName.c_str());
	if (binaryFile)
		writer->SetFileTypeToBinary();
	else
		writer->SetFileTypeToASCII();
	writer->SetInputConnection(isosurface->GetOutputPort());
	writer->Write();
	writer->Delete();
	isosurface->Delete();

}

void vtkWriterClass::writeToDicom(
		vtkAlgorithmOutput * outputPort, 
		vtkDICOMMetaData * metaData, 
		string filePrefix, 
		string filePattern,
	vtkMatrix4x4 * PatientMatrix
		
)
{
	vtkDICOMCTGenerator * generator = vtkDICOMCTGenerator::New();

	vtkDICOMWriter * writer = vtkDICOMWriter::New();
	vtkDICOMMetaData * copymetaData = vtkDICOMMetaData::New();
	copymetaData->DeepCopy(metaData);
	//copymetaData->SetAttributeValue(DC::Rows, rows);
	//copymetaData->SetAttributeValue(DC::Columns, columns);

	writer->SetInputConnection(outputPort);
	writer->SetGenerator(generator);
	writer->SetPatientMatrix(PatientMatrix);
	writer->SetMemoryRowOrderToFileNative();
	writer->SetMetaData(copymetaData);
	writer->SetFilePrefix(filePrefix.c_str());
	writer->SetFilePattern(filePattern.c_str());
	writer->Write();
	generator->Delete();
	writer->Delete();
	copymetaData->Delete();
}

void vtkWriterClass::writeToPng(string fileName, double range[2], vtkAlgorithmOutput * inputData)
{
	vtkImageShiftScale * converter = vtkImageShiftScale::New();
	converter->SetInputConnection(inputData);
	
	converter->SetShift(-range[0]);
	converter->SetScale(255.0 / (range[1] - range[0]));
	converter->SetOutputScalarTypeToUnsignedChar();
	converter->ClampOverflowOn();
	converter->Update();

	vtkPNGWriter * writer = vtkPNGWriter::New();
	writer->SetFileName(fileName.c_str());
	
	
	writer->SetInputConnection(converter->GetOutputPort());
	writer->Write();
	writer->Delete();
}
