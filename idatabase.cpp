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

// QVector<DoctorInfo> IDatabase::getAllDoctors()
// {
//     QVector<DoctorInfo> doctors;
//     QSqlQuery query(database);

//     query.prepare("SELECT * FROM user WHERE permission_level > 0"); // 假设只有权限级别大于0的是医生

//     if (!query.exec()) {
//         qDebug() << "查询医生信息失败：" << query.lastError().text();
//         return doctors; // 返回空列表
//     }
//     while (query.next()) {
//         DoctorInfo doctor;
//         doctor.id = query.value("id").toInt();
//         doctor.certificateNumber = query.value("certificate_number").toString();
//         doctor.username = query.value("username").toString();
//         doctor.age = query.value("age").toInt();
//         DoctorInfo::Sex sex = DoctorInfo::sexFromString(query.value("sex").toString());
//         doctor.name = query.value("name").toString();

//         doctors.append(doctor);
//     }

//     return doctors;
// }

// bool IDatabase::addDoctor(const DoctorInfo &doctor)
// {
//     QSqlQuery query(database);

//     // 查询最大 ID
//     query.exec("SELECT COALESCE(MAX(ID), 0) AS max_id FROM user");
//     if (!query.next()) {
//         qDebug() << "查询最大 ID 失败：" << query.lastError().text();
//         return false;
//     }

//     int newId = query.value("max_id").toInt() + 1;

//     // 插入新用户数据
//     query.prepare("INSERT INTO Doctor(ID, CERTIFICATE_NUMBER, AGE, SEX,NAME, DEPARTMENT_ID) "
//                   "VALUES (:ID, :CERTIFICATE_NUMBER, :AGE, :SEX, :NAME, :DEPARTMENT_ID)");
//     query.bindValue(":ID", newId);
//     query.bindValue(":CERTIFICATE_NUMBER", doctor.certificateNumber);
//     query.bindValue(":AGE", doctor.age);
//     query.bindValue(":SEX", doctor.sex);
//     query.bindValue(" :NAME", doctor.name);
//     query.bindValue(":DEPARTMENT_ID", "0000000");
//     if (!query.exec()) {
//         qDebug() << "插入医生数据失败：" << query.lastError().text();
//         return false;
//     }

//     qDebug() << "医生添加成功";
//     return true;
// }




bool IDatabase::userRegister(const DoctorInfo &doctorInfo)
{
    QSqlQuery query(database);
    // 查询最大 ID
    query.exec("SELECT COALESCE(MAX(ID), 0) AS max_id FROM user");
    if (!query.next()) {
        qDebug() << "查询最大 ID 失败：" << query.lastError().text();
        return false;
    }

    int newId = query.value("max_id").toInt() + 1;
    // 检查是否已有相同账号
    query.prepare("SELECT COUNT(*) FROM user WHERE username = :USER");
    query.bindValue(":USER", doctorInfo.username);
    if (!query.exec() || !query.next()) {
        qDebug() << "查询账号是否存在时出错：" << query.lastError().text();
        return false;
    }

    int count = query.value(0).toInt();
    if (count > 0) {
        qDebug() << "该账号已存在";
        return false;
    }

    // 插入新用户数据
    query.prepare("INSERT INTO user (ID,FULLNAME, USERNAME,PASSWORD,permission_level, created_timestamp) "
                  "VALUES (:ID, :FULLNAME, :USERNAME , :PASSWORD, :PERMISSION_LEVEL, :CREATED_TIMESTAMP)");
    query.bindValue(":ID", newId);
    query.bindValue(":FULLNAME", doctorInfo.name);
    query.bindValue(":USERNAME", doctorInfo.username);
    //query.bindValue(":CERTIFICATE_NUMBER", doctorInfo.certificateNumber);
    query.bindValue(":PASSWORD", doctorInfo.password);
    query.bindValue(":PERMISSION_LEVEL", doctorInfo.permissionLevel);
    query.bindValue(":CREATED_TIMESTAMP", doctorInfo.createdTimestamp.toString("yyyy-MM-dd hh:mm:ss"));


    //调试使用
    // qDebug() << "Bound values:" << query.boundValues();
    // if (!query.exec()) {
    //     qDebug() << "插入用户数据失败：" << query.lastError().text();
    //     return false;
    // }

    qDebug() << "用户注册成功";
    return true;
}


DoctorInfo IDatabase::getUserInfo(const QString &userName)
{
    QSqlQuery query(database);
    DoctorInfo doctorInfo;

    query.prepare("SELECT * FROM user WHERE username = :USER");
    query.bindValue(":USER", userName);

    if (!query.exec() || !query.next()) {
        qDebug() << "查询用户信息失败：" << query.lastError().text();
        return doctorInfo; // 返回默认构造的空对象
    }

    doctorInfo.id = query.value("id").toInt();
    doctorInfo.username = query.value("username").toString();
    doctorInfo.name = query.value("name").toString();
    doctorInfo.age = query.value("age").toInt();
    doctorInfo.certificateNumber = query.value("certificate_number").toString();
    doctorInfo.password = query.value("password").toString();
    doctorInfo.permissionLevel = query.value("permission_level").toInt();
    doctorInfo.createdTimestamp = QDateTime::fromString(query.value("created_timestamp").toString(), "yyyy-MM-dd hh:mm:ss");

    return doctorInfo;
}


int IDatabase::getPermissionLevelByUsernamePrefix(const QString &userName)
{
    if (userName.length() < 2) {
        qDebug() << "用户名长度不足";
        return 0; // 默认权限级别为0
    }

    QString prefix = userName.left(2).toUpper(); // 获取前两位字符并转换为大写

    // 定义权限映射规则
    static const QMap<QString, int> permissionMap = {
        {"AD", 1}, // 假设前缀为 "AD" 表示管理员权限
        {"DO", 2}  // 假设前缀为 "DO" 表示医生权限
    };

    auto it = permissionMap.find(prefix);
    if (it != permissionMap.end()) {
        return it.value();
    } else {
        return 0; // 默认权限级别为0
    }
}

bool IDatabase::setUserPermissionLevel(const QString &userName, int permissionLevel)
{
    QSqlQuery query(database);
    query.prepare("UPDATE user SET permission_level = :LEVEL WHERE username = :USER");
    query.bindValue(":LEVEL", permissionLevel);
    query.bindValue(":USER", userName);

    if (!query.exec()) {
        qDebug() << "更新用户权限失败：" << query.lastError().text();
        return false;
    }

    return true;
}


