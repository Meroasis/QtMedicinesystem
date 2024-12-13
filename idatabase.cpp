#include "idatabase.h"
#include <QUuid>
void IDatabase::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString aFile ="../../lab.db";
    database.setDatabaseName(aFile);

    if(!database.open()){ //打开数据库
        qDebug()<<"failed to open database";
    }
    else
        qDebug()<<"open database is ok";
}

QString IDatabase::userLogin(QString userName, QString Mypassword)
{
    QSqlQuery query;
    query.prepare("select username,password from user where username = :USER");
    query.bindValue(":USER",userName);
    query.exec();

    if(query.first()&&query.value("username").isValid()){
        QString password = query.value("password").toString();
        if(Mypassword == password){
            return "login_Ok";
        }
        else{
             qDebug() << "wrong_password";
            return "wrong_password";

        }
    }else{
        qDebug() << "no such user";
        return "wrong_username";

    }

    return "login_fault";
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    initDatabase();


}

bool IDatabase::initPatientModel()
{
    patientTabModel = new QSqlTableModel(this,database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(
        QSqlTableModel::OnManualSubmit);//数据保存方式
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder);//排序
    if(!(patientTabModel->select()))//查询数据
        return false;
    thePatientSelection = new QItemSelectionModel(patientTabModel);
    return true;
}

bool IDatabase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex();
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();

}

bool IDatabase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void IDatabase::reverPatientEidt()
{
    patientTabModel->revertAll();
}

int IDatabase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),
                                QModelIndex());//在末尾添加一个记录
    QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1,1);//创建最后一行的ModelIndex
    int curRecNo =curIndex.row();
    QSqlRecord curRec=patientTabModel->record(curRecNo);//获取当前记录
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));
    patientTabModel->setRecord(curRecNo,curRec);
    return curIndex.row();
}
