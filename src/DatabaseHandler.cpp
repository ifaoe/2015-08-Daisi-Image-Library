/*
 * DatabaseHandler.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: awg
 */

#include "DatabaseHandler.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

DatabaseHandler::DatabaseHandler(UserSettings * cfg) : cfg(cfg){
	// TODO Auto-generated constructor stub

    db = new QSqlDatabase();
	*db = QSqlDatabase::addDatabase("QPSQL");
	if (!db->isValid()) {
		qFatal("Database invalid: QPSQL");
	}
}

bool DatabaseHandler::OpenDatabase() {
	if (cfg->getPreferredDatabase().isEmpty())
		return false;

	if (db->isOpen()) {
		db->close();
	}

	DatabaseInfo info = cfg->getDatabaseInfo(cfg->getPreferredDatabase());
	if (info.id.isEmpty())
		return false;
    db->setHostName(info.host);
    db->setDatabaseName(info.name);
    db->setPort(info.port);
    db->setUserName(info.user);
    db->setPassword(info.password);
    qDebug() << "Opening Database " + db->databaseName()+ " on Host " + db->hostName() + ".";
    if (!db->open()) {
        qFatal("Could not open Database");
    }
    return true;
}

DatabaseHandler::~DatabaseHandler() {
	// TODO Auto-generated destructor stub
	db->close();
}

QString DatabaseHandler::GetImageLocation(const QString & session, const QString & cam, const QString & img) {
	QString query_string = "SELECT path FROM projects WHERE project_id='%1'";
	QSqlQuery query(query_string.arg(session));
	if (query.next())
		return 	query.value(0).toString() + QString("/cam%1/geo/%2.tif").arg(cam).arg(img);
	return QString();
}

void DatabaseHandler::GetFilterOptions(QComboBox * box, const QString & type) {
	box->clear();
	box->addItem("-",QString("%"));
	QString query_string = "SELECT DISTINCT %1 FROM view_census WHERE censor = 2 AND confidence<4 AND tp like '%2' ORDER BY %1";
	QSqlQuery query(query_string.arg(box->property("column").toString()).arg(type));
	while(query.next())
			box->addItem(query.value(0).toString(),query.value(0));
}

QStringList DatabaseHandler::GetSessionList() {
	QStringList return_list;
	return_list.append(QString());
	QString query_string = "SELECT DISTINCT session FROM raw_census ORDER BY session";
	QSqlQuery query(query_string);
	while (query.next())
		return_list.append(query.value(0).toString());
	return return_list;
}

