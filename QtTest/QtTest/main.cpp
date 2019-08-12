
#include "vtkAutoInit.h" 

VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)

#include <QApplication>
#include <QSurfaceFormat>

//#define vtkRenderingCore_AUTOINIT 3(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingOpenGL)
//#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL2)
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)


#include "MainWindow.h"
//#include <QtWidgets/QApplication>
//#include <QSurfaceFormat>



int main(int argc, char *argv[])
{
	QSurfaceFormat fmt = QVTKOpenGLWidget::defaultFormat();
	fmt.setSamples(0);
	QSurfaceFormat::setDefaultFormat(fmt);

	
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
