#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	
	
	ui.setupUi(this);
	//vpSettings = new VPSettings("d:\\CProg\\QtTest\\x64\\Debug\\presets.xml");
	
	vpSettings = new VPSettings(QString::fromStdString(getApplicationPath()));
	__initUI();
	__initAction();

	
	hounsfieldFilter = new HounsfieldFilterClass();
	


}

MainWindow::~MainWindow()
{
	__destroyData();
	if (vpSettings) {
		delete vpSettings;
		vpSettings = nullptr;
	}
	
	if (viewer3d) {
		delete viewer3d;
		viewer3d = nullptr;
	}


	if (hounsfieldFilter)
	{
		delete hounsfieldFilter;
		hounsfieldFilter = nullptr;
	}

	timerIdle->stop();
	delete timerIdle;
}

void MainWindow::__initAction() {

	connect(ui.actionOpen_DicomDir_File, &QAction::triggered, this, &MainWindow::getDicomDir);
	connect(ui.actionOpen_Dicom_Folder_and_predicted_channel_file, &QAction::triggered, this, &MainWindow::getDicomDirAndChannelFile);
	connect(ui.tableWindowLevelPreset,
		SIGNAL(cellClicked(int, int)),
		this,
		SLOT(WindowLevelChange(int, int))
	);

	connect(ui.tableVolumePropertyPreset,
		SIGNAL(cellClicked(int, int)),
		this,
		SLOT(VolumePropertyChange(int, int))
	);

	connect(ui.action3D_actor, &QAction::toggled, this, &MainWindow::actor_view_change);
	connect(ui.actionVOI_actor, &QAction::toggled, this, &MainWindow::actor_view_change);
	connect(ui.actionAxial_slice_actor, &QAction::toggled, this, &MainWindow::actor_view_change);
	connect(ui.actionSagittal_slice_actor, &QAction::toggled, this, &MainWindow::actor_view_change);
	connect(ui.actionCoronal_slice_actor, &QAction::toggled, this, &MainWindow::actor_view_change);

	connect(ui.actionUpdate_window_level_settings, &QAction::triggered, this, &MainWindow::UpdateWindowLevel);
	connect(ui.actionUpdate_volume_property_settings, &QAction::triggered, this, &MainWindow::UpdateVolumeProperty);
	connect(ui.fResliceMode, SIGNAL(stateChanged(int)), this, SLOT(resliceMode(int)));

	connect(ui.actionAxial_move, &QAction::triggered, this, &MainWindow::RotateAxialPlane);
	connect(ui.actionSave_to_Dicom, &QAction::triggered, this, &MainWindow::SaveToDicom);
	connect(ui.actionSave_to_Stl, &QAction::triggered, this, &MainWindow::SaveToStl);
	connect(ui.actionApply_Hounsfield_filter, &QAction::triggered, this, &MainWindow::ApplyHounsfieldFilter);
	connect(ui.actionMove_to_slice, &QAction::triggered, this, &MainWindow::MoveToAxialSlice);
	connect(ui.actionUpdate_axial_normale, &QAction::triggered, this, &MainWindow::UpdateAxialNormal);
	connect(ui.actionSave_axial_slice_to_Png, &QAction::triggered, this, &MainWindow::SaveToPng);
}

