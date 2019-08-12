#include "vtk3DViewerClass.h"




class vtkImageResliceCallback : public vtkCommand
{
public:
	static vtkImageResliceCallback *New() { 
		return new vtkImageResliceCallback;
		
			}

	void setmainViewer(vtk3DViewerClass * mainViewer) { this->mainViewer = mainViewer; }
	

	void Execute(vtkObject *caller, unsigned long ev, void *callData) override
	{

		int sliceInd;

		if (ev == vtkCommand::WindowLevelEvent) {
			vtkInteractorStyleImage* interActorStyle = dynamic_cast<vtkInteractorStyleImage*>(caller);
			int wlCurPos[2];
			int wlStartPos[2];
			interActorStyle->GetWindowLevelCurrentPosition(wlCurPos);
			interActorStyle->GetWindowLevelStartPosition(wlStartPos);
			
			//mainViewer->windowlevel[0]->setText(QString::number(wlCurPos[0] - wlStartPos[0], 'f', 3));
			//mainViewer->windowlevel[1]->setText(QString::number(wlCurPos[1] - wlStartPos[1], 'f', 3));
			int direction = wlCurPos[1] - wlStartPos[1];
			
			if (interActorStyle) {
				if (interActorStyle == mainViewer->views[0]->GetInteractor()->GetInteractorStyle()) {
					//mainViewer->ShowColorWindowLevel(mainViewer->sliceActor[0]);
					//mainViewer->ShowColorWindowLevel(mainViewer->planeWidget[0]);
					mainViewer->SetColorWindowLevel(0, direction);
					return;
				}
				
				if (interActorStyle == mainViewer->views[1]->GetInteractor()->GetInteractorStyle()) {
					//mainViewer->ShowColorWindowLevel(mainViewer->sliceActor[1]);
					//mainViewer->ShowColorWindowLevel(mainViewer->planeWidget[1]);
					mainViewer->SetColorWindowLevel(1, direction);
					return;
				}

				if (interActorStyle == mainViewer->views[2]->GetInteractor()->GetInteractorStyle()) {
					//mainViewer->ShowColorWindowLevel(mainViewer->sliceActor[2]);
					//mainViewer->ShowColorWindowLevel(mainViewer->planeWidget[2]);
					mainViewer->SetColorWindowLevel(2, direction);
					return;
				}
			
				
			}
		}
		if (ev == vtkCommand::MouseWheelForwardEvent || ev == vtkCommand::MouseWheelBackwardEvent) {
			int sign = (ev == vtkCommand::MouseWheelForwardEvent) ? 1 : -1;
			vtkRenderWindowInteractor* interActor = dynamic_cast<vtkRenderWindowInteractor*>(caller);

			if (interActor) {
				
				
				if (interActor == mainViewer->views[0]->GetInteractor()) {
					sliceInd = UpdatePlaneWidget(0,  sign);
					UpdateImageReslice(0);
					
					mainViewer->ShowSaggital(static_cast<vtkPlaneSource *>(mainViewer->planeWidget[0]->GetPolyDataAlgorithm()));
					UpdateVoi(0, sliceInd);
					mainViewer->views[3]->GetInteractor()->GetRenderWindow()->Render();
					return;
				}

				if (interActor == mainViewer->views[1]->GetInteractor()) {
					sliceInd = UpdatePlaneWidget(1, sign);
					UpdateImageReslice(1);
					mainViewer->ShowCoronal(static_cast<vtkPlaneSource *>(mainViewer->planeWidget[1]->GetPolyDataAlgorithm()));
					UpdateVoi(1, sliceInd);
					mainViewer->views[3]->GetInteractor()->GetRenderWindow()->Render();
					return;
				}

				if (interActor == mainViewer->views[2]->GetInteractor()) {
					
					sliceInd = UpdatePlaneWidget(2, sign);
					UpdateImageReslice(2);
					mainViewer->ShowAxial(static_cast<vtkPlaneSource *>(mainViewer->planeWidget[2]->GetPolyDataAlgorithm()));
					mainViewer->views[3]->GetInteractor()->GetRenderWindow()->Render();
					return;
				}
			}
			
		}
	
	}



	vtkImageResliceCallback() {}
	
private:

	vtk3DViewerClass * mainViewer;

	int UpdatePlaneWidget(int plane, int sign);
	void UpdateImageReslice(int plane);
	void UpdateVoi(int plane, int sliceInd);
};


int vtkImageResliceCallback::UpdatePlaneWidget(int plane, int sign)
{
	double origin[3];
	//double center[3];
	double point1[3];
	double point2[3];
	double normal[3];

	double normalOld[3];
	vtkPlaneSource *ps = static_cast<vtkPlaneSource *>(mainViewer->planeWidget[plane]->GetPolyDataAlgorithm());

	if (plane == 2) {
		ps->GetNormal(normalOld);
		if (normalOld[2] != 1) {
			mainViewer->planeWidget[plane]->SetSliceIndex(mainViewer->axialIndex);
			mainViewer->SetAxialCharacteristics(static_cast<vtkPlaneSource *>(mainViewer->planeWidget[plane]->GetPolyDataAlgorithm()));
		}
	}
	mainViewer->planeWidget[plane]->GetOrigin(origin);
	mainViewer->planeWidget[plane]->GetPoint1(point1);
	mainViewer->planeWidget[plane]->GetPoint2(point2);

	double offset = origin[plane];
	offset += sign * (mainViewer->dataSpacing[plane]);
	offset = (offset > mainViewer->sliceMaxBorder[plane]) ? 0 :
		(offset < 0) ? mainViewer->sliceMaxBorder[plane] : offset;
	origin[plane] = offset;

	/*
	offset = point1[plane];
	
	offset += sign * (dataSpacing[plane]);
	offset = (offset > sliceMaxBorder[plane]) ? 0 :
		(offset < 0) ? sliceMaxBorder[plane] : offset;
		*/
	point1[plane] = offset;

	/*
	offset = point2[plane];
	offset += sign * (dataSpacing[plane]);
	offset = (offset > sliceMaxBorder[plane]) ? 0 :
		(offset < 0) ? sliceMaxBorder[plane] : offset;
		*/
	point2[plane] = offset;

	mainViewer->planeWidget[plane]->SetOrigin(origin);
	mainViewer->planeWidget[plane]->SetPoint1(point1);
	mainViewer->planeWidget[plane]->SetPoint2(point2);
	//ps->SetNormal(normalOld);
	//planeWidget[plane]->GetNormal(normal);


	mainViewer->planeWidget[plane]->UpdatePlacement();
	int sliceInd = mainViewer->planeWidget[plane]->GetSliceIndex();

	mainViewer->sliceIndex->setText(QString::number(sliceInd, 'f', 3));
	mainViewer->slicePos->setText(QString::number(mainViewer->planeWidget[plane]->GetSlicePosition(), 'f', 3));
	return sliceInd;
}



