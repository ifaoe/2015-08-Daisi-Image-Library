/*
 * MainWindow.h
 *
 *  Created on: Aug 7, 2015
 *      Author: awg
 */

#ifndef SRC_MAINWINDOW_H_
#define SRC_MAINWINDOW_H_

#include <QMainWindow>
#include <QItemSelection>
#include <QAbstractButton>
#include <QPlainTextEdit>
#include <QToolButton>
#include <QLayout>

#include "DatabaseHandler.h"
#include "QtExtension/QSqlReadOnlyTableModel.h"
#include "ImageCanvas.h"
#include "UserSettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(UserSettings * config, DatabaseHandler * db);
    ~MainWindow();
private:
	Ui::MainWindow * ui;
	UserSettings * config;
	DatabaseHandler * db;
	ImageCanvas * canvas;
	QVBoxLayout * image_canvas_layout;

	QSqlReadOnlyTableModel * census_model = 0;
	QMap<QString, QString> filter_map;
	QMap<QString, int> census_index_map;
	QMap<QString, bool> enabled_columns_map;
	QMap<QString,QComboBox*> filter_combo_map;
	QMap<QString,QPlainTextEdit*> filter_text_map;

	QString current_session;
	QString current_cam;
	QString current_img;

	QMap<QString, QComboBox*> filter_boxes;

	void HandleFilter();
	void ClearFilter();
	void ResizeHeader();
	void SetDatabaseModels();
	void SetupTables();
	void RefreshHeader();
	void RefreshColumnMap();
private slots:
	void HandleColumnVisibility();
	void HandleColumnChooser();
	void HandleTypeFilter(QAbstractButton * button);
	void HandleSelectionChange(const QItemSelection & selected, const QItemSelection & deselected);
	void HandleComboFilter(int index);
	void HandleSessionSelection();
	void HandleServerSelection();
	void HandleImagePopup();
	void HandleHeaderChange();
};

#endif /* SRC_MAINWINDOW_H_ */