void MainWindow::__initUI()
{
	
	
	ui.splitter->setSizes(QList<int>() << 500 << 400);
	ui.splitter->setStretchFactor(0, 0.5);
		
	ui.splitter->setStretchFactor(1, 1);
	ui.centralWidget->setVisible(false);
	ui.tabWidget->setVisible(false);

	this->setWindowState(Qt::WindowMaximized);


	CreatePngSaveMenu();

	views[0] = ui.view1;
	views[1] = ui.view2;
	views[2] = ui.view3;
	views[3] = ui.view4;

	saggital[0] = ui.saggital_Origin_X;
	saggital[1] = ui.saggital_Origin_Y;
	saggital[2] = ui.saggital_Origin_Z;
	saggital[3] = ui.saggital_UL_X;
	saggital[4] = ui.saggital_UL_Y;
	saggital[5] = ui.saggital_UL_Z;
	saggital[6] = ui.saggital_LR_X;
	saggital[7] = ui.saggital_LR_Y;
	saggital[8] = ui.saggital_LR_Z;
	saggital[9] = ui.saggital_N_X;
	saggital[10] = ui.saggital_N_Y;
	saggital[11] = ui.saggital_N_Z;
	saggital[12] = ui.saggital_C_X;
	saggital[13] = ui.saggital_C_Y;
	saggital[14] = ui.saggital_C_Z;

	coronal[0] = ui.coronal_Origin_X;
	coronal[1] = ui.coronal_Origin_Y;
	coronal[2] = ui.coronal_Origin_Z;
	coronal[3] = ui.coronal_UL_X;
	coronal[4] = ui.coronal_UL_Y;
	coronal[5] = ui.coronal_UL_Z;
	coronal[6] = ui.coronal_LR_X;
	coronal[7] = ui.coronal_LR_Y;
	coronal[8] = ui.coronal_LR_Z;
	coronal[9] = ui.coronal_N_X;
	coronal[10] = ui.coronal_N_Y;
	coronal[11] = ui.coronal_N_Z;
	coronal[12] = ui.coronal_C_X;
	coronal[13] = ui.coronal_C_Y;
	coronal[14] = ui.coronal_C_Z;

	axial[0] = ui.axial_Origin_X;
	axial[1] = ui.axial_Origin_Y;
	axial[2] = ui.axial_Origin_Z;
	axial[3] = ui.axial_UL_X;
	axial[4] = ui.axial_UL_Y;
	axial[5] = ui.axial_UL_Z;
	axial[6] = ui.axial_LR_X;
	axial[7] = ui.axial_LR_Y;
	axial[8] = ui.axial_LR_Z;
	axial[9] = ui.axial_N_X;
	axial[10] = ui.axial_N_Y;
	axial[11] = ui.axial_N_Z;
	axial[12] = ui.axial_C_X;
	axial[13] = ui.axial_C_Y;
	axial[14] = ui.axial_C_Z;
	windowlevel[0] = ui.curWindow;
	windowlevel[1] = ui.curLevel;

	viewer3d = new vtk3DViewerClass(views, saggital, coronal, axial,windowlevel,ui.slicePos, ui.sliceIndex,ui.yDegree);

	tbWindowLevelSetting = new QToolBar();
	tbWindowLevelSetting->addAction(ui.actionUpdate_window_level_settings);
	ui.parWindowLevelToobar->addWidget(tbWindowLevelSetting);
	
	ui.tableWindowLevelPreset->selectRow(0);
	ui.tableWindowLevelPreset->setSelectionBehavior(QAbstractItemView::SelectRows);

	tbVolumePropertySetting = new QToolBar();
	tbVolumePropertySetting->addAction(ui.actionUpdate_volume_property_settings);
	ui.parVolumePropertyToolbar->addWidget(tbVolumePropertySetting);
	ui.tableVolumePropertyPreset->setRowCount(vpSettings->names.size());
	for (auto r = 0; r < vpSettings->names.size(); r++)
		ui.tableVolumePropertyPreset->setItem(r, 0, new QTableWidgetItem(vpSettings->names[r]));
	ui.tableVolumePropertyPreset->selectRow(0);
	ui.tableVolumePropertyPreset->setSelectionBehavior(QAbstractItemView::SelectRows);

	tbHounsfieldFilter = new QToolBar();
	tbHounsfieldFilter->addAction(ui.actionApply_Hounsfield_filter);
	ui.parHounsfieldFilterToolbar->addWidget(tbHounsfieldFilter);

	timerIdle = new QTimer();
	timerIdle->setInterval(0);
	connect(timerIdle, SIGNAL(timeout()), this, SLOT(timerIdleExec()));
	timerIdle->start();


}

void MainWindow::__destroyData()
{

	if (dicomDir)
	{
		delete dicomDir;
		dicomDir = nullptr;
	}

	if (dicomSorter) {

		delete dicomSorter;
		dicomSorter = nullptr;

	};

}

void MainWindow::__destroy3DData()
{
	if (reader3d) {
		delete reader3d;
		reader3d = nullptr;
	}

}

