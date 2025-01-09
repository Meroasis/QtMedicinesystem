#include "idatabase.h"
#include <QUuid>
#include "patienteditview.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSortFilterProxyModel>
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
    patientTabModel->setTable("Patient");
    patientTabModel->setEditStrategy(
        QSqlTableModel::OnManualSubmit);//数据保存方式
    patientTabModel->setSort(patientTabModel->fieldIndex("NAME"),Qt::AscendingOrder);//排序
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

bool IDatabase::deleteCurrentPatient(QItemSelectionModel *selectionModel)
{
    if (!selectionModel) {
        QMessageBox::critical(nullptr, tr("Error"), tr("No selection model available."));
        return false;
    }

    // 获取当前选中的源模型索引
    QModelIndex curIndex = selectionModel->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::information(nullptr, tr("Information"), tr("Please select a patient to delete."));
        return false;
    }

    // 确认删除操作
    int ret = QMessageBox::question(nullptr, tr("Confirm Delete"), tr("Are you sure you want to delete this patient?"),
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret != QMessageBox::Yes) {
        return false;
    }

    // 尝试移除行
    bool success = patientTabModel->removeRow(curIndex.row());
    if (!success) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Failed to remove row: ") + patientTabModel->lastError().text());
        return false;
    }

    // 提交所有更改到数据库
    success = patientTabModel->submitAll();
    if (!success) {
        QMessageBox::critical(nullptr, tr("Error"), tr("Failed to submit changes: ") + patientTabModel->lastError().text());
        patientTabModel->revertAll(); // 回滚更改
        return false;
    }

    // 刷新模型数据
    patientTabModel->select();

    // 更新选择模型（如果需要）
    int newRow = qMin(curIndex.row(), patientTabModel->rowCount() - 1);
    if (newRow >= 0 && newRow < patientTabModel->rowCount()) {
        selectionModel->setCurrentIndex(patientTabModel->index(newRow, curIndex.column()), QItemSelectionModel::ClearAndSelect);
    } else {
        selectionModel->clearSelection();
    }

    return true;
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
    bool success = patientTabModel->insertRow(patientTabModel->rowCount());
    if (!success) {
        qWarning() << "Failed to insert a new row into the patient table.";
        return -1;
    }
    // QModelIndex curIndex = patientTabModel->index(patientTabModel->rowCount() - 1,1);//创建最后一行的ModelIndex
    // int curRecNo =curIndex.row();
    // QSqlRecord curRec=patientTabModel->record(curRecNo);//获取当前记录
    // 获取最后一行的索引
    int curRecNo = patientTabModel->rowCount() - 1;
    // 获取当前记录
    // QSqlRecord curRec = patientTabModel->record(curRecNo);

    // 设置 CREATEDTIMESTAMP 字段值为当前日期时间
    QModelIndex idxCreatedTimestamp = patientTabModel->index(curRecNo, patientTabModel->fieldIndex("CREATEDTIMESTAMP"));
    patientTabModel->setData(idxCreatedTimestamp, QDateTime::currentDateTime().toString("yyyy/MM/dd"));

    // 设置 ID 字段值为新生成的 UUID
    QModelIndex idxId = patientTabModel->index(curRecNo, patientTabModel->fieldIndex("ID"));
    patientTabModel->setData(idxId, QUuid::createUuid().toString(QUuid::WithoutBraces));
    // 设置其他字段为空字符串或默认值
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("NAME")), "");
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("SEX")), "");
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("HEIGHT")), 0);
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("WEIGHT")), 0);
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("MOBILEPHONE")), "");
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("ID_CARD")), "");
    patientTabModel->setData(patientTabModel->index(curRecNo, patientTabModel->fieldIndex("AGE")), "");

    // patientTabModel->setRecord(curRecNo,curRec);
    // patientTabModel->submitAll();
    return curRecNo;
}






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

QVector<DoctorInfo> IDatabase::getAllDoctors()
{
    QVector<DoctorInfo> allDoctors;
    QSqlQuery query("SELECT * FROM Doctor", database);
    while (query.next()) {
        DoctorInfo doctor;
        doctor.id = query.value("id").toInt();
        doctor.name = query.value("name").toString();
        doctor.age = query.value("age").toInt();
        doctor.sex = static_cast<DoctorInfo::Sex>(query.value("sex").toInt());
        doctor.certificateNumber = query.value("certificate_number").toString();
        allDoctors.append(doctor);
    }
    return allDoctors;
}

// 添加新医生
bool IDatabase::addNewDoctor(const DoctorInfo &doctor)
{
    QSqlQuery query(database);
    query.prepare("INSERT INTO Doctor (ID, CERTIFICATE_NUMBER, NAME, AGE, SEX, DEPARTMENT_ID)"
                  " VALUES (:ID, :CERTIFICATE_NUMBER, :NAME, :AGE, :SEX, :DEPARTMENT_ID)");
    query.bindValue(":ID", doctor.id);
    query.bindValue(":CERTIFICATE_NUMBER", doctor.certificateNumber);
    query.bindValue(":NAME", doctor.name);
    query.bindValue(":AGE", doctor.age);
    query.bindValue(":SEX", doctor.sex);
    query.bindValue(":DEPARTMENT_ID", "0000");

    return query.exec();
}

// 更新医生信息
bool IDatabase::updateDoctor(const DoctorInfo &doctor)
{
    QSqlQuery query(database);
    query.prepare("UPDATE Doctor SET name = ?, sex = ?, age = ?, certificate_number = ? WHERE id = ?");
    query.addBindValue(doctor.name);
    query.addBindValue(static_cast<int>(doctor.sex));
    query.addBindValue(doctor.age);
    query.addBindValue(doctor.certificateNumber);
    query.addBindValue(doctor.id);

    return query.exec();
}

// 删除医生信息
bool IDatabase::deleteDoctor(int id)
{
    QSqlQuery query(database);
    query.prepare("DELETE FROM Doctor WHERE id = ?");
    query.addBindValue(id);

    return query.exec();
}




