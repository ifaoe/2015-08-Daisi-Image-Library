/*
 * UserSettings.h
 *
 *  Created on: Aug 12, 2015
 *      Author: awg
 */

#ifndef SRC_USERSETTINGS_H_
#define SRC_USERSETTINGS_H_

#include <QSettings>
#include <QStringList>
#include <QPoint>
#include <QSize>

struct DatabaseInfo {
	QString id = "local";
	QString host = "localhost";
	int port = 5432;
	QString name = "daisi";
	QString password = "18ifaoe184";
	QString user = "daisi";
};

class UserSettings : public QSettings{
public:
	using QSettings::QSettings;
	void InitSettings();
	void AddDatabase(const QString & id, const QString & host, int port, const QString & name,const QString & user,
			const QString & password);

	void setVisibleColumns(QStringList columns);
	QStringList getVisibleColumns();
	void setAppPosition(QPoint pos);
	QPoint getAppPosition();
	void setAppSize(QSize size);
	QSize getAppSize();
	void setAppMaximized(bool max);
	bool getAppMaximized();
	void setPreferredDatabase(QString database);
	QString getPreferredDatabase();
	QStringList getDatabaseList();

	DatabaseInfo getDatabaseInfo(const QString & id);


};

#endif /* SRC_USERSETTINGS_H_ */