void vtkImageResliceCallback::UpdateImageReslice(int plane)
{
	vtkMatrix4x4 * axes = vtkMatrix4x4::New();
	axes->DeepCopy(mainViewer->planeWidget[plane]->GetResliceAxes());
	
	if (plane == 1) {
		axes->SetElement(0, 0, 1);
		axes->SetElement(0, 1, 0);
		axes->SetElement(2, 0, 0);
		axes->SetElement(2, 1, 1);
	}
	mainViewer->riw[plane]->SetResliceAxes(axes);
	mainViewer->riw[plane]->Update();
	mainViewer->views[plane]->GetInteractor()->GetRenderWindow()->Render();

}
void vtkImageResliceCallback::UpdateVoi(int plane, int sliceInd)
{
	int voi[6];
	mainViewer->voi3D->GetVOI(voi);
	switch (plane)
	{
	case 0:
		voi[0] = sliceInd - 127;
		voi[1] = sliceInd + 128;
		if (voi[0] < 0) {
			voi[0] = 0;
			voi[1] = mainViewer->dataExtention[1];
			return;
		}
		if (voi[1] > mainViewer->dataExtention[1]) {
			voi[0] = 0;
			voi[1] = mainViewer->dataExtention[1];
			return;
		}
		
		break;
	case 1:
		voi[2] = sliceInd - 127;
		voi[3] = sliceInd + 127;
		if (voi[2] < 0) {
			voi[2] = 0;
			voi[3] = mainViewer->dataExtention[3];
			return;
		}
		if (voi[3] > mainViewer->dataExtention[3]) {
			voi[2] = 0;
			voi[3] = mainViewer->dataExtention[3];
			return;
		}
		break;
	
	}
	mainViewer->voi3D->SetVOI(voi);
	mainViewer->voi3D->Update();
}




VPSettings::VPSettings(QString vppath)
{
	//QFile file("d:\\CProg\\QtTest\\x64\\Debug\\presets.xml");
	QFile file(vppath);

	
	if (file.open(QFile::ReadOnly | QFile::Text)) {
		QXmlStreamReader xmlReader;
		xmlReader.setDevice(&file);
		QStringRef name;
		QString attr_name, attr_value;
		while (!xmlReader.atEnd()) {
			if (xmlReader.isStartElement()) {
				name = xmlReader.name();
				if (name == "VolumeProperty") {
					VP_SETTING vp;
					foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
						
						attr_name = attr.name().toString();

						attr_value = attr.value().toString();

						vp.insert({ attr_name.toStdString(), attr_value.toStdString() });
						
						if (attr_name == "name") {
							names.append(attr_value);
						}

					}

					vpsettings.push_back(vp);

				}
			}
			
			xmlReader.readNext();
		}
	}
}

VPSettings::~VPSettings()
{
	vpsettings.clear();
}

