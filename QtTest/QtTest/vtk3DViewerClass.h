#pragma once

using namespace std;

#include "QVTKOpenGLWidget.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkOpenGLGPUVolumeRayCastMapper.h"
#include "vtkVolume.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkResliceImageViewer.h>
#include <vtkImagePlaneWidget.h>
#include <vtkCellPicker.h>
#include <QVTKInteractor.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceCursorLineRepresentation.h>
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkResliceImageViewerMeasurements.h"
#include <vtkPlaneSource.h>
#include <vtkInteractorStyleImage.h>
#include <vtkImageMapToWindowLevelColors.h>
#include <vtkDICOMLookupTable.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkVolumeProperty.h>
#include <vtkContourValues.h>
#include <vtkImageActor.h>
#include <vtkImageReslice.h>
#include <vtkTransform.h>
#include <vtkCamera.h>
#include <vtkPlane.h>
#include <qapplication.h>
#include <QLineEdit.h>
#include <vtkImageReslice.h>
#include <vtkImageMapper3D.h>
#include <vtkDataSetAlgorithm.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkImageProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkExtractVOI.h>
#include <vtkPolyDataMapper.h>
#include "vtkBorderWidget.h"
#include "vtkBorderRepresentation.h"
#include <vtkProperty2D.h>
#include "MathUtilsClass.h"
#include "DicomReaderClass.h"
#include "vtkWriterClass.h"
#include <QXmlStreamReader>
#include <QFile>
#include "npy.hpp"



typedef map<string, string> VP_SETTING;
typedef vector<VP_SETTING> VP_SETTING_LIST;


class vtkResliceCursorCallback;
class vtkImageResliceCallback;

enum CT_Type { 
	SpineBone = 0, 
	SpineSoftTissues = 1, 
	AbdomenSoftTissues = 2, 
	AbdomenLiver = 3
};


class VPSettings
{
public:
	VP_SETTING_LIST vpsettings;
	QStringList names;
	VPSettings(QString vppath);
	~VPSettings();
	VP_SETTING get_volume_property(int index);
private:

};


/*

class OpacityPoint
{
public:
	OpacityPoint(
		double x,
		double y,
		double midpoint,
		double sharpness
	);
	~OpacityPoint();

	double x;
	double y;
	double midpoint;
	double sharpness;
private:

};


class ColorPoint
{
public:
	ColorPoint(
		double x,
		double red,
		double green,
		double blue,
		double midpoint,
		double sharpness
	);
	~ColorPoint();

	double x;
	double red;
	double green;
	double blue;
	double midpoint;
	double sharpness;
private:

};

class ColorOpacitySettings
{
public:
	ColorOpacitySettings(
						vector<double> points,
						vector<double> reds,
						vector<double> greens, 
						vector<double> blues,
						vector<double> colormidpoints,
						vector<double> colorsharpness,
						vector<double> opacitys,
						vector<double> opacitysmidpoints,
						vector<double> opacityssharpness,
						vector<double> gradientpoints,
						vector<double> gradients,
						vector<double> gradientsmidpoints,
						vector<double> gradientssharpness,
						int BlendMode,
						int Shade,
						double Ambient,
						double Diffuse,
						double Specular,
						double SpecularPower,
						double ScalarOpacityUnitDistance
						);
	~ColorOpacitySettings();

	list<ColorPoint*> colorPoints;
	list<OpacityPoint*> opacityPoints;
	list<OpacityPoint *> gradientPoints;

	int BlendMode;
	int Shade;
	double Ambient;
	double Diffuse;
	double Specular;
	double SpecularPower;
	double ScalarOpacityUnitDistance;
	
private:

};


class ColorOpacitySettingsList
{
public:
	ColorOpacitySettingsList();
	~ColorOpacitySettingsList();
	vector<ColorOpacitySettings*> colorOpacitySettings;
private:

};


*/


class vtk3DViewerClass
{
public:
	vtk3DViewerClass(
						QVTKOpenGLWidget * views[4],
						QLineEdit * saggital[15],
						QLineEdit * coronal[15],
						QLineEdit * axial[15],
						QLineEdit * windowlevel[2],
						QLineEdit * slicePos,
						QLineEdit * sliceIndex,
						QLineEdit * yDegree

	);
	~vtk3DViewerClass();
	void ShowScene(
					vtkAlgorithmOutput* outputPort, 
					DicomReaderClass * reader,
					VP_SETTING vpSetting,
					float windowWidth,
					float windowLevel,
					bool actorView[5]
	
					);

