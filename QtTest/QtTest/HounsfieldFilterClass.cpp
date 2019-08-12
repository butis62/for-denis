#include "HounsfieldFilterClass.h"





void HounsfieldFilterClass::Execute(vtkAlgorithmOutput * inputData, double filterRange[2], double OutValue)
{
	this->filterRange[0] = filterRange[0];
	this->filterRange[1] = filterRange[1];
	filter->RemoveAllInputConnections(0);
	filter->SetInputConnection(inputData);
	filter->ThresholdBetween(filterRange[0], filterRange[1]);
	filter->SetOutValue(OutValue);
	filter->SetReplaceOut(true);
	filter->Update();
	_filteringDone = true;
}
