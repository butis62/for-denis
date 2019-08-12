#include "MathUtilsClass.h"



MathUtilsClass::MathUtilsClass()
{
}


MathUtilsClass::~MathUtilsClass()
{
}

vtkMatrix4x4 * MathUtilsClass::getRotateMatrix(double angle_x, double angle_y, double angle_z)
{
	vtkMatrix4x4 * rotateMatrix = vtkMatrix4x4::New();
	double A = cos(angle_x);
	double B = sin(angle_x);
	double C = cos(angle_y);
	double D = sin(angle_y);
	double E = cos(angle_z);
	double F = sin(angle_z);

	double AD = A * D;
	double BD = B * D;

	rotateMatrix->SetElement(0, 0, C * E);
	rotateMatrix->SetElement(0, 1,-C * F);
	rotateMatrix->SetElement(0, 2, -D);
	rotateMatrix->SetElement(1, 0, -BD * E + A * F);
	rotateMatrix->SetElement(1, 1, BD * F + A * E);
	rotateMatrix->SetElement(1, 2, -B * C);
	rotateMatrix->SetElement(2, 0, AD * E + B * F);
	rotateMatrix->SetElement(2, 1, -AD * F + B * E);
	rotateMatrix->SetElement(2, 2, A * C);

	return rotateMatrix;
}

void MathUtilsClass::getRotateTrasform(
				vtkAlgorithmOutput* outputPort,
				vtkDICOMMetaData * MetaData,
				double angle_x, 
				double angle_y, 
				double angle_z
)
{
	double d_angle_x = vtkMath::DegreesFromRadians(angle_x);
	double d_angle_y = vtkMath::DegreesFromRadians(angle_y);
	double d_angle_z = vtkMath::DegreesFromRadians(angle_z);

	
	vtkMatrix4x4 * rotateMatrix = getRotateMatrix(d_angle_x, d_angle_y, d_angle_z);
	vtkTransform * transform = vtkTransform::New();

	transform->RotateX(d_angle_x);
	transform->RotateY(d_angle_y);
	transform->RotateZ(d_angle_z);
	transform->Update();
	vtkMatrix4x4 * tmp = transform->GetMatrix();
	vtkTransformFilter * transformfilter = vtkTransformFilter::New();
	transformfilter->SetInputConnection(outputPort);
	transformfilter->SetTransform(transform);
	transformfilter->Update();

	DicomSeriesWriterFromVtkImageDataClass * writer = new DicomSeriesWriterFromVtkImageDataClass(
		transformfilter->GetOutputPort(),
		MetaData,
		"e:\\PythonProg\\Data\\DicomData\\tmp",
		"%s/IM-0001-%04.4d.dcm"
	);
}

vtkDataSetAlgorithm * MathUtilsClass::LoadArrayFromNumpy(string fileName,
	int ScalarType,
	int NumberOfScalarComponents,
	double * dataSpacing,
	double * dataRange,
	vtkInformation * info
		)
{
	vector<unsigned long> shape;
	vector<double> data;
	npy::LoadArrayFromNumpy(fileName, shape, data);

	vtkImageData * dataImage = vtkImageData::New();
	
	
	dataImage->SetExtent(0,shape[0],0,shape[1],0,shape[2]);
	dataImage->SetSpacing(dataSpacing);
	
	dataImage->SetOrigin(0, 0, 0);

	dataImage->SetInformation(info);

	dataImage->AllocateScalars(ScalarType,NumberOfScalarComponents);
	
	double range[2];
	int count = 0;
	
	int y_top = 0;
	int x_right = 0;
	vector<int> slce_square_count(shape[2],0);
	int x_same_y_top;
	int x_right_y_same;
	int x_right_y_top;
	
	for (int z = shape[2]-1,zinverse = 0; z >= 0 ; z--,zinverse++)
	{
		//slce_square_count = 0;

		for (int x = 0; x < shape[0];x++) //row
		{
			
			for (int y = 0; y < shape[1]; y++)//col
			{
				short* pixel = static_cast<short*>(dataImage->GetScalarPointer(x, y, z));
				if (data[count] > 0) {
					pixel[0] = 4000;// static_cast<short>(dataRange[1]);
					/*
					if (x == shape[0] - 1) {
						count++;
						continue;
					}
					if (y == shape[1] - 1) {
						count++;
						continue;
					}

					
					x_same_y_top = count+shape[0];
					x_right_y_same = count+1;
					x_right_y_top = x_same_y_top +1;
					if (data[x_same_y_top] > 0 && data[x_right_y_same] > 0 && data[x_right_y_top] > 0)
					*/
					slce_square_count[z]++;
					count++;
					continue;
				}
				else {
					pixel[0] = static_cast<short>(dataRange[0]);// dataReslicGetS GetScalarComponentAsDouble(x, y, 0, 0);
					count++;
				}
				
			}
		}
	}

	int dataExtention[6];
	dataExtention[0] = 0;
	dataExtention[1] = shape[0];
	dataExtention[2] = 0;
	dataExtention[3] = shape[1];
	dataExtention[4] = 0;
	dataExtention[5] = shape[2];

	
 	vtkDataSetAlgorithm * tmp = vtkDataSetAlgorithm::New();
	tmp->SetInputData(0, dataImage);
	tmp->Update();
	//tmp->UpdateExtent(dataExtention);
	dataImage->GetPointData()->GetScalars()->Modified();
	
	dataImage->GetPointData()->GetScalars()->GetRange(range);
	
	return tmp;
}

int MathUtilsClass::xyindextocount(int xindex, int yindex, int xlen)
{
	return yindex*xlen + xindex;
}
