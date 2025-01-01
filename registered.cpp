#include "registered.h"
#include "ui_registered.h"
#include <QMessageBox.h>
#include "idatabase.h"
registered::registered(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::registered)
{
    ui->setupUi(this);
}

registered::~registered()
{
    delete ui;
}

void registered::on_registerButton_clicked()
{
    QString account = ui->accountLineEdit->text();
    QString name = ui->nameLineEdit->text();
    int age = ui->ageSpinBox->value();
    QString certificateNumber = ui->certificateLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();

    // 验证必填项是否为空
    if (account.isEmpty() || name.isEmpty() || certificateNumber.isEmpty() ||
        password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写所有必填项！");
        return;
    }

    // 验证密码是否匹配
    if (password != confirmPassword) {
        QMessageBox::warning(this, "警告", "密码和确认密码不匹配，请重新输入！");
        return;
    }
    // 根据用户名前缀确定权限级别
    int permissionLevel = IDatabase::getInstance().getPermissionLevelByUsernamePrefix(account);
    DoctorInfo doctorInfoUser(-1, account, name, age, certificateNumber, password, permissionLevel);
    if (IDatabase::getInstance().userRegister(doctorInfoUser)) {
        QMessageBox::information(this, "成功", "注册成功！");
        emit goPreviousView(); // 注册成功后返回上一页
    } else {
        QMessageBox::critical(this, "错误", "注册失败，请检查输入或稍后再试！");
    }
}

