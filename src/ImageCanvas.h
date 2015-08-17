/*
 * ImageCanvas.h
 *
 *  Created on: Aug 7, 2015
 *      Author: awg
 */

#ifndef IMAGECANVAS_H_
#define IMAGECANVAS_H_

#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <qgsmaplayerregistry.h>
#include <qgsproviderregistry.h>
#include "DatabaseHandler.h"

class ImageCanvas: public QgsMapCanvas {
	Q_OBJECT
public:
	ImageCanvas(DatabaseHandler * db, QWidget * parent);
	virtual ~ImageCanvas();
	bool LoadObject(const QString & session, const QString & cam, const QString & img, const double & ux, const double & uy);
	void CenterOnWorldPosition(const double & ux, const double & uy, const double & scale);
public slots:
	void SaveImage();
	void ShowProgress(int value) { qDebug() << "Progress: " << value; }
private:
	QWidget * parent;
	DatabaseHandler * db;
	QgsRasterLayer * image_layer = 0;
	QgsRasterDataProvider * image_provider = 0;

	QgsMapLayerRegistry * layer_registry = 0;
	QgsProviderRegistry * provider_registry = 0;

	QString current_session = "";
	QString current_cam = "";
	QString current_img = "";
protected:
	virtual int heightForWidth(int) const;
//	void resizeEvent(QResizeEvent * event);
};

#endif /* IMAGECANVAS_H_ */
