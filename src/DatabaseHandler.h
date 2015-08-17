/*
 * DatabaseHandler.h
 *
 *  Created on: Jul 22, 2015
 *      Author: awg
 */

#ifndef DATABASEHANDLER_H_
#define DATABASEHANDLER_H_

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QComboBox>
#include "UserSettings.h"

class DatabaseHandler {
public:
	DatabaseHandler(UserSettings * cfg);
	virtual ~DatabaseHandler();
	bool OpenDatabase();
	QSqlDatabase * GetDatabase() { return db; }
	QString GetImageLocation(const QString & session, const QString & cam, const QString & img);
	void GetFilterOptions(QComboBox * box, const QString & type);
	QStringList GetSessionList();
private:
	UserSettings * cfg = 0;
	QSqlDatabase * db = 0;
};

#endif /* DATABASEHANDLER_H_ */