void MainWindow::getDicomDir()
{
	inputNameType = InputNameType::DicomDir;

	QString tmp = QFileDialog::getExistingDirectory(this,
		tr("Open Dicom Folder"),
		dicomDirPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


	if (tmp.isEmpty()) return;
	ui.centralWidget->setVisible(false);

	
	
	inputName = tmp;
	__destroyData();
	dicomSorter = new DicomSorterClass(inputName.toStdString());
	dicomSorter->getFiles();


	DicomTreeClass * model = new DicomTreeClass(this);
	model->setupModelData(dicomSorter->PatientsList, model->rootItem);
	ui.dicomDirView->setModel(model);
	ui.dicomDirView->expandAll();
	selectionModel = ui.dicomDirView->selectionModel();
	connect(this->selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(dicomModelSelection(const QItemSelection&, const QItemSelection&)));

	

	ui.centralWidget->setVisible(!inputName.isEmpty());

}

void MainWindow::getDicomDirAndChannelFile()
{
	inputNameType = InputNameType::DicomDirAndChannel;

	QString tmp = QFileDialog::getExistingDirectory(this,
		tr("Open Dicom Folder"),
		dicomDirPath,
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);


	ui.centralWidget->setVisible(false);

	if (tmp.isEmpty()) return;
	QString tmpFile = (QFileDialog::getOpenFileName(this, "Open predicted channel file", dicomDirPath, "predicted channel files (*.npy);;All files (*.*)"));
	if (tmpFile.isEmpty()) return;



	inputName = tmp;
	predictedChannelFile = tmpFile;
	__destroyData();
	dicomSorter = new DicomSorterClass(inputName.toStdString());
	dicomSorter->getFiles();


	DicomTreeClass * model = new DicomTreeClass(this);
	model->setupModelData(dicomSorter->PatientsList, model->rootItem);
	ui.dicomDirView->setModel(model);
	ui.dicomDirView->expandAll();
	selectionModel = ui.dicomDirView->selectionModel();
	connect(this->selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(dicomModelSelection(const QItemSelection&, const QItemSelection&)));
	
	ui.centralWidget->setVisible(!inputName.isEmpty());
}

void MainWindow::dicomModelSelection(const QItemSelection & selected, const QItemSelection & deselected)
{

	QModelIndexList indexes = selected.indexes();
	if (indexes.size() != 1) return;
	QModelIndex tmp = indexes[0];
	TreeItem *ptr = (TreeItem *)indexes[0].internalPointer();
	DicomSeries * series;
	switch (ptr->level) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:

		SetControlStatus(false);
		ui.tabWidget->setVisible(false);
		series = (DicomSeries *)ptr->internalData;
		__destroy3DData();
		reader3d = new DicomReaderClass(series->seriesFiles);
		double *readerRange = reader3d->getDataRange();
		bool actorView[5] = { ui.actionSagittal_slice_actor->isChecked(),
			ui.actionCoronal_slice_actor->isChecked(),
			ui.actionAxial_slice_actor->isChecked(),
			ui.action3D_actor->isChecked(),
			ui.actionVOI_actor->isChecked()
		};
		if (inputNameType == InputNameType::DicomDir) {
			double hounsfieldFilterRange[2];

			

			hounsfieldFilterRange[0] = readerRange[0];
			hounsfieldFilterRange[1] = readerRange[1];
			

			hounsfieldFilter->Execute(reader3d->getReaderOutputPort(), hounsfieldFilterRange, readerRange[0]);

			
			
			viewer3d->ShowScene(
				hounsfieldFilter->getOutputPort(),
				reader3d,
				vpSettings->get_volume_property(VolumePropertyIndex),
				windowWidth,
				windowLevel,
				actorView
			);
			SetHounsfieldFilter(hounsfieldFilterRange, readerRange);
			
		}

		if (inputNameType == InputNameType::DicomDirAndChannel) {
			vtkDataSetAlgorithm * detectChannel = MathUtilsClass::LoadArrayFromNumpy(
				predictedChannelFile.toStdString(),//"d:/CProg/QtTest/QtTest/predicted_data_orig.npy",
				reader3d->getReaderOutput()->GetScalarType(),
				reader3d->getReaderOutput()->GetNumberOfScalarComponents(),
				reader3d->getDataSpacing(),
				readerRange,
				reader3d->getReaderOutput()->GetInformation()

			);
			vtkImageMathematics * mergeFilter = vtkImageMathematics::New();

			
			
			mergeFilter->SetInputConnection(0, reader3d->getReaderOutputPort());
			mergeFilter->SetInputConnection(1,detectChannel->GetInputConnection(0,0));
			mergeFilter->SetOperationToMax();
			mergeFilter->Update();
			
			vtkAlgorithmOutput * t = mergeFilter->GetOutputPort();
			
			viewer3d->ShowScene(
				mergeFilter->GetOutputPort(),
				
				reader3d,
				vpSettings->get_volume_property(VolumePropertyIndex),
				windowWidth,
				windowLevel,
				actorView
			
			);
			SetHounsfieldFilter(readerRange, readerRange);
			
		}

		

		SetControlStatus(true);
		ui.tabWidget->setVisible(true);

		break;

	}

}
void MainWindow::WindowLevelChange(int row, int column)
{

	windowWidth = ui.tableWindowLevelPreset->item(row, 2)->text().toFloat();
	windowLevel = ui.tableWindowLevelPreset->item(row, 3)->text().toFloat();

}

