#include "DicomSorterClass.h"

DicomRecord::DicomRecord(string fileName)
{
	this->fileName = fileName;
}

DicomRecord::~DicomRecord()
{
}

DicomSeries::DicomSeries(vtkStringArray *seriesFiles)
{
	this->seriesFiles = vtkStringArray::New();
	this->seriesFiles->DeepCopy(seriesFiles);
	this->RecordCount = to_string(seriesFiles->GetSize());
	

	vtkSmartPointer<vtkDICOMMetaData> metaData;
	vtkDICOMParser * p;
	DicomPatient * newPatient;
	metaData = vtkSmartPointer<vtkDICOMMetaData>::New();

	vtkSmartPointer<vtkDICOMParser> parser = vtkSmartPointer<vtkDICOMParser>::New();
	p = parser.Get();
	p->SetFileName(seriesFiles->GetValue(0));
	p->SetMetaData(metaData);
	p->Update();

	vtkDICOMMetaData * md = p->GetMetaData();
	string patientName = md->GetAttributeValue(DC::PatientName).AsString();
	string patientID = md->GetAttributeValue(DC::PatientID).AsString();
	SeriesNumber = md->GetAttributeValue(DC::SeriesNumber).AsString();
	SeriesModality = md->GetAttributeValue(DC::Modality).AsString();
	SeriesDescription = md->GetAttributeValue(DC::SeriesDescription).AsString();
	ScanOptions = md->GetAttributeValue(DC::ScanOptions).AsString();
	
		
}

DicomSeries::~DicomSeries()
{
	this->seriesFiles->Delete();
	list<DicomRecord *> ::iterator iter = RecordList.begin();
	int z = RecordList.size();
	while (iter != RecordList.end())
	{
		DicomRecord *tmp = *iter;
		delete tmp;
		RecordList.erase(iter++);
	}
	z = RecordList.size();
}

void DicomSeries::addRecord(DicomRecord * record)
{
	RecordList.push_back(record);
}



DicomStudy::DicomStudy()
{
}

DicomStudy::~DicomStudy()
{
	list<DicomSeries *> ::iterator iter = SeriesList.begin();
	int z = SeriesList.size();
	while (iter != SeriesList.end())
	{
		DicomSeries *tmp = *iter;
		delete tmp;
		SeriesList.erase(iter++);
	}
	z = SeriesList.size();
}

void DicomStudy::addSeries(DicomSeries * series)
{
	SeriesList.push_back(series);
}

DicomPatient::DicomPatient(string name, string id)
{
	this->name = name;
	this->id = id;
}

DicomPatient::~DicomPatient()
{
	list<DicomStudy *>::iterator iter = StudyList.begin();
	int z = StudyList.size();
	while (iter != StudyList.end())
	{
		DicomStudy * tmp = *iter;
		delete tmp;
		StudyList.erase(iter++);
	}
	z = StudyList.size();
}

void DicomPatient::addStudy(DicomStudy * study)
{
	StudyList.push_back(study);
}

DicomSorterClass::DicomSorterClass(string directoryName)
{
	this->directoryName = directoryName;
}


DicomSorterClass::~DicomSorterClass()
{
	
	list<DicomPatient*>::iterator iter = PatientsList.begin();
	int z = PatientsList.size();

	while (iter != PatientsList.end())
	{
		DicomPatient * tmp = *iter;
		delete tmp;
		PatientsList.erase(iter++);
		
	}
	
	z = PatientsList.size();
}

