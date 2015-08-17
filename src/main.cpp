#include <QtGui/QApplication>
#include <QLabel>
#include <qgsapplication.h>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("ifaoe");
	QCoreApplication::setOrganizationDomain("ifaoe.de");
	QCoreApplication::setApplicationName("daisi-image-library");
    QApplication a(argc, argv);

    QgsApplication::setPrefixPath("/usr", true);
    QgsApplication::initQgis();
    UserSettings * config = new UserSettings;
    config->InitSettings();
    DatabaseHandler * db = new DatabaseHandler(config);
    MainWindow main_window(config, db);
    if (config->getAppMaximized())
    	main_window.showMaximized();
    else
    	main_window.show();
    return a.exec();
}
