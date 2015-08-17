/*
 * ColumnChooser.h
 *
 *  Created on: Aug 13, 2015
 *      Author: awg
 */

#ifndef SRC_COLUMNCHOOSER_H_
#define SRC_COLUMNCHOOSER_H_

#include <qdialog.h>
#include "ui_column_options.h"
#include "UserSettings.h"

class ColumnChooser: public QDialog {
	Q_OBJECT
public:
	ColumnChooser(UserSettings * config, QWidget * parent =0);
	virtual ~ColumnChooser();
private:
	Ui::ColumnOptions * ui;
	UserSettings * config;
private slots:
	void OnAccept();
};

#endif /* SRC_COLUMNCHOOSER_H_ */