void DicomSorterClass::getFiles()
{
	int number;
	vtkStringArray *sortedFiles;
	vtkStdString studyFile;
	

	DicomRecord * currentRecord;
	DicomSeries * currentSeries;
	DicomStudy * currentStudy;
	DicomPatient * currentPatient;

	
	vtkNew<vtkStringArray> allfiles;
	for (const auto & entry : fs::recursive_directory_iterator(directoryName)) {

		
		const fs::path unicodePath = entry.path();
		
		if (fs::is_directory(unicodePath)) {
			continue;
		}
		const string utf8Path = unicodePath.u8string();
		allfiles->InsertNextValue(utf8Path.c_str());
		

		
	}


	dicomSorter = vtkSmartPointer<vtkDICOMFileSorter>::New();
	dicomSorter->SetInputFileNames(allfiles);
	dicomSorter->Update();
	

	//Study	
	currentPatient = nullptr;
	for (int i = 0; i < dicomSorter->GetNumberOfStudies(); i++)
	{
		//new study
		//if (i == 0) 
		//	currentPatient = new DicomPatient("","");

	    currentStudy = new DicomStudy();

		//Study series
		
		int firstStudySeries = dicomSorter->GetFirstSeriesForStudy(i);
		int lastStudySeries = dicomSorter->GetLastSeriesForStudy(i);

		sortedFiles = dicomSorter->GetFileNamesForSeries(firstStudySeries);

		DicomPatient * newPatient = getPatientData(currentPatient,currentStudy, sortedFiles->GetValue(0));

		//first study series 
		currentSeries = new DicomSeries(sortedFiles);
		for (int i = 0; i < sortedFiles->GetNumberOfValues(); ++i) {
			currentRecord = new DicomRecord(sortedFiles->GetValue(i));
			currentSeries->addRecord(currentRecord);
		}
		currentStudy->addSeries(currentSeries);

		//next study series
		for (int j = firstStudySeries+1; j <= lastStudySeries; j++)
		{
			//get series files
			 sortedFiles = dicomSorter->GetFileNamesForSeries(j);
			 currentSeries = new DicomSeries(sortedFiles);
			 for (int i = 0; i < sortedFiles->GetNumberOfValues(); ++i) {
				  currentRecord = new DicomRecord(sortedFiles->GetValue(i));
				  currentSeries->addRecord(currentRecord);
			 }
			 currentStudy->addSeries(currentSeries);
		}

		if (newPatient != currentPatient) {
			if (currentPatient) PatientsList.push_back(currentPatient);
			currentPatient = newPatient;
		}

		currentPatient->addStudy(currentStudy);
		
	}

	PatientsList.push_back(currentPatient);
}

DicomPatient * DicomSorterClass::getPatientData(DicomPatient * curentPatient, DicomStudy* currentStudy, vtkStdString firstFile)
{
	vtkSmartPointer<vtkDICOMMetaData> metaData;
	vtkDICOMParser * p;
	DicomPatient * newPatient;
	metaData = vtkSmartPointer<vtkDICOMMetaData>::New();

	vtkSmartPointer<vtkDICOMParser> parser = vtkSmartPointer<vtkDICOMParser>::New();
	p = parser.Get();
	p->SetFileName(firstFile);
	p->SetMetaData(metaData);
	p->SetOverrideCharacterSet(47);
	p->OverrideCharacterSetOn();
	p->Update();
	
	vtkDICOMMetaData * md = p->GetMetaData();
	string patientName=md->GetAttributeValue(DC::PatientName).AsString();
	string patientID = md->GetAttributeValue(DC::PatientID).AsString();
	currentStudy->StudyDescription = md->GetAttributeValue(DC::StudyDescription).AsString();
	currentStudy->StudyID = md->GetAttributeValue(DC::StudyID).AsString();
	currentStudy->StudyInstanceUID = md->GetAttributeValue(DC::StudyInstanceUID).AsString();
	currentStudy->StudyDate = md->GetAttributeValue(DC::StudyDate).AsString();
	if (curentPatient == nullptr) {
		newPatient = new DicomPatient(patientName, patientID);
		return newPatient;
	}
	if (curentPatient->name == patientName && curentPatient->id == patientID)
		return curentPatient;

	newPatient = new DicomPatient(patientName, patientID);
	return newPatient;
}

bool DicomSorterClass::findPatient(string name, string id)
{	
	if (PatientsList.empty()) return false;
	for (auto const& i : PatientsList) {
		

		if (i->name == name & i->id == id) return true;
	}
	
	return false;
}
