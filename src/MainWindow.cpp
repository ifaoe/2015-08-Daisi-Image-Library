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

	type_tab_bar = new QTabBar(ui->widget_types);
	type_tab_bar->setExpanding(true);
	type_tab_bar->setShape(QTabBar::RoundedNorth);
	type_tab_bar->setCurrentIndex(-1);
//	type_tab_bar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
//	type_tab_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	type_tab_bar->addTab(QString::fromUtf8("Alle"));
	type_tab_bar->addTab(QString::fromUtf8("Vögel"));
	type_tab_bar->addTab(QString::fromUtf8("Meeressäuger"));
	type_tab_bar->addTab(QString::fromUtf8("Anthropogenes"));
	type_tab_bar->addTab(QString::fromUtf8("Sonstiges"));

	type_tab_bar->setTabData(0,"%");
	type_tab_bar->setTabData(1,"BIRD");
	type_tab_bar->setTabData(2,"MAMMAL");
	type_tab_bar->setTabData(3,"ANTHRO");
	type_tab_bar->setTabData(4,"MISC");

	ui->widget_types->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	ui->widget_types->setFixedHeight(type_tab_bar->height());
	filter_map["standard"] = "confidence<4 AND censor=2 AND tp!='NOSIGHT'";

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
	connect(type_tab_bar, SIGNAL(currentChanged(int)), this, SLOT(HandleTypeFilter(int)));
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

	if (selected.isEmpty()) {
		canvas->UnloadObject();
		return;
	}

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
	int column;
	for (int i=0; i<census_index_map.keys().size();i++) {
		column = census_index_map[census_index_map.keys().at(i)];
		if (!config->getVisibleColumns().contains(census_index_map.keys().at(i))) {
			ui->table_view_objects->hideColumn(column);
			ui->table_header_filters->hideColumn(column);
		} else {
			ui->table_view_objects->showColumn(column);
			ui->table_header_filters->showColumn(column);
		}
	}
}

void MainWindow::HandleColumnChooser() {
	ColumnChooser dialog(config);
	dialog.exec();
	HandleColumnVisibility();
}

void MainWindow::HandleTypeFilter(int index){
	if (!db->GetDatabase()->isOpen()) return;

	QString type_filter = QString("tp like '%1'").arg(type_tab_bar->tabData(index).toString());
	int min_width = 0;
	QStringList::iterator i;
	foreach (QString column, config->getVisibleColumns()) {
		filter_boxes[column]->disconnect();
		db->GetFilterOptions(filter_boxes[column],type_tab_bar->tabData(index).toString());
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
	if (!check)
		return;
	if (project.isEmpty())
		filter_map.remove("session");
	else
		filter_map["session"] = QString("session='%1'").arg(project);

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
