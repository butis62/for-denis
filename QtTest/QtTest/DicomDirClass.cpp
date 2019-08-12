

#include "DicomDirClass.h"


DicomRecordClass::DicomRecordClass()
{
}

DicomRecordClass::~DicomRecordClass()
{
}

DicomSeriesClass::DicomSeriesClass()
{
}

DicomSeriesClass::~DicomSeriesClass()
{
}

DicomStudyClass::DicomStudyClass()
{
}

DicomStudyClass::~DicomStudyClass()
{
}

DicomPatientClass::DicomPatientClass(const vtkDICOMItem& patientItem)
{
	this->patientName = patientItem.Get(DC::PatientName);
}

DicomPatientClass::~DicomPatientClass()
{
}

DicomDirClass::DicomDirClass(std::string DicomDirFile)
{
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	errno_t err;

	this->DicomDirFile = DicomDirFile;

	err = _splitpath_s(this->DicomDirFile.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname,_MAX_FNAME, ext, _MAX_EXT);
	
	_makepath_s(path_buffer, _MAX_PATH, drive, dir,"", "");

	this->DicomDirPath = path_buffer;



}



DicomDirClass::~DicomDirClass()
{
	this->patients.clear();
	
}

void DicomDirClass::GetPatients()
{
	dicomDirectory = vtkSmartPointer<vtkDICOMDirectory>::New();
	dicomDirectory->SetDirectoryName(DicomDirPath.c_str());
	dicomDirectory->Update();
	
	
	for (int patient = 0; patient < dicomDirectory->GetNumberOfPatients(); patient++)
	{
		const vtkDICOMItem& _patient = dicomDirectory->GetPatientRecord(patient);
		DicomPatientClass patientItem = DicomPatientClass(_patient);
		patients.push_back(patientItem);
			

	}
}



