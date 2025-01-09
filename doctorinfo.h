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
    enum Sex { MALE, FEMALE }; // 性别枚举
    Sex sex = MALE; // 性别
    QString department_ID = "0000";


    // 默认构造函数
    DoctorInfo() : id(0), age(0), permissionLevel(0) {}

    // 构造函数，用于初始化所有成员变量
    DoctorInfo(int _id, const QString &_username, const QString &_name, int _age,
               const QString &_certificateNumber, const QString &_password, int _permissionLevel)
        : id(_id), username(_username), name(_name), age(_age),
        certificateNumber(_certificateNumber), password(_password),
        permissionLevel(_permissionLevel),createdTimestamp(QDateTime::currentDateTime()) {}

    DoctorInfo(int _id , const QString &_name, Sex _sex , int _age , const QString &_certificateNumber )
        : id(_id), name(_name), sex(_sex), age(_age), certificateNumber(_certificateNumber) {}

    // 将字符串转换为 Sex 枚举
    static Sex sexFromString(const QString &sexStr) {
        if (sexStr.toLower() == "male") {
            return MALE;
        } else if (sexStr.toLower() == "female") {
            return FEMALE;
        } else {
            qWarning() << "Unknown sex string:" << sexStr;
            return MALE; // 默认返回 MALE
        }
    }

    // 将 Sex 枚举转换为字符串
    static QString sexToString(Sex sex) {
        return sex == MALE ? "Male" : "Female";
    }

};



#endif // DOCTORINFO_H
