/*
 * ImagePopup.cpp
 *
 *  Created on: Aug 11, 2015
 *      Author: awg
 */

#include "ImagePopup.h"

ImagePopup::ImagePopup(QWidget * canvas, ImageCanvas * image_canvas, QWidget * parent) : QWidget(parent, Qt::Window),
	ui(new Ui::widget_image_popup), widget_canvas(canvas), image_canvas(image_canvas), parent(parent) {
	ui->setupUi(this);
	ui->widget_image->setLayout(canvas->layout());

	connect(ui->button_quit, SIGNAL(clicked()),this, SLOT(close()));
	connect(ui->button_zoom_in, SIGNAL(clicked()), this, SLOT(ZoomIn()));
	connect(ui->button_zoom_out, SIGNAL(clicked()), this, SLOT(ZoomOut()));
	connect(ui->button_zoom_fit_best, SIGNAL(clicked()), this, SLOT(ZoomToFullExtent()));
	connect(ui->button_zoom_original, SIGNAL(clicked()), this, SLOT(ZoomOriginal()));
	connect(ui->button_save,SIGNAL(clicked()),image_canvas,SLOT(SaveImage()));

	map_pan_tool = new QgsMapToolPan(image_canvas);
	image_canvas->setMapTool(map_pan_tool);
}

ImagePopup::~ImagePopup() {
	image_canvas->unsetMapTool(map_pan_tool);
	delete map_pan_tool;
}

void ImagePopup::close() {
	widget_canvas->setLayout(ui->widget_image->layout());
	QWidget::close();
}
