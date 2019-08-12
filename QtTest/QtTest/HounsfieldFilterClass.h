#pragma once
#include <vtkImageThreshold.h>
#include <vtkAlgorithmOutput.h>

class HounsfieldFilterClass
{
public:
	HounsfieldFilterClass( ) {
		filter = vtkImageThreshold::New();
	};
	~HounsfieldFilterClass() 
	{
		if (filter) {
			filter->Delete();
			filter = nullptr;
	  }
	};
	void Execute(vtkAlgorithmOutput * inputData, double filterRange[2], double OutValue);
	vtkAlgorithmOutput * getOutputPort() { return filter->GetOutputPort(); };
	double * getFilterRange() { return filterRange; };
	bool filteringDone() { return _filteringDone; };
private:
	vtkImageThreshold * filter = nullptr;
	bool _filteringDone = false;
	double filterRange[2];
};

