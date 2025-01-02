#ifndef DOCTORINFO_H
#define DOCTORINFO_H

#include <QString>
#include <QDateTime>

struct DoctorInfo {
    int id; // 医生 ID
    QString username; // 登录用户名
    QString name; // 真实姓名
    int age; // 年龄
    QString certificateNumber; // 执业证书号
    QString password; // 密码
    int permissionLevel; // 权限级别
    QDateTime createdTimestamp; // 创建时间戳
    QString sex;//性别


    // 默认构造函数
    DoctorInfo() : id(0), age(0), permissionLevel(0) {}

    // 构造函数，用于初始化所有成员变量
    DoctorInfo(int _id, const QString &_username, const QString &_name, int _age,
               const QString &_certificateNumber, const QString &_password, int _permissionLevel, QString _sex)
        : id(_id), username(_username), name(_name), age(_age),
        certificateNumber(_certificateNumber), password(_password),
        permissionLevel(_permissionLevel),createdTimestamp(QDateTime::currentDateTime()), sex(_sex) {}
};

#endif // DOCTORINFO_H