VP_SETTING VPSettings::get_volume_property(int index)
{
	if (index > vpsettings.size() - 1)
		return vpsettings[0];
	else 
		return vpsettings[index];
}
/*
ColorPoint::ColorPoint(double x,
	double red,
	double green,
	double blue,
	double midpoint,
	double sharpness
)
{
	this->x = x;
	this->red = red;
	this->green = green;
	this->blue = blue;
	this->midpoint = midpoint;
	this->sharpness = sharpness;
}

ColorPoint::~ColorPoint()
{
}

OpacityPoint::OpacityPoint(
	double x,
	double y,
	double midpoint,
	double sharpness
)
{
	this->x = x;
	this->y = y;
	this->midpoint = midpoint;
	this->sharpness = sharpness;
}

OpacityPoint::~OpacityPoint()
{
}

ColorOpacitySettings::ColorOpacitySettings(
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
)
{
	for (int i = 0; i < points.size(); i++)
	{
		colorPoints.push_back(new ColorPoint(points[i], reds[i], greens[i], blues[i], colormidpoints[i], colorsharpness[i]));
		opacityPoints.push_back(new OpacityPoint(points[i], opacitys[i], opacitysmidpoints[i], opacityssharpness[i]));

	}

	for (int i = 0; i < gradientpoints.size(); i++)
	{

		gradientPoints.push_back(new OpacityPoint(gradientpoints[i], gradients[i], gradientsmidpoints[i], gradientssharpness[i]));

	}

	this->BlendMode = BlendMode;
	this->Shade = Shade;
	this->Ambient = Ambient;
	this->Diffuse = Diffuse;
	this->Specular = Specular;
	this->SpecularPower = SpecularPower;
	this->ScalarOpacityUnitDistance = ScalarOpacityUnitDistance;
}


ColorOpacitySettings::~ColorOpacitySettings()
{
	//list<ColorPoint*> colorPoints;
	list<ColorPoint*>::iterator iter = colorPoints.begin();

	while (iter != colorPoints.end())
	{
		ColorPoint * tmp = *iter;
		delete tmp;
		colorPoints.erase(iter++);
	}

	list<OpacityPoint*>::iterator iter1 = opacityPoints.begin();

	while (iter1 != opacityPoints.end())
	{
		OpacityPoint * tmp = *iter1;
		delete tmp;
		opacityPoints.erase(iter1++);
	}

	list<OpacityPoint*>::iterator iter2 = gradientPoints.begin();

	while (iter2 != gradientPoints.end())
	{
		OpacityPoint * tmp = *iter2;
		delete tmp;
		gradientPoints.erase(iter2++);
	}
}

ColorOpacitySettingsList::ColorOpacitySettingsList()
{


	colorOpacitySettings.push_back(
		new ColorOpacitySettings(
			{ -3024, -16.4458,641.385,3071,4000 },//points
			{ 0, 0.729412 ,0.905882 ,1,1 }, //red
			{ 0, 0.254902 ,0.815686 ,1,0 }, //green
			{ 0, 0.301961 ,0.552941 ,1,0 }, //blue
			{ 0.5 ,0.492958,0.5,0.5, 0.5},//colormidpoints,
			{ 0.0,0.61,0.0,0.0,1.0},//colorsharpness,
			{ 0.0,0.0, 0.715686, 0.705882,1 },//opacitys,
			{ 0.5 ,0.492958 ,0.5,0.5,0.5 },//opacitysmidpoints,
			{ 0.0,0.61,0.0,0.0,1.0 },//opacityssharpness,
			{ 0,255 },//gradientpoints,
			{ 1,1 },//gradients,
			{ 0.5,0.5 },//gradientsmidpoints,
			{ 0,0 },//gradientssharpness,
			0,
			1,
			0.1,
			0.9,
			0.2,
			10,
			0.891927
		)
	);


}

ColorOpacitySettingsList::~ColorOpacitySettingsList()
{

	vector<ColorOpacitySettings*>::iterator iter = colorOpacitySettings.begin();

	while (iter != colorOpacitySettings.end())
	{
		ColorOpacitySettings * tmp = *iter;
		delete tmp;
		//colorPoints.erase(iter++);
		iter++;
	}
	colorOpacitySettings.clear();
	
	
}
*/
vtk3DViewerClass::vtk3DViewerClass(
									QVTKOpenGLWidget * views[4], 
									QLineEdit * saggital[15], 
									QLineEdit * coronal[15], 
									QLineEdit * axial[15],
									QLineEdit * windowlevel[2],
									QLineEdit * slicePos,
									QLineEdit * sliceIndex,
									QLineEdit * yDegree
				
										)
{
	for (int i = 0; i < 4; i++)
	{
		this->views[i] = views[i];

	}

	ipwProp = vtkProperty::New();

	renderer = vtkRenderer::New();

	renderWindow = vtkGenericOpenGLRenderWindow::New();

	//mapper = vtkOpenGLGPUVolumeRayCastMapper::New();
	mapper = vtkGPUVolumeRayCastMapper::New();
	
	volume3D = vtkVolume::New();

	voi3D = vtkExtractVOI::New();
	voiData = vtkOutlineFilter::New();
	voiDataMapper = vtkPolyDataMapper::New();
	voiActor = vtkActor::New();

	

	colorFun = vtkColorTransferFunction::New();
	opacityFun = vtkPiecewiseFunction::New();
	gradientOpacity = vtkPiecewiseFunction::New();
	volume3Property = vtkVolumeProperty::New();
	
	

	for (int i = 0; i < 3; i++)
	{
		
		riw[i] = vtkImageReslice::New();
		riw[i]->SetOutputDimensionality(2);
		sliceMaptocolors[i] = vtkImageMapToColors::New();
		sliceActor[i] = vtkImageActor::New();
		
		sliceLut[i] = vtkWindowLevelLookupTable::New();
		
		sliceRenderer[i] = vtkRenderer::New();
		//sliceRenderer[i]->AddActor(sliceActor[i]);

		renderWindows[i] = vtkGenericOpenGLRenderWindow::New();
		views[i]->SetRenderWindow(renderWindows[i]);

		views[i]->GetRenderWindow()->AddRenderer(sliceRenderer[i]);
		views[i]->GetInteractor()->SetInteractorStyle(vtkInteractorStyleImage::New());

		
		
		sliceRenderer[i]->AddActor(sliceActor[i]);
		
	}
	
	picker = vtkCellPicker::New();
	picker->SetTolerance(0.005);
	

	views[3]->SetRenderWindow(renderWindow);
	views[3]->GetRenderWindow()->AddRenderer(renderer);
	iren = views[3]->GetInteractor();

	for (int i = 0; i < 3; i++)
	{
		planeWidget[i] = vtkImagePlaneWidget::New();
		planeWidget[i]->SetInteractor(iren);
		planeWidget[i]->SetPicker(picker);
		
		
	}

	this->setUI(saggital, coronal, axial,windowlevel,yDegree,slicePos,sliceIndex);

	cbkNew = vtkImageResliceCallback::New();
	
	for (int i = 0; i < 3; i++)
	{


		//views[i]->GetInteractor()->RemoveObserver(vtkCommand::WindowLevelEvent);
		//views[i]->GetInteractor()->GetInteractorStyle()->RemoveObservers(vtkCommand::WindowLevelEvent);
		views[i]->GetInteractor()->RemoveObservers(vtkCommand::MouseWheelForwardEvent);
		views[i]->GetInteractor()->RemoveObservers(vtkCommand::MouseWheelBackwardEvent);

		views[i]->GetInteractor()->AddObserver(vtkCommand::MouseWheelForwardEvent, cbkNew);
		views[i]->GetInteractor()->AddObserver(vtkCommand::MouseWheelBackwardEvent, cbkNew);
		views[i]->GetInteractor()->GetInteractorStyle()->AddObserver(vtkCommand::WindowLevelEvent, cbkNew);
		
	}
	
	renderer->AddVolume(volume3D);
	renderer->AddActor(voiActor);
}

vtk3DViewerClass::~vtk3DViewerClass()
{
	mapper->Delete();
	volume3D->Delete();
	colorFun->Delete();
	opacityFun->Delete();
	gradientOpacity->Delete();
	volume3Property->Delete();

	for (int i = 0; i < 3; i++)
	{
		if (riw[i]) {
			riw[i]->Delete();
		}
		if (planeWidget[i])
			planeWidget[i]->Delete();
		if (renderWindows[i])
			renderWindows[i]->Delete();

	}

	if (picker)
		picker->Delete();
	if (ipwProp)
		ipwProp->Delete();
	if (renderer)
		renderer->Delete();
	if (renderWindow)
		renderWindow->Delete();
	if (cbkNew)
		cbkNew->Delete();

}