void MainWindow::VolumePropertyChange(int row, int column)
{
	VolumePropertyIndex = row;
}





void MainWindow::UpdateWindowLevel()
{
	viewer3d->UpdateWindowLevel(windowWidth, windowLevel);
}

void MainWindow::UpdateVolumeProperty()
{
	viewer3d->UpdateVolumeProperty(vpSettings->get_volume_property(VolumePropertyIndex));
}

void MainWindow::timerIdleExec()
{
	bool visibleScene = viewer3d != nullptr && viewer3d->visibleScene;
	ui.actionUpdate_window_level_settings->setEnabled(visibleScene);
	ui.actionUpdate_volume_property_settings->setEnabled(visibleScene);
	ui.actionApply_Hounsfield_filter->setEnabled(visibleScene);
	ui.actionMove_to_slice->setEnabled(visibleScene);
	ui.actionUpdate_axial_normale->setEnabled(visibleScene);
	ui.actionSave_to_Dicom->setEnabled(visibleScene);
	ui.actionSave_to_Stl->setEnabled(visibleScene);
	ui.actionAxial_move->setEnabled(visibleScene);
	ui.actionSave_axial_slice_to_Png->setEnabled(visibleScene);

}



void MainWindow::SaveToDicom()
{
	QString tmp = QFileDialog::getExistingDirectory(this,
		tr("Choose Or Create Directory"),
		dicomDirPath,
		QFileDialog::ShowDirsOnly);
	// | QFileDialog::DontResolveSymlinks

	if (!tmp.isEmpty())
	{
		viewer3d->SaveToDicom(tmp.toStdString(), "%s/IM-0001-%04.4d.dcm", ui.actionUse_voi->isChecked());
	}
}

void MainWindow::SaveToStl()
{
	QString tmpFile = (QFileDialog::getSaveFileName(this, "Choose or create file", dicomDirPath, "stereo lithography files (*.stl);;All files (*.*)"));
	if (tmpFile.isEmpty()) return;
	if (viewer3d->visibleScene) {
		bool usevoi = ui.actionUse_voi->isChecked();
			if (hounsfieldFilter->filteringDone())
			//	vtkWriterClass::writeToStl(tmpFile.toStdString(), hounsfieldFilter->getOutputPort(), hounsfieldFilter->getFilterRange());
				viewer3d->SaveToStl(tmpFile.toStdString(), hounsfieldFilter->getFilterRange(), 20,usevoi);
			else  
				viewer3d->SaveToStl(tmpFile.toStdString(), reader3d->getDataRange(), 10, usevoi);
		}
	else {
		vtkWriterClass::writeToStl(tmpFile.toStdString(), reader3d->getReaderOutputPort(), reader3d->getDataRange(),5);
	}
 	

}

void MainWindow::SaveToPng()
{
	QString tmpFile = (QFileDialog::getSaveFileName(this, "Choose or create file", dicomDirPath, "PNG files (*.png);;All files (*.*)"));
	if (tmpFile.isEmpty()) return;
	if (ui.actionSaveSaggitalToPng->isChecked())
		viewer3d->SaveToPng(tmpFile.toStdString(),0);
	if (ui.actionSaveCoronalToPng->isChecked())
		viewer3d->SaveToPng(tmpFile.toStdString(), 1);
	if (ui.actionSaveAxialToPng->isChecked())
		viewer3d->SaveToPng(tmpFile.toStdString(), 2);
}


void MainWindow::RotateAxialPlane()
{
	viewer3d->RotateAxialPlane();
}

