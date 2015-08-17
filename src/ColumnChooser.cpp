/*
 * ColumnChooser.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: awg
 */

#include "ColumnChooser.h"
#include <QDebug>

ColumnChooser::ColumnChooser(UserSettings * config, QWidget * parent)
	: QDialog(parent), ui(new Ui::ColumnOptions), config(config) {
	ui->setupUi(this);
	foreach (QAbstractButton * button, ui->buttongroup_column_boxes->buttons()) {
		if (config->getVisibleColumns().contains(button->property("dbvalue").toString()))
			button->setChecked(true);
		else
			button->setChecked(false);

	}
	connect(this, SIGNAL(accepted()), this, SLOT(OnAccept()));
}

ColumnChooser::~ColumnChooser() {
	// TODO Auto-generated destructor stub
}

void ColumnChooser::OnAccept() {
	QStringList columns;
	QList<QAbstractButton*> buttons = ui->buttongroup_column_boxes->buttons();
	for (int i=0; i<buttons.size(); i++) {
		if (buttons[i]->isChecked())
			columns.append(buttons[i]->property("dbvalue").toString());
	}
	config->setVisibleColumns(columns);
	config->sync();
}