void vtk3DViewerClass::ShowScene(
	vtkAlgorithmOutput* outputPort,
	DicomReaderClass * reader,
	VP_SETTING vpSetting,
	float windowWidth,
	float windowLevel,
	bool actorView[5]
)
{


	this->windowWidth = windowWidth;
	this->windowLevel = windowLevel;
	this->outputPort = outputPort;

	this->MetaData = reader->getMetaData();
	this->PatientMatrix = reader->GetPatientMatrix();
	this->dataRange = reader->getDataRange();
	this->dataSpacing = reader->getDataSpacing();
	this->dataExtention = reader->getDataExtention();
	this->NumberOfScalarComponents = reader->getNumberOfScalarComponents();


	opacityWindow = dataRange[1] - dataRange[0];
	opacityLevel = (dataRange[0] + dataRange[1]) / 2.0;
	//this->ctSetting = ctSetting;
	this->vpSetting = vpSetting;

	sliceCount[0] = dataExtention[1];
	sliceCount[1] = dataExtention[3];
	if (this->NumberOfScalarComponents > 1) {
		sliceCount[2] = int((dataExtention[5] + 1) / NumberOfScalarComponents - 1);
		volume3Property->SetIndependentComponents(false);
	}
	else{

		sliceCount[2] = dataExtention[5];
	    volume3Property->SetIndependentComponents(true);
		}

	voi3D->RemoveAllInputConnections(0);
	voi3D->SetInputConnection(outputPort);
	
	voi[0] = sliceCount[0] / 2 - 127;
	voi[1] = sliceCount[0] / 2 + 128;
	voi[3] = dataExtention[3] - 80;
	voi[2] = voi[3] - 255;

	voi[4] = dataExtention[4];
	voi[5] = sliceCount[2];// dataExtention[5];

	voi3D->SetVOI(
					voi[0], voi[1],
					voi[2], voi[3],
					voi[4], voi[5]);
					
	voi3D->Update();

	voiData->RemoveAllInputConnections(0);
	voiData->SetInputConnection(voi3D->GetOutputPort());
	voiData->Update();
	voiDataMapper->RemoveAllInputConnections(0);
	voiDataMapper->SetInputConnection(voiData->GetOutputPort());
	voiDataMapper->Update();
	voiActor->SetMapper(voiDataMapper);
	voiActor->GetProperty()->SetColor(1, 1, 1);
	
	createCT();
	
	mapper->RemoveAllInputConnections(0);
	mapper->SetLockSampleDistanceToInputSpacing(true);
	mapper->SetInputConnection(outputPort);
	mapper->Update();

	
	volume3Property->SetScalarOpacity(opacityFun);
	volume3Property->SetColor(colorFun);
	volume3Property->SetGradientOpacity(gradientOpacity);
	

	volume3D->SetProperty(volume3Property);
	volume3D->SetMapper(mapper);


	
	volume3D->SetUserMatrix(PatientMatrix);
	
	
	double x = PatientMatrix->GetElement(0, 3);
	double y = PatientMatrix->GetElement(1, 3);
	double z = PatientMatrix->GetElement(2, 3);
	x = (x < 0) ? fabs(x) : -x;
	y = (y < 0) ? fabs(y) : -y;
	z = (z >= 0) ? -z : fabs(z);
	volume3D->SetPosition(x, y , z);
	volume3D->Update();
	
	volume3D->SetVisibility(actorView[3]);
	voiActor->SetVisibility(actorView[4]);

	for (int i = 0; i < 3; i++)
	{

		planeWidget[i]->RestrictPlaneToVolumeOn();
		double color[3] = { 0, 0, 0 };
		
		color[i] = 1;
		planeWidget[i]->GetPlaneProperty()->SetColor(color);

		color[0] /= 4.0;
		color[1] /= 4.0;
		color[2] /= 4.0;
		
		planeWidget[i]->SetTexturePlaneProperty(ipwProp);
		planeWidget[i]->TextureInterpolateOn();
		planeWidget[i]->SetResliceInterpolateToLinear();
		
		
		
		planeWidget[i]->SetInputConnection(outputPort);
		//planeWidget[i]->SetInputConnection(voi3D->GetOutputPort());
		planeWidget[i]->SetPlaneOrientation(i);
		if (i==1)
		 planeWidget[i]->SetSliceIndex(voi[3]-127);
		else 
		 planeWidget[i]->SetSliceIndex(sliceCount[i] / 2);
		//planeWidget[i]->SetSliceIndex(0);
		//planeWidget[i]->SetSliceIndex(sliceCount[i]);
		planeWidget[i]->DisplayTextOn();
		planeWidget[i]->SetDefaultRenderer(renderer);

		planeWidget[i]->SetWindowLevel(this->windowWidth, this->windowLevel);
		
		
		
		planeWidget[i]->On();
		
	
		planeWidget[i]->InteractionOff();

	

	

		sliceLut[i]->DeepCopy(reader->getLUT());
		
		sliceLut[i]->SetWindow(this->windowWidth);
		sliceLut[i]->SetLevel(this->windowLevel);
		
		
		sliceLut[i]->SetBelowRangeColor(0, 0, 0,1);
		sliceLut[i]->SetAboveRangeColor(0, 0, 0,1);
		sliceLut[i]->SetNanColor(0, 0, 0, 1);
		
		
		sliceLut[i]->BuildSpecialColors();

		sliceMaptocolors[i]->SetLookupTable(sliceLut[i]);
		
		
		
		riw[i]->RemoveAllInputConnections(0);
		riw[i]->SetInputConnection(outputPort);
		//riw[i]->SetInputConnection(voi3D->GetOutputPort());
		
		vtkMatrix4x4 * axes = vtkMatrix4x4::New();
		
		
		setPlaneWidgitVisibility(actorView[i], i);

		switch (i)
		{
		case 0://saggital
			planeWidget[i]->GetNormal(nSaggital);
			axes->DeepCopy(planeWidget[i]->GetResliceAxes());
			

			
			break;
		case 1://coronal
			/*
			coronal.DeepCopy((1, 0, 0, center[0],
  41                   0, 0, 1, center[1],
  42                   0,-1, 0, center[2],
  43                   0, 0, 0, 1))
			1*/
			
			
			
			
			axes->Zero();
			axes->SetElement(0, 0, 1);
			axes->SetElement(0, 3, planeWidget[i]->GetResliceAxes()->GetElement(0, 3));
			
			axes->SetElement(1, 2, 1);
			axes->SetElement(1, 3, planeWidget[i]->GetResliceAxes()->GetElement(1, 3));
			axes->SetElement(2, 1, 1);
			axes->SetElement(2, 3, planeWidget[i]->GetResliceAxes()->GetElement(2, 3));
			axes->SetElement(3, 3, 1);
			
			

			
			
			break;

		case 2://axial
			planeWidget[i]->GetNormal(nAxial);
			axes->DeepCopy(planeWidget[i]->GetResliceAxes());
			/*
			riw[i]->SetOutputSpacing(dataSpacing[0], dataSpacing[1], 1);
			riw[i]->SetOutputExtent(dataExtention[0], dataExtention[1], dataExtention[2], dataExtention[3], 0, 0);
			riw[i]->SetInterpolationMode(1);
			riw[i]->SetTransformInputSampling(false);
			*/
			
			
			break;
			

		}
	
		
		
		riw[i]->SetResliceAxes(axes);
		riw[i]->SetOutputSpacing(dataSpacing);
		riw[i]->Update();

		sliceMaptocolors[i]->SetInputConnection(riw[i]->GetOutputPort());
		sliceMaptocolors[i]->Update();
		sliceActor[i]->SetUserMatrix(PatientMatrix);

		
		

		sliceActor[i]->GetMapper()->SetInputConnection(sliceMaptocolors[i]->GetOutputPort());

	
		
		
		sliceActor[i]->Update();

		sliceMaxBorder[i] = dataSpacing[i] * sliceCount[i];
		
		
	}

	
	for (int i = 0; i < 3; i++)
	{
		
		sliceRenderer[i]->ResetCamera();
		planeWidget[i]->GetCurrentRenderer()->ResetCameraClippingRange();
	}

	vtkPlaneSource *psSaggital = static_cast<vtkPlaneSource *>(planeWidget[0]->GetPolyDataAlgorithm());
	vtkPlaneSource *psCoronal = static_cast<vtkPlaneSource *>(planeWidget[1]->GetPolyDataAlgorithm());
	vtkPlaneSource *psAxial = static_cast<vtkPlaneSource *>(planeWidget[2]->GetPolyDataAlgorithm());

	ShowSaggital(psSaggital);
	ShowCoronal(psCoronal);
	ShowAxial(psAxial);
	ShowColorWindowLevel(planeWidget[0]);
	


	for (int i = 0; i < 4; i++)
	{
		views[i]->GetRenderWindow()->Render();
	
	}

	visibleScene = true;
	
	GetAxialCharacteristics(static_cast<vtkPlaneSource *>(planeWidget[2]->GetPolyDataAlgorithm()));
	cbkNew->setmainViewer(this);
}

