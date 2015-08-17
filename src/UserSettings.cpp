/*
 * UserSettings.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: awg
 */

#include "UserSettings.h"
#include <QFile>
#include <QDebug>

DatabaseInfo UserSettings::getDatabaseInfo(const QString & id) {
	DatabaseInfo info;
	beginGroup("Database");
	if (!childGroups().contains(id)) {
		return info;
		endGroup();
	}
		beginGroup(id);
		info.id = id;
		info.host = value("host",QString("localhost")).toString();
		info.port = value("port", 5432).toInt();
		info.name = value("name", "daisi").toString();
		info.password = value("password", "18ifaoe184").toString();
		info.user = value("user","daisi").toString();
		endGroup();
	endGroup();
	return info;
}

void UserSettings::AddDatabase(const QString & id, const QString & host, int port, const QString & name,const QString & user,
			const QString & password) {
	beginGroup("Database");
	{
		beginGroup(id);
		setValue("host",QString(host));
		setValue("port", port);
		setValue("name", name);
		setValue("password", password);
		setValue("user",user);
		endGroup();
	}
	endGroup();
}

void UserSettings::InitSettings() {
	if (!QFile::exists(fileName())) {
		AddDatabase("Rostock", "192.168.118.35", 5432, "daisi", "daisi","18ifaoe184");
		AddDatabase("Hamburg", "192.168.200.35", 5432, "daisi", "daisi","18ifaoe184");
	}
}

/*
 * get and set
 */

void UserSettings::setVisibleColumns(QStringList columns) {
	setValue("Census/columns_visible", columns);
}

QStringList UserSettings::getVisibleColumns() {
	QStringList default_columns = QStringList() << "rcns_id" << "name" <<"id_code" << "confidence" <<"beh" << "age" <<
			"age_year" << "gen" << "plumage" << "dir" << "imgqual" << "length" << "width" << "stuk4_ass" << "stuk4_beh";
	return value("Census/columns_visible", default_columns).toStringList();
}

void UserSettings::setPreferredDatabase(QString database) {
	setValue("Database/preferred", database);
}

QString UserSettings::getPreferredDatabase() {
	return value("Database/preferred","").toString();
}

void UserSettings::setAppSize(QSize size) {
	setValue("MainWindow/size",size);
}

QSize UserSettings::getAppSize() {
	return value("MainWindow/size",QSize(800,600)).toSize();
}

void UserSettings::setAppPosition(QPoint pos) {
	setValue("MainWindow/position",pos);
}

QPoint UserSettings::getAppPosition() {
	return value("MainWindow/position",QPoint(50,50)).toPoint();
}

void UserSettings::setAppMaximized(bool max) {
	setValue("MainWindow/maximized",max);
}

bool UserSettings::getAppMaximized() {
	return value("MainWindow/maximized",false).toBool();
}

QStringList UserSettings::getDatabaseList() {
	beginGroup("Database");
	QStringList children = childGroups();
	endGroup();
	return children;
}
