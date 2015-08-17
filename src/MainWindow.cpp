/*
 * MainWindow.cpp
 *
 *  Created on: Aug 7, 2015
 *      Author: awg
 */

#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "ImagePopup.h"
#include "ColumnChooser.h"
#include <QSqlError>
#include <QDebug>
#include <QScrollBar>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(UserSettings * config, DatabaseHandler * db)
	: QMainWindow(0), ui(new Ui::MainWindow), config(config), db(db){
	// TODO Auto-generated constructor stub
	ui->setupUi(this);

	filter_map["standard"] = "censor=2 AND tp!='NOSIGHT'";

	ui->frame_types->setMaximumHeight(ui->button_type_all->height());

	if (!db->OpenDatabase())
		HandleServerSelection();

	SetDatabaseModels();
	ui->table_view_objects->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->table_view_objects->setSelectionMode(QAbstractItemView::SingleSelection);

	QHeaderView * header = ui->table_view_objects->horizontalHeader();
		ui->table_header_filters->setHorizontalHeader(header);
		header->setVisible(true);
		ui->table_view_objects->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->table_view_objects));
		ui->table_view_objects->horizontalHeader()->setVisible(false);
		connect(ui->table_view_objects->horizontalScrollBar(), SIGNAL(valueChanged(int)),
				ui->table_header_filters->horizontalScrollBar(), SLOT(setValue(int)));
		connect(header, SIGNAL(sectionResized(int, int, int)),this, SLOT(HandleHeaderChange()));


    image_canvas_layout = new QVBoxLayout;
    image_canvas_layout->setMargin(0);
	canvas = new ImageCanvas(db, ui->widget_image);
	image_canvas_layout->addWidget(canvas);
	ui->widget_image->setLayout(image_canvas_layout);

	SetupTables();
	HandleColumnVisibility();

	connect(ui->table_view_objects->selectionModel(),
			SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this,
			SLOT(HandleSelectionChange(const QItemSelection &, const QItemSelection &)));
	connect(ui->actionSession_w_hlen, SIGNAL(triggered()),this, SLOT(HandleSessionSelection()));
	connect(ui->actionMit_Server_verbinden,SIGNAL(triggered()),this,SLOT(HandleServerSelection()));
	connect(ui->button_image_popup, SIGNAL(clicked()),this,SLOT(HandleImagePopup()));
	connect(ui->actionSpalten,SIGNAL(triggered()),this,SLOT(HandleColumnChooser()));
	connect(ui->button_save_image,SIGNAL(clicked()),canvas, SLOT(SaveImage()));
}

MainWindow::~MainWindow() {
	config->setAppPosition(pos());
	config->setAppSize(size());
	config->setAppMaximized(isMaximized());
	config->sync();
}

void MainWindow::HandleFilter() {
	census_model->setFilter( static_cast<QStringList>(filter_map.values()).join(" AND ") );
	census_model->select();
}

void MainWindow::SetupTables() {
	ui->table_header_filters->setColumnCount(census_model->columnCount());
	ui->table_header_filters->setRowCount(1);
	QString column_name;
	QComboBox * box;
	for (int i=0; i<census_model->columnCount();i++) {
		column_name = census_model->headerData(i,Qt::Horizontal).toString();
		census_index_map[column_name] = census_model->fieldIndex(column_name);
		box = new QComboBox(ui->table_header_filters);
		box->setEditable(true);
		box->setProperty("column", column_name);
		box->addItem("",QString("%"));
		filter_boxes[column_name] = box;
		ui->table_header_filters->setCellWidget(0,i,box);
	}
	ui->table_header_filters->setFixedHeight(ui->table_header_filters->horizontalHeader()->height());
	connect(ui->button_group_types, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(HandleTypeFilter(QAbstractButton*)));
}