void vtk3DViewerClass::setUI(QLineEdit * saggital[15], QLineEdit * coronal[15], QLineEdit * axial[15], QLineEdit * windowlevel[2],
	QLineEdit * yDegree, QLineEdit * slicePos, QLineEdit * sliceIndex)

{
	for (int i = 0; i < 15; i++)
	{
		this->saggital[i] = saggital[i];
		this->coronal[i] = coronal[i];
		this->axial[i] = axial[i];
	}
	this->windowlevel[0] = windowlevel[0];
	this->windowlevel[1] = windowlevel[1];
	this->yDegree = yDegree;
	this->slicePos = slicePos;
	this->sliceIndex = sliceIndex;

}

void vtk3DViewerClass::UpdateScene(
			vtkAlgorithmOutput* inputData,
			VP_SETTING vpSetting
)
{
	int sliceIndex;

	this->vpSetting = vpSetting;
	
	createCT();
	this->outputPort = inputData;
	mapper->RemoveAllInputConnections(0);
	mapper->SetInputConnection(inputData);
	mapper->Update();
	

	for (int i = 0; i < 3; i++)
	{
		//planeWidget[i]->GetReslice()->RemoveAllInputConnections(0);
		sliceIndex = planeWidget[i]->GetSliceIndex();
		planeWidget[i]->SetInputConnection(inputData);
		planeWidget[i]->SetPlaneOrientation(i);
		planeWidget[i]->SetSliceIndex(sliceIndex);
		planeWidget[i]->UpdatePlacement();
		
		riw[i]->SetInputConnection(inputData);
		if (i == 1) {
			vtkMatrix4x4 * axes = vtkMatrix4x4::New();
			axes->Zero();
			axes->SetElement(0, 0, 1);
			axes->SetElement(0, 3, planeWidget[i]->GetResliceAxes()->GetElement(0, 3));

			axes->SetElement(1, 2, 1);
			axes->SetElement(1, 3, planeWidget[i]->GetResliceAxes()->GetElement(1, 3));
			axes->SetElement(2, 1, 1);
			axes->SetElement(2, 3, planeWidget[i]->GetResliceAxes()->GetElement(2, 3));
			axes->SetElement(3, 3, 1);
			
			riw[i]->SetResliceAxes(axes);
		}
		riw[i]->Update();

		
	}
	voi3D->RemoveAllInputConnections(0);
	voi3D->SetInputConnection(inputData);
	voi3D->Update();

	for (int i = 0; i < 4; i++)
	{
		views[i]->GetRenderWindow()->Render();
	}

	
	
}

void vtk3DViewerClass::UpdateWindowLevel(float windowWidth, float windowLevel)
{
	
	this->windowWidth = windowWidth;
	this->windowLevel = windowLevel;
	for (int i = 0; i < 3; i++)
	{
	
		sliceLut[i]->SetWindow(this->windowWidth);
		sliceLut[i]->SetLevel(this->windowLevel);
		
		planeWidget[i]->SetWindowLevel(this->windowWidth, this->windowLevel);
	
	}

	
	for (int i = 0; i <4; i++)
	{
		views[i]->GetRenderWindow()->Render();
		
		
	}

	this->ShowColorWindowLevel(planeWidget[0]);
	
}

