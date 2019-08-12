#pragma once


using namespace std;


#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "vtkSmartPointer.h"
#include <QFileDialog>
#include "DicomDirClass.h"
#include "DicomSorterClass.h"
#include "DicomReaderClass.h"
#include "vtk3DViewerClass.h"
#include "DicomTreeClass.h"
#include <QTimer>
#include <QTime>
#include <vtkImageMathematics.h>

#include "MathUtilsClass.h"
#include "vtkWriterClass.h"
#include "HounsfieldFilterClass.h"
#include <QXmlStreamReader>


enum InputNameType {
	DicomDirFile = 0, 
	DicomDir =1,
	DicomDirAndChannel = 2
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	
	MainWindow(QWidget *parent = Q_NULLPTR);
	virtual ~MainWindow();
	QItemSelectionModel* selectionModel;

	QVTKOpenGLWidget * views[4];
	QLineEdit * saggital[15];
	QLineEdit * coronal[15];
	QLineEdit * axial[15];
	QLineEdit * windowlevel[2];

	

private:
	Ui::MainWindowClass ui;

	QActionGroup *winGroup;
	QString inputName = "";
	QString predictedChannelFile = "";
	QTimer * timerIdle;
	InputNameType inputNameType;
	

	DicomDirClass * dicomDir = nullptr;
	DicomSorterClass * dicomSorter = nullptr;
	DicomReaderClass * reader3d = nullptr;
	vtk3DViewerClass * viewer3d = nullptr;
	QString dicomDirPath = "e:\\PythonProg\\Data\\DicomData";
	//QString dicomDirPath = "d:\\PythonProg\\DicomData";

	QToolBar * tbWindowLevelSetting;
	QToolBar * tbVolumePropertySetting;
	QToolBar * tbHounsfieldFilter;
	
	float windowWidth = 1500;
	float windowLevel = 300;
	int VolumePropertyIndex = 0;

	
	

	VPSettings * vpSettings = nullptr;
	HounsfieldFilterClass * hounsfieldFilter = nullptr;
	

	void __initAction();
	void __initUI();
	void __destroyData();
	void __destroy3DData();
		
	void SetControlStatus(bool status);
	void SetHounsfieldFilter(double filterRange[2],double minmaxRange[2]);

	void CreatePngSaveMenu();
	string getApplicationPath();
	
private slots:
	
	void getDicomDirFile();
	void getDicomDir();
	void getDicomDirAndChannelFile();
	void dicomModelSelection(const QItemSelection &selected, const QItemSelection &deselected);
	void WindowLevelChange(int row,int column);
	void VolumePropertyChange(int row, int column);
	
	
	void UpdateWindowLevel();
	void UpdateVolumeProperty();
	void timerIdleExec();
	
	void SaveToDicom();
	void SaveToStl();
	void SaveToPng();
	
	void RotateAxialPlane();
	void ApplyHounsfieldFilter();
	void MoveToAxialSlice();
	void UpdateAxialNormal();

	void actor_view_change(bool checked);

	
};