void MainWindow::RefreshColumnMap() {
	census_index_map.clear();
	QString column_name;
	for (int i=0; i<census_model->columnCount();i++) {
		column_name = census_model->headerData(i,Qt::Horizontal).toString();
		census_index_map[column_name] = census_model->fieldIndex(column_name);
	}
}

void MainWindow::RefreshHeader() {
	for (int i=0; i<census_model->columnCount();i++)
		ui->table_header_filters->setCellWidget(0,i,filter_boxes[census_model->headerData(i,Qt::Horizontal).toString()]);


}

void MainWindow::ClearFilter() {
	if (filter_boxes.isEmpty())
		return;
	foreach (QString column, config->getVisibleColumns()) {
		filter_map.remove(column);
		filter_boxes[column]->setCurrentIndex(0);
	}
}

void MainWindow::SetDatabaseModels() {
	if (census_model != 0) {
		delete census_model;
		census_model = 0;
	}
	census_model = new QSqlReadOnlyTableModel(this, db->GetDatabase());
	census_model->setTable("view_census");
	ui->table_view_objects->setModel(census_model);
}

/*
 * SIGNALS
 */

void MainWindow::HandleHeaderChange() {
	ui->table_view_objects->horizontalHeader()->restoreGeometry(
			ui->table_header_filters->horizontalHeader()->saveGeometry());
	ui->table_view_objects->horizontalHeader()->restoreState(
			ui->table_header_filters->horizontalHeader()->saveState());
}

void MainWindow::HandleSelectionChange(const QItemSelection & selected, const QItemSelection & deselected){
	Q_UNUSED(deselected);
	ui->label_session_text->clear();
	ui->label_cam_text->clear();
	ui->label_image_text->clear();
	ui->label_utm_text->clear();
	ui->label_geo_text->clear();
	ui->label_usr_text->clear();
	ui->label_pre_user_text->clear();
	ui->plain_text_remark->clear();

	current_session.clear();
	current_cam.clear();
	current_img.clear();

	if (selected.isEmpty())
		return;
	int row = selected.indexes().at(0).row();
	current_session = census_model->data(census_model->index(row, census_index_map["session"])).toString();
	current_cam = census_model->data(census_model->index(row, census_index_map["cam"])).toString();
	current_img = census_model->data(census_model->index(row, census_index_map["img"])).toString();

	ui->label_session_text->setText(current_session);
	ui->label_cam_text->setText(current_cam);
	ui->label_image_text->setText(current_img);
	double ux = census_model->data(census_model->index(row, census_index_map["ux"])).toDouble();
	double uy = census_model->data(census_model->index(row, census_index_map["uy"])).toDouble();
	ui->label_utm_text->setText(QString("%1,\t%2").arg(ux).arg(uy));
	double lx = census_model->data(census_model->index(row, census_index_map["lx"])).toDouble();
	double ly = census_model->data(census_model->index(row, census_index_map["ly"])).toDouble();
	ui->label_geo_text->setText(QString("%1,\t%2").arg(lx).arg(ly));
	ui->label_usr_text->setText(census_model->data(census_model->index(row, census_index_map["usr"])).toString());
	ui->label_pre_user_text->setText(census_model->data(census_model->index(row, census_index_map["pre_usr"])).toString());
	ui->plain_text_remark->setPlainText(census_model->data(census_model->index(row, census_index_map["rem"])).toString());

	if (!canvas->LoadObject(current_session, current_cam, current_img,ux,uy)) {
		QMessageBox messageBox;
		messageBox.critical(0,"Fehler","Bild konnte nicht geladen werden.");
	}
}

void MainWindow::HandleColumnVisibility() {
	ClearFilter();
	if (ui->button_group_types->checkedButton() != 0)
		ui->button_group_types->checkedButton()->setChecked(false);
	int column;
	for (int i=0; i<census_index_map.keys().size();i++) {
		if (!config->getVisibleColumns().contains(census_index_map.keys().at(i))) {
			column = census_index_map[census_index_map.keys().at(i)];
			ui->table_view_objects->hideColumn(column);
			ui->table_header_filters->hideColumn(column);
		}
	}

//	ui->table_header_filters->resizeColumnsToContents();
	ui->table_header_filters->horizontalHeader()->restoreGeometry(
			ui->table_view_objects->horizontalHeader()->saveGeometry());
	ui->table_header_filters->horizontalHeader()->restoreState(
			ui->table_view_objects->horizontalHeader()->saveState());
}