void vtk3DViewerClass::UpdateVolumeProperty(VP_SETTING vpSetting)
{
	this->vpSetting = vpSetting;

	createCT();
	
	mapper->Update();

	for (int i = 0; i < 4; i++)
	{
		views[i]->GetRenderWindow()->Render();


	}
}

void vtk3DViewerClass::MoveToSlice(int slice)
{
	planeWidget[2]->SetSliceIndex(slice);
	planeWidget[2]->UpdatePlacement();
	riw[2]->SetResliceAxes(planeWidget[2]->GetResliceAxes());
	riw[2]->Update();
	views[3]->GetRenderWindow()->Render();
	views[2]->GetRenderWindow()->Render();
	ShowAxial(static_cast<vtkPlaneSource *>(planeWidget[2]->GetPolyDataAlgorithm()));
	QApplication::processEvents();

}

void vtk3DViewerClass::UpdateActorView(bool actorView[5])
{
	volume3D->SetVisibility(actorView[3]);
	voiActor->SetVisibility(actorView[4]);
	for (int i = 0; i < 3; i++)
	{
		setPlaneWidgitVisibility(actorView[i], i);

	}
	views[3]->GetRenderWindow()->Render();
}




void vtk3DViewerClass::SaveToDicom(string filePrefix, string filePattern, bool voi)
{
	
	if (voi) 
	 vtkWriterClass::writeToDicom(voi3D->GetOutputPort(), MetaData, filePrefix, filePattern,PatientMatrix);
	else
    	vtkWriterClass::writeToDicom(this->outputPort, MetaData, filePrefix, filePattern,  PatientMatrix);
}

void vtk3DViewerClass::SaveToStl(string fileName, double filterRange[2], int numContours, bool voi)
{
	if (voi)
		 vtkWriterClass::writeToStl(fileName, voi3D->GetOutputPort(), filterRange, numContours);
	else
    	vtkWriterClass::writeToStl(fileName, this->outputPort, filterRange,numContours);

}

void vtk3DViewerClass::SaveToPng(string fileName, int planeIndex)
{
	double range[2];
	planeWidget[planeIndex]->GetReslice()->Update();
	planeWidget[planeIndex]->GetReslice()->GetOutput()->GetScalarRange(range);
	
	//vtkWriterClass::writeToPng(fileName, range,planeWidget[planeIndex]->GetReslice()->GetOutputPort());
	
	vtkWriterClass::writeToPng(fileName, range, riw[planeIndex]->GetOutputPort());
}

void vtk3DViewerClass::RotateAxialPlane()
{
	vtkPlaneSource *psAxial = static_cast<vtkPlaneSource *>(planeWidget[2]->GetPolyDataAlgorithm());

	GetAxialCharacteristics(psAxial);

	

	double normalAxial[3];
	double testNormal[3];
	vtkMatrix4x4 * axes = vtkMatrix4x4::New();
	vtkMatrix4x4 * axesCoronal = vtkMatrix4x4::New();

	 
	 
	

	double step = 0.01;//1.0 / sliceCount[2];

	normalAxial[0] = nAxial[0];
	normalAxial[1] = nAxial[1];
	normalAxial[2] = nAxial[2];

	testNormal[0]  = nAxial[0];
	testNormal[1] = nAxial[1];
	testNormal[2] = nAxial[2];

	/*
	normalAxial[1] -= step*40;// 0.01;
	normalAxial[2] -= step*40;// 0.01;
	*/

	testNormal[0] -= step * 50;
	testNormal[1] -= step * 50;
	//vtkMath::Normalize(testNormal);
	testNormal[2] = sqrt(1.0 - testNormal[0] * testNormal[0] - testNormal[1] * testNormal[1]);
	double n = vtkMath::Norm(testNormal);
	//psAxial->SetNormal(normalAxial);
	psAxial->SetNormal(testNormal);
	planeWidget[2]->UpdatePlacement();

	riw[2]->SetResliceAxes(planeWidget[2]->GetResliceAxes());
	riw[2]->Update();

	views[3]->GetRenderWindow()->Render();
	views[2]->GetRenderWindow()->Render();

	ShowAxial(psAxial);

	double angle_y = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(testNormal, nAxial));
	yDegree->setText(QString::number(angle_y, 'f', 3));
	QApplication::processEvents();
	/*
	for (int i = 0; i < 99; i++)
	{
		
		testNormal[0] = nAxial[0];
		testNormal[0] -= step * 50;
	*/	
		for (int j = 0; j < 99; j++)
		{
			

			testNormal[2] = sqrt(1.0 - testNormal[0] * testNormal[0] - testNormal[1] * testNormal[1]);
			
			

			//vtkMath::Normalize(testNormal);
			n = vtkMath::Norm(testNormal);


			psAxial->SetNormal(testNormal);
			planeWidget[2]->UpdatePlacement();

			

			riw[2]->SetResliceAxes(planeWidget[2]->GetResliceAxes());
			
			riw[2]->SetOutputOrigin(planeWidget[2]->GetReslice()->GetOutputOrigin());
			riw[2]->Update();
			vtkImageData * tmp = riw[2]->GetOutput();
			vtkImageReslice * res = planeWidget[2]->GetReslice();
			vtkImageData * tmp1 = planeWidget[2]->GetReslice()->GetOutput();
			views[2]->GetRenderWindow()->Render();
			views[3]->GetRenderWindow()->Render();
			ShowAxial(psAxial);
			angle_y = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(testNormal, nAxial));
			yDegree->setText(QString::number(angle_y, 'f', 3));

			testNormal[0] += step;

			QApplication::processEvents();
		}

	/*	testNormal[1] += step;
	}

	planeWidget[2]->SetSliceIndex(axialIndex);
	SetAxialCharacteristics(psAxial);

	riw[2]->SetResliceAxes(planeWidget[2]->GetResliceAxes());
	riw[2]->Update();
	views[2]->GetRenderWindow()->Render();
	views[3]->GetRenderWindow()->Render();
	ShowAxial(psAxial);
	
	psAxial->GetNormal(normalAxial);
	angle_y = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(normalAxial, nAxial));
	yDegree->setText(QString::number(angle_y, 'f', 3));
	*/
}

