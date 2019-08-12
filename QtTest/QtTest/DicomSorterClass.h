#pragma once
#include <string>
#include <iostream>
#include <filesystem>
#include "vtkDICOMFileSorter.h"
#include "vtkDICOMParser.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkDICOMMetaData.h"
#include "vtkType.h"

namespace fs = std::experimental::filesystem;
using namespace std;

class DicomRecord
{
public:
	DicomRecord(string fileName);
	~DicomRecord();

private:
	string fileName;
};


class DicomSeries
{
public:
	DicomSeries(vtkStringArray * seriesFiles);
	~DicomSeries();

	void addRecord(DicomRecord * record);

	string SeriesNumber;
	string SeriesDescription;
	string SeriesModality;
	string RecordCount;
	string ScanOptions;

	list<DicomRecord *> RecordList;
	
	vtkStringArray * seriesFiles;
private:
	
};


class DicomStudy
{
public:
	DicomStudy();
	~DicomStudy();

	string StudyInstanceUID = "";
	string StudyID ="";
	string StudyDescription ="";
	string StudyDate = "";

	list<DicomSeries *> SeriesList;

	void addSeries(DicomSeries * series);

private:

};

 class DicomPatient
{
public:
	DicomPatient(string name,string id);
	~DicomPatient();

	string name;
	string id;

	list<DicomStudy *> StudyList;

	void addStudy(DicomStudy * study);
private:
	
};


class DicomSorterClass
{
public:
	DicomSorterClass(string directoryName) ;
	~DicomSorterClass();
	void getFiles();

	list<DicomPatient*> PatientsList;

private:
	string directoryName;

	vtkSmartPointer <vtkDICOMFileSorter> dicomSorter;

	
	
	DicomPatient * getPatientData(DicomPatient * curentPatient, DicomStudy* currentStudy,vtkStdString firstFile);

	bool findPatient(string name, string id);

};

