#include "loginview.h"
#include "ui_loginview.h"
#include "idatabase.h"
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




void LoginView::on_btnSignIn_clicked()
{
    QString status = IDatabase::getInstance().userLogin(ui->InputName->text(),ui->InputPassword->text());
    if(status == "login_Ok")
        emit loginSuccess();

}