void vtk3DViewerClass::UpdateAxialNormal(double normal[3])
{


	vtkPlaneSource *psAxial = static_cast<vtkPlaneSource *>(planeWidget[2]->GetPolyDataAlgorithm());

	GetAxialCharacteristics(psAxial);

	
	psAxial->SetNormal(normal);
	planeWidget[2]->UpdatePlacement();

	riw[2]->SetResliceAxes(planeWidget[2]->GetResliceAxes());
	riw[2]->Update();

	ShowAxial(psAxial);

	double angle_y = vtkMath::DegreesFromRadians(vtkMath::AngleBetweenVectors(axialnormal, normal));
	yDegree->setText(QString::number(angle_y, 'f', 3));


	views[3]->GetRenderWindow()->Render();
	views[2]->GetRenderWindow()->Render();
	QApplication::processEvents();


}






void vtk3DViewerClass::setPlaneWidgitVisibility(bool visibility, int plane)
{
	planeWidget[plane]->SetTextureVisibility(visibility);
	planeWidget[plane]->GetPlaneProperty()->SetOpacity((visibility)? 1: 0);
}

void vtk3DViewerClass::ShowColorWindowLevel(vtkImageActor * actor)
{
	double window, level;

	double colorWindow = actor->GetProperty()->GetColorWindow();
	double colorLevel = actor->GetProperty()->GetColorLevel();

	
	vtkWindowLevelLookupTable * lut = static_cast<vtkWindowLevelLookupTable *>(sliceMaptocolors[0]->GetLookupTable());
	

	window = lut->GetWindow();
	window = sliceLut[0]->GetWindow();
	level = sliceLut[0]->GetLevel();

	windowlevel[0]->setText(QString::number(colorWindow, 'f', 3));
	windowlevel[1]->setText(QString::number(colorLevel, 'f', 3));
}

void vtk3DViewerClass::ShowColorWindowLevel(vtkImagePlaneWidget * planeWidget)
{
	double WindowLevel[2];
	planeWidget->GetWindowLevel(WindowLevel);
	windowlevel[0]->setText(QString::number(WindowLevel[0], 'f', 3));
	windowlevel[1]->setText(QString::number(WindowLevel[1], 'f', 3));
}

void vtk3DViewerClass::SetColorWindowLevel(int planeIndex, int direction)
{
	double WindowLevel[2];
	planeWidget[planeIndex]->GetWindowLevel(WindowLevel);
	if (direction < 0) {
		WindowLevel[0] -=10;
		WindowLevel[1] = WindowLevel[0] / 2;
	}
	else
	{
		WindowLevel[0] +=10;
		WindowLevel[1] = WindowLevel[0] / 2;
	}
	
	
	planeWidget[planeIndex]->SetWindowLevel(WindowLevel[0], WindowLevel[1]);
	planeWidget[planeIndex]->GetWindowLevel(WindowLevel);

	sliceLut[planeIndex]->SetWindow(WindowLevel[0]);
	sliceLut[planeIndex]->SetLevel(WindowLevel[1]);

	windowlevel[0]->setText(QString::number(WindowLevel[0], 'f', 3));
	windowlevel[1]->setText(QString::number(WindowLevel[1], 'f', 3));

	views[planeIndex]->GetRenderWindow()->Render();
	

}

void vtk3DViewerClass::ShowSaggital(vtkPlaneSource *planeSource)
{
	double point2[3];
	double point1[3];
	double center[3];
	double origin[3];
	double normal[3];

	planeSource->GetOrigin(origin);
	planeSource->GetCenter(center);
	planeSource->GetNormal(normal);
	planeSource->GetPoint1(point1);
	planeSource->GetPoint2(point2);

	saggital[0]->setText(QString::number(origin[0], 'f', 3));
	saggital[1]->setText(QString::number(origin[1], 'f', 3));
	saggital[2]->setText(QString::number(origin[2], 'f', 3));
	saggital[3]->setText(QString::number(point2[0], 'f', 3));
	saggital[4]->setText(QString::number(point2[1], 'f', 3));
	saggital[5]->setText(QString::number(point2[2], 'f', 3));
	saggital[6]->setText(QString::number(point1[0], 'f', 3));
	saggital[7]->setText(QString::number(point1[1], 'f', 3));
	saggital[8]->setText(QString::number(point1[2], 'f', 3));
	saggital[9]->setText(QString::number(normal[0], 'f', 3));
	saggital[10]->setText(QString::number(normal[1], 'f', 3));
	saggital[11]->setText(QString::number(normal[2], 'f', 3));
	saggital[12]->setText(QString::number(center[0], 'f', 3));
	saggital[13]->setText(QString::number(center[1], 'f', 3));
	saggital[14]->setText(QString::number(center[2], 'f', 3));
}

void vtk3DViewerClass::ShowCoronal(vtkPlaneSource *planeSource)
{
	double point2[3];
	double point1[3];
	double center[3];
	double origin[3];
	double normal[3];

	planeSource->GetOrigin(origin);
	planeSource->GetCenter(center);
	planeSource->GetNormal(normal);
	planeSource->GetPoint1(point1);
	planeSource->GetPoint2(point2);

	coronal[0]->setText(QString::number(origin[0], 'f', 3));
	coronal[1]->setText(QString::number(origin[1], 'f', 3));
	coronal[2]->setText(QString::number(origin[2], 'f', 3));
	coronal[3]->setText(QString::number(point2[0], 'f', 3));
	coronal[4]->setText(QString::number(point2[1], 'f', 3));
	coronal[5]->setText(QString::number(point2[2], 'f', 3));
	coronal[6]->setText(QString::number(point1[0], 'f', 3));
	coronal[7]->setText(QString::number(point1[1], 'f', 3));
	coronal[8]->setText(QString::number(point1[2], 'f', 3));
	coronal[9]->setText(QString::number(normal[0], 'f', 3));
	coronal[10]->setText(QString::number(normal[1], 'f', 3));
	coronal[11]->setText(QString::number(normal[2], 'f', 3));
	coronal[12]->setText(QString::number(center[0], 'f', 3));
	coronal[13]->setText(QString::number(center[1], 'f', 3));
	coronal[14]->setText(QString::number(center[2], 'f', 3));
}

