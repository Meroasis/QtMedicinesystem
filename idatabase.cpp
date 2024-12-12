#include "idatabase.h"

void IDatabase::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString aFile ="../../lib.db";
    database.setDatabaseName(aFile);

    if(!database.open()){ //打开数据库
        qDebug()<<"failed to open database";
    }
    else
        qDebug()<<"open database is ok";
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    initDatabase();


}
