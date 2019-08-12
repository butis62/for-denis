#pragma once

#include <string>
#include <list>
#include <stdio.h>

#include "vtkDICOMDirectory.h"
#include "vtkSmartPointer.h"
#include "vtkDICOMDictHash.h"
#include "vtkDICOMItem.h"

using namespace std;


class DicomRecordClass
{
public:
	DicomRecordClass();
	~DicomRecordClass();

private:

};

class DicomSeriesClass
{
public:
	DicomSeriesClass();
	~DicomSeriesClass();

private:

};

class DicomStudyClass
{
public:
	DicomStudyClass();
	~DicomStudyClass();

private:

};

class DicomPatientClass
{
public:
	DicomPatientClass(const vtkDICOMItem& patientItem);
	~DicomPatientClass();

private:
	vtkDICOMValue patientName;

};



class DicomDirClass
{
public:
	DicomDirClass(string DicomDirPath);
	~DicomDirClass();
	void GetPatients();
	vtkSmartPointer<vtkDICOMDirectory> dicomDirectory;

private:
	string DicomDirFile;
	string DicomDirPath;

	

	list<DicomPatientClass> patients;
	
};