void vtk3DViewerClass::ShowAxial(vtkPlaneSource *planeSource)
{
	double point2[3];
	double point1[3];
	double center[3];
	double origin[3];
	double normal[3];

	planeSource->GetOrigin(origin);
	planeSource->GetCenter(center);
	planeSource->GetNormal(normal);
	planeSource->GetPoint1(point1);
	planeSource->GetPoint2(point2);

	axial[0]->setText(QString::number(origin[0], 'f', 3));
	axial[1]->setText(QString::number( origin[1], 'f', 3));
	axial[2]->setText(QString::number(origin[2], 'f', 3));
	axial[3]->setText(QString::number(point2[0], 'f', 3));
	axial[4]->setText(QString::number(point2[1], 'f', 3));
	axial[5]->setText(QString::number(point2[2], 'f', 3));
	axial[6]->setText(QString::number(point1[0], 'f', 3));
	axial[7]->setText(QString::number(point1[1], 'f', 3));
	axial[8]->setText(QString::number(point1[2], 'f', 3));
	axial[9]->setText(QString::number(normal[0], 'f', 3));
	axial[10]->setText(QString::number(normal[1], 'f', 3));
	axial[11]->setText(QString::number(normal[2], 'f', 3));
	axial[12]->setText(QString::number(center[0], 'f', 3));
	axial[13]->setText(QString::number(center[1], 'f', 3));
	axial[14]->setText(QString::number(center[2], 'f', 3));
	slicePos->setText(QString::number(planeWidget[2]->GetSlicePosition(), 'f', 3));
	sliceIndex->setText(QString::number(planeWidget[2]->GetSliceIndex(), 'f', 3));
}
void vtk3DViewerClass::GetAxialCharacteristics(vtkPlaneSource * planeSource)
{
	axialIndex = planeWidget[2]->GetSliceIndex();
	planeSource->GetOrigin(axialorigin);
	planeSource->GetCenter(axialcenter);
	planeSource->GetNormal(axialnormal);
	planeSource->GetPoint1(axialpoint1);
	planeSource->GetPoint2(axialpoint2);
}
void vtk3DViewerClass::SetAxialCharacteristics(vtkPlaneSource * planeSource)
{
	
	planeSource->SetOrigin(axialorigin);
	planeSource->SetCenter(axialcenter);
	planeSource->SetNormal(axialnormal);
	planeSource->SetPoint1(axialpoint1);
	planeSource->SetPoint2(axialpoint2);
	planeWidget[2]->UpdatePlacement();
}

void vtk3DViewerClass::createCT()
{

	
	colorFun->RemoveAllPoints();
	opacityFun->RemoveAllPoints();
	gradientOpacity->RemoveAllPoints();

	string name = vpSetting["name"];
	string  gradient_opacity_value = vpSetting["gradientOpacity"];
	std::istringstream g_iss(gradient_opacity_value);
	std::vector<std::string> gov_results(std::istream_iterator<std::string>{g_iss},
		std::istream_iterator<std::string>());
	
	string  scalar_opacity_value = vpSetting["scalarOpacity"];
	std::istringstream sov_iss(scalar_opacity_value);
	std::vector<std::string> sov_results(std::istream_iterator<std::string>{sov_iss},
		std::istream_iterator<std::string>());

	string  color_transfer_value = vpSetting["colorTransfer"];
	std::istringstream c_iss(color_transfer_value);
	std::vector<std::string> ctv_results(std::istream_iterator<std::string>{c_iss},
		std::istream_iterator<std::string>());


	colorFun->SetAllowDuplicateScalars(true);
	for (int point = 1; point < ctv_results.size(); point += 4)
	{
		colorFun->AddRGBPoint(atof(ctv_results[point].c_str()),
			atof(ctv_results[point + 1].c_str()),
			atof(ctv_results[point + 2].c_str()),
			atof(ctv_results[point + 3].c_str())
		);
	}

	opacityFun->SetAllowDuplicateScalars(true);
	for (int point = 1; point < sov_results.size(); point += 2)
	{
		opacityFun->AddPoint(
			atof(sov_results[point].c_str()),
			atof(sov_results[point + 1].c_str())
		);
	}

	gradientOpacity->SetAllowDuplicateScalars(true);
	for (int point = 1; point < gov_results.size(); point += 2)
	{
		gradientOpacity->AddPoint(
			atof(gov_results[point].c_str()),
			atof(gov_results[point + 1].c_str())
		);
	}

	mapper->SetBlendModeToComposite();

	int shade = atoi(vpSetting["shade"].c_str());

	float am = atof(vpSetting["ambient"].c_str());

	volume3Property->SetShade(atoi(vpSetting["shade"].c_str()));
	volume3Property->SetAmbient(atof(vpSetting["ambient"].c_str()));
	volume3Property->SetDiffuse(atof(vpSetting["diffuse"].c_str()));
	volume3Property->SetSpecular(atof(vpSetting["specular"].c_str()));
	volume3Property->SetSpecularPower(atof(vpSetting["specularPower"].c_str()));
	volume3Property->SetInterpolationType(atoi(vpSetting["interpolation"].c_str()));

	/*
	for (const ColorPoint* point : ctSetting->colorPoints)
	{
		colorFun->AddRGBPoint(point->x, point->red, point->green, point->blue, point->midpoint, point->sharpness);
		
	}
	
	for (const OpacityPoint* point : ctSetting->opacityPoints) {
		opacityFun->AddPoint(point->x, point->y, point->midpoint, point->sharpness);
	}

	for (const OpacityPoint* point : ctSetting->gradientPoints) {
		gradientOpacity->AddPoint(point->x, point->y, point->midpoint, point->sharpness);
		
	}
	
	
	mapper->SetBlendMode(ctSetting->BlendMode);
	volume3Property->SetShade(ctSetting->Shade);
	volume3Property->SetAmbient(ctSetting->Ambient);
	volume3Property->SetDiffuse(ctSetting->Diffuse);
	volume3Property->SetSpecular(ctSetting->Specular);
	volume3Property->SetSpecularPower(ctSetting->SpecularPower);
	volume3Property->SetScalarOpacityUnitDistance(ctSetting->ScalarOpacityUnitDistance);

	*/
}






