#include "loginview.h"
#include "ui_loginview.h"
#include "idatabase.h"
#include <QMessageBox.h>
LoginView::LoginView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginView)
{
    ui->setupUi(this);
}

LoginView::~LoginView()
{
    delete ui;
}

QString LoginView::getUsername() const
{
    return ui->InputName->text();
}




void LoginView::on_btnSignIn_clicked()
{
    QString status = IDatabase::getInstance().userLogin(ui->InputName->text(), ui->InputPassword->text());
    if (status == "login_Ok") {
        emit loginSuccess();
    } else {
        QMessageBox::warning(this, "警告", "用户名或密码错误！");
    }
}


void LoginView::on_btnSignUp_clicked()
{
    emit signUpClicked();
}