void MainWindow::ApplyHounsfieldFilter()
{
	
	double hounsfieldFilterRange[2];

	hounsfieldFilterRange[0] = ui.minHounsfieldFilter->value();
	hounsfieldFilterRange[1] = ui.maxHounsfieldFilter->value();
	double * dataRange = reader3d->getDataRange();
	
	hounsfieldFilter->Execute(reader3d->getReaderOutputPort(), hounsfieldFilterRange, dataRange[0]);
	viewer3d->UpdateScene(
		hounsfieldFilter->getOutputPort(),
		vpSettings->get_volume_property(VolumePropertyIndex)
		
	);

}

void MainWindow::MoveToAxialSlice()
{
	QString tmp = ui.sliceIndex->text();
	double axialSlice = tmp.toDouble();//ui.sliceIndex->text().toInt();
	viewer3d->MoveToSlice(int(axialSlice));
}

void MainWindow::UpdateAxialNormal()
{
	double normal[3];
	normal[0] = ui.axial_N_X->text().toDouble();
	normal[1] = ui.axial_N_Y->text().toDouble();
	normal[2] = ui.axial_N_Z->text().toDouble();
	vtkMath::Normalize(normal);
	viewer3d->UpdateAxialNormal(normal);
}

void MainWindow::actor_view_change(bool checked)
{
	bool visibleScene = viewer3d != nullptr && viewer3d->visibleScene;
	if (!visibleScene) return;
	bool actorView[5] = { ui.actionSagittal_slice_actor->isChecked(),
			ui.actionCoronal_slice_actor->isChecked(),
			ui.actionAxial_slice_actor->isChecked(),
			ui.action3D_actor->isChecked(),
			ui.actionVOI_actor->isChecked()
	};
	viewer3d->UpdateActorView(actorView);
	/*
	QAction* pAction = qobject_cast<QAction*>(sender());
	if (pAction == ui.action3D_actor) {
		return;
	}
	*/
	
}

void MainWindow::SetControlStatus(bool status)
{
	if (status) this->setCursor(Qt::ArrowCursor);
	else
	{
		this->setCursor(Qt::WaitCursor);
	}
	ui.actionOpen_DicomDir_File->setEnabled(status);
	ui.actionExit->setEnabled(status);

	QApplication::processEvents();


}

void MainWindow::SetHounsfieldFilter(double filterRange[2], double minmaxRange[2])
{
	ui.minHounsfieldFilter->setMinimum(minmaxRange[0]);
	ui.minHounsfieldFilter->setMaximum(minmaxRange[1]);
	ui.minHounsfieldFilter->setValue(filterRange[0]);
	ui.maxHounsfieldFilter->setMinimum(minmaxRange[0]);
	ui.maxHounsfieldFilter->setMaximum(minmaxRange[1]);
	ui.maxHounsfieldFilter->setValue(filterRange[1]);

}

void MainWindow::CreatePngSaveMenu()
{
	ui.mainToolBar->insertWidget(ui.actionUse_voi, ui.planeSavePngChoice);
	QMenu * menuSavePng = new QMenu("mymenu");
	QActionGroup  * group = new QActionGroup(this);

	menuSavePng->addAction(ui.actionSaveSaggitalToPng);
	menuSavePng->addAction(ui.actionSaveCoronalToPng);
	menuSavePng->addAction(ui.actionSaveAxialToPng);

	group->addAction(ui.actionSaveSaggitalToPng);
	group->addAction(ui.actionSaveCoronalToPng);
	group->addAction(ui.actionSaveAxialToPng);
	ui.planeSavePngChoice->setMenu(menuSavePng);

}

string MainWindow::getApplicationPath()
{
	QStringList arg = qApp->arguments();
	std::filesystem::path appPath = arg[0].toStdString();
	appPath.replace_filename("presets.xml");
	return (appPath.string());
	
}


void MainWindow::getDicomDirFile() {
	inputNameType = InputNameType::DicomDirFile;

	inputName = (QFileDialog::getOpenFileName(this, "Open Dicomdir file", dicomDirPath, "dicomdir (dicomdir);;All files (*.*)"));
	if (!inputName.isEmpty())
	{
		__destroyData();
		dicomDir = new DicomDirClass(inputName.toStdString());
		dicomDir->GetPatients();


	}

}




