#include "homeview.h"
#include "ui_homeview.h"

HomeView::HomeView(const QString &username, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeView)
{
    ui->setupUi(this);
    ui->Welcome->setText(tr("Welcome, %1").arg(username));
    setupPermissions(username);
}

HomeView::~HomeView()
{
    delete ui;
}

void HomeView::on_BtnRoom_clicked()
{
    emit goDepartmentView();
}


void HomeView::on_BtnDoctor_clicked()
{
    emit goDoctorView();
}


void HomeView::on_BtnPatient_clicked()
{
    emit goPatientView();
}


void HomeView::on_BtnDrug_clicked()
{
    emit goMedicineView();
}


void HomeView::on_BtnLog_clicked()
{
    emit goAppointmentsView();
}

void HomeView::setupPermissions(const QString &username)
{
    bool hasManageDoctorsPermission = username.startsWith("ad");
    // 根据权限级别启用或禁用按钮
    ui->BtnDoctor->setEnabled(hasManageDoctorsPermission);
}

