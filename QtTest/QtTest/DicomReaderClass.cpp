#include "DicomReaderClass.h"



DicomReaderClass::DicomReaderClass(vtkStringArray * seriesFiles)
{
	//reader = vtkSmartPointer<vtkDICOMReader>::New();
	reader = vtkDICOMReader::New();
	reader->SetFileNames(seriesFiles);
	reader->SetMemoryRowOrderToFileNative();
	//reader->AutoRescaleOff();
	reader->UpdateInformation();
	
	reader->Update();

	reader->GetDataOrigin(dataOrigin);
	
	readerOutputPort = reader->GetOutputPort();
	readerOutput = reader->GetOutput();
	NumberOfScalarComponents = readerOutput->GetNumberOfScalarComponents();
	reader->GetDataExtent(dataExtention);
	//Data_Scalar = reader->GetOutput()->GetPointData()->GetScalars();
	//Data_Scalar->GetRange(dataRange);
	reader->GetOutput()->GetPointData()->GetScalars()->GetRange(dataRange);
	PatientMatrix = reader->GetPatientMatrix();
	MetaData = reader->GetMetaData();
	vtkDICOMValue photometric = MetaData->Get(DC::PhotometricInterpretation);
	if (photometric.Matches("MONOCHROME1")) {
		lutDicom = vtkDICOMLookupTable::New();
		
	}

	if (photometric.Matches("MONOCHROME2")) {
		lutDicom = vtkDICOMLookupTable::New();
		lutDicom->SetTableRange(dataRange[0], dataRange[1]);
		lutDicom->SetSaturationRange(0, 0);
		lutDicom->SetHueRange(0, 0);
		lutDicom->SetValueRange(0, 1);
		lutDicom->SetAlphaRange(1, 1);
		lutDicom->Build();
	}

	if (photometric.Matches("PALETTE*")) {
		lutDicom = vtkDICOMLookupTable::New();
		lutDicom->BuildImagePalette(MetaData);
		
		
	}

	if (photometric.Matches("RGB*") || photometric.Matches("YBR*")) {

	}

	

}


DicomReaderClass::~DicomReaderClass()
{
	reader->Delete();
}

vtkAlgorithmOutput * DicomReaderClass::getReaderOutputPort()
{
	return this->readerOutputPort;
}

vtkImageData * DicomReaderClass::getReaderOutput()
{
	return this->readerOutput;
}

vtkDICOMMetaData * DicomReaderClass::getMetaData()
{
	return this->MetaData;
}

vtkMatrix4x4 * DicomReaderClass::GetPatientMatrix()
{
	return this->PatientMatrix;
}



vtkDICOMLookupTable * DicomReaderClass::getLUT()
{
	return this->lutDicom;
}

double * DicomReaderClass::getDataRange()
{
	return this->dataRange;
}

int * DicomReaderClass::getDataExtention()
{
	return this->dataExtention;
}

double * DicomReaderClass::getDataSpacing()
{
	return reader->GetDataSpacing();
}

int DicomReaderClass::getNumberOfScalarComponents()
{
	return NumberOfScalarComponents;
}





