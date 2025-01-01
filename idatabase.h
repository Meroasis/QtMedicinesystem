#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <Qtsql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>

#include "doctorinfo.h"
class IDatabase : public QObject
{
    Q_OBJECT
public:
    static IDatabase& getInstance()
    {
        static IDatabase instance; // Guaranteed to be destroyed.
            // Instantiated on first use.
        return instance;
    }
    QString userLogin(QString username,QString password);
    bool initPatientModel();

    QSqlTableModel  *patientTabModel; //数据模型
    QItemSelectionModel *thePatientSelection;//选择模型
    QDataWidgetMapper  *dataMapper;//数据映射

    // bool getUserPermissionLevel(const QString &userName, int &permissionLevel);
    int getPermissionLevelByUsernamePrefix(const QString &userName);
    bool setUserPermissionLevel(const QString &userName, int permissionLevel);//0 表示普通医生。 1 表示具有管理其他医生权限的医生
    bool userRegister(const DoctorInfo &doctorInfo); // 接收 DoctorInfo 对象
    DoctorInfo getUserInfo(const QString &userName); // 返回 DoctorInfo 对象

    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void reverPatientEidt();
    int addNewPatient();

private:
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const&)  = delete;
    void operator=(IDatabase const&)  = delete;
    QSqlDatabase database;



    void initDatabase();


signals:


};

#endif // IDATABASE_H