	void UpdateScene(
					vtkAlgorithmOutput* inputData,
					VP_SETTING vpSetting
					);

	void UpdateWindowLevel(float windowWidth, float windowLevel);
	void UpdateVolumeProperty(VP_SETTING vpSetting);
	void MoveToSlice(int slice);

	void UpdateActorView(bool actorView[5]);
	
	
	void SaveToDicom(string filePrefix, string filePattern,bool voi);
	void SaveToStl(string fileName, double filterRange[2], int numContours = 2,bool voi = true);
	void SaveToPng(string fileName, int planeIndex = 2);
	void RotateAxialPlane();
	void UpdateAxialNormal(double normal[3]);

	void ShowColorWindowLevel(vtkImageActor * actor);
	void ShowColorWindowLevel(vtkImagePlaneWidget * planeWidget);
	void SetColorWindowLevel(int planeIndex, int direction);
	void ShowSaggital(vtkPlaneSource *planeSource);
	void ShowCoronal(vtkPlaneSource *planeSource);
	void ShowAxial(vtkPlaneSource *planeSource);
	void GetAxialCharacteristics(vtkPlaneSource *planeSource);
	void SetAxialCharacteristics(vtkPlaneSource *planeSource);
	
	bool visibleScene = false;

	QVTKOpenGLWidget * views[4];
	vtkImagePlaneWidget * planeWidget[3];
	vtkImageReslice * riw[3];
	vtkExtractVOI *voi3D;
	vtkImageActor * sliceActor[3];

	
	

	QLineEdit * saggital[15];
	QLineEdit * coronal[15];
	QLineEdit * axial[15];
	QLineEdit * windowlevel[2];
	QLineEdit * yDegree;
	QLineEdit * slicePos;
	QLineEdit * sliceIndex;
	double * dataRange;// [2];
	double * dataSpacing;// [3];
	int * dataExtention;// [6];
	int sliceCount[3];
	double sliceMaxBorder[3];

	double axialpoint2[3];
	double axialpoint1[3];
	double axialcenter[3];
	double axialorigin[3];
	double axialnormal[3];
	int axialIndex;
private:
	vtkImageResliceCallback *cbkNew = nullptr;
	
	
	vtkWindowLevelLookupTable *sliceLut[3];
	vtkImageMapToColors * sliceMaptocolors[3];
	
	//vtkResliceImageViewer* riw[3];
	vtkGenericOpenGLRenderWindow * renderWindows[3];
	
	vtkRenderer * renderer = nullptr;
	//vtkResliceCursorCallback *cbkOld = nullptr;
	vtkCellPicker * picker = nullptr;
	vtkProperty * ipwProp = nullptr;
	vtkGenericOpenGLRenderWindow * renderWindow = nullptr;
	vtkRenderWindowInteractor *iren = nullptr;

	//vtkOpenGLGPUVolumeRayCastMapper * mapper;
	vtkGPUVolumeRayCastMapper * mapper;
	vtkVolume *volume3D;
	
	vtkOutlineFilter * voiData;
	vtkPolyDataMapper * voiDataMapper;
	vtkActor *voiActor;

	

	vtkColorTransferFunction *colorFun;
	vtkPiecewiseFunction *opacityFun ;
	vtkPiecewiseFunction * gradientOpacity;
	vtkVolumeProperty *volume3Property;

	

	vtkAlgorithmOutput* outputPort;
	vtkImageData *outputData;
	vtkDICOMMetaData * MetaData;
	vtkMatrix4x4 * PatientMatrix;
	

	

	double opacityWindow = 4096;
	double opacityLevel = 2048;
	
	float windowWidth = 1800;
	float windowLevel = 400;
	float FinalColorLevel;
	float FinalColorWindow;

	double nSaggital[3];
	double nCoronal[3];
	double nAxial[3];

	int voi[6];
	int NumberOfScalarComponents;
	
	vtkRenderer *sliceRenderer[3];

	//ColorOpacitySettings * ctSetting;
	VP_SETTING vpSetting;

	//string filePrefix = "e:\\PythonProg\\Data\\DicomData\\tmp";
	string filePrefix = "d:\\PythonProg\\DicomData\\result";

	void setUI(QLineEdit * saggital[15], QLineEdit * coronal[15], QLineEdit * axial[15], QLineEdit * windowlevel[2],
		QLineEdit * yDegree, QLineEdit * slicePos, QLineEdit * sliceIndex);

	
	void setPlaneWidgitVisibility(bool visibility, int plane);

	void createCT();
	
	
	
};

