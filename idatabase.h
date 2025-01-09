#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <Qtsql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>
#include "patienteditview.h"

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



    int getPermissionLevelByUsernamePrefix(const QString &userName);
    bool setUserPermissionLevel(const QString &userName, int permissionLevel);//0 表示普通医生。 1 表示具有管理其他医生权限的医生
    bool userRegister(const DoctorInfo &doctorInfo); // 接收 DoctorInfo 对象
    DoctorInfo getUserInfo(const QString &userName); // 返回 DoctorInfo 对象
    //患者
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void reverPatientEidt();
    int addNewPatient();
    bool deleteCurrentPatient(QItemSelectionModel *selectionModel);


    //医生
    bool initConnection();
    QVector<DoctorInfo> getAllDoctors();
    bool addNewDoctor(const DoctorInfo &doctor);
    bool updateDoctor(const DoctorInfo &doctor);
    bool deleteDoctor(int id);
    QSqlDatabase database;


    //药品
    QSqlTableModel *medicineModel; //药品模型
    std::unique_ptr<QItemSelectionModel> theMedicineSelection;//药品选择模型
    QDataWidgetMapper  *medicineDataMapper;
    bool initMedicineModel();
    bool searchMedicine(QString filter);
    int addMedicine();
    bool deleteMedicine(QItemSelectionModel *selectionModel);
    bool submitMedicineEdit();
    void reverMedicineEidt();
    bool importMedicines(const QString &fileName);
    bool exportMedicines(const QString &fileName);


private:
    explicit IDatabase(QObject *parent = nullptr);
    IDatabase(IDatabase const&)  = delete;
    void operator=(IDatabase const&)  = delete;


    void initDatabase();


signals:


};

#endif // IDATABASE_H