void MainWindow::HandleColumnChooser() {
	ColumnChooser dialog(config);
	dialog.exec();
	HandleColumnVisibility();
}

void MainWindow::HandleTypeFilter(QAbstractButton * button){
	if (!db->GetDatabase()->isOpen()) return;
	if (!ui->button_group_types->exclusive())
		ui->button_group_types->setExclusive(true);

	QString type_filter = QString("tp like '%1'").arg(button->property("dbvalue").toString());
	int min_width = 0;
	QStringList::iterator i;
	foreach (QString column, config->getVisibleColumns()) {
		filter_boxes[column]->disconnect();
		db->GetFilterOptions(filter_boxes[column],button->property("dbvalue").toString());
		min_width = filter_boxes[column]->minimumSizeHint().width();
		filter_boxes[column]->setMinimumWidth(min_width);
		connect(filter_boxes[column], SIGNAL(currentIndexChanged(int)),this,SLOT(HandleComboFilter(int)));
	}
	ui->table_header_filters->resizeColumnsToContents();
	ui->table_header_filters->resizeRowsToContents();
	ui->table_header_filters->setFixedHeight(ui->table_header_filters->horizontalHeader()->height()
			+ ui->table_header_filters->rowHeight(0)+4);

	ClearFilter();
	filter_map["tp"] = type_filter;
	HandleFilter();
	HandleHeaderChange();
};

void MainWindow::HandleComboFilter(int index) {
	QComboBox * box = static_cast<QComboBox*>(sender());
	if (box->currentIndex() == 0){
		filter_map.remove(box->property("column").toString());
		HandleFilter();
		return;
	}

	//TODO: Ugly and inefficient
	filter_map[box->property("column").toString()] =
			QString("cast(%1 as text) like '%2'").arg(sender()->property("column").toString()).arg(box->itemData(index).toString());
	HandleFilter();
}

void MainWindow::HandleSessionSelection() {
	bool check;
	QString project = QInputDialog::getItem(this,tr("Flug auswählen"),tr("Flug:"),
			db->GetSessionList(),0,false,&check);
	if (check) {
		filter_map["session"] = QString("session='%1'").arg(project);
	} else {
		filter_map.remove("session");
		return;
	}
	HandleColumnVisibility();
	HandleFilter();
}

void MainWindow::HandleServerSelection() {
	bool check;
	QString database = QInputDialog::getItem(this,tr("Datenbank auswählen..."),tr("Datenbank:"),
			config->getDatabaseList(),0,false,&check);
	if (check) {
		config->setPreferredDatabase(database);
		db->OpenDatabase();
		SetDatabaseModels();
	} else {
		return;
	}
	filter_map.remove("session");
	QHeaderView * header = ui->table_view_objects->horizontalHeader();
	ui->table_header_filters->setHorizontalHeader(header);
	header->setVisible(true);
	ui->table_view_objects->setHorizontalHeader(new QHeaderView(Qt::Horizontal, ui->table_view_objects));

	connect(ui->table_view_objects->horizontalScrollBar(), SIGNAL(valueChanged(int)),
			ui->table_header_filters->horizontalScrollBar(), SLOT(setValue(int)));
	connect(header, SIGNAL(sectionResized(int, int, int)),this, SLOT(HandleHeaderChange()));

	RefreshColumnMap();
	HandleColumnVisibility();
	HandleFilter();
	HandleHeaderChange();
}

void MainWindow::HandleImagePopup() {
	ImagePopup * window = new ImagePopup(ui->widget_image, canvas, this);
	window->show();
}
