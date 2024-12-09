#include "masterview.h"
#include "ui_masterview.h"

MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    goLoginView();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);

    connect(loginView,SIGNAL(loginSuccess()),this,SLOT(goWelcomView()));
}

void MasterView::goWelcomView()
{
    welcomView = new HomeView(this);
    pushWidgetToStackView(welcomView);
    connect(welcomView,SIGNAL(goDoctorView()),this,SLOT(goDoctorView()));
    connect(welcomView,SIGNAL(goPatientView()),this,SLOT(goPatientView()));
    connect(welcomView,SIGNAL(goDepartmentView()),this,SLOT(goDepartmentView()));
}

void MasterView::goDoctorView()
{
    doctorView = new DoctorView(this);
    pushWidgetToStackView(doctorView);
}

void MasterView::goDepartmentView()
{
    departmentView = new RoomView(this);
    pushWidgetToStackView(departmentView);
}

void MasterView::goPatientView()
{
    patientView = new PatientView(this);
    pushWidgetToStackView(patientView);
    connect(patientView,SIGNAL(goPatientEditView()),this,SLOT(goPatientEditView()));
}

void MasterView::goPatientEditView()
{
    patientEditView = new PatientEditView(this);
    pushWidgetToStackView( patientEditView);
}

void MasterView::goPreviousView()
{
    int count = ui->stackedWidget->count();

    if(count > 1){
        ui->stackedWidget->setCurrentIndex(count - 2);
        ui->labelTile->setText(ui->stackedWidget->currentWidget()->windowTitle());

        QWidget *widget = ui->stackedWidget->widget(count - 1);
        ui->stackedWidget->removeWidget(widget);
        delete widget;

    }
}



void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count = ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count-1);
    ui->labelTile->setText(widget->windowTitle());
}

void MasterView::on_BtnBack_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int count = ui->stackedWidget->count();
    if(count > 1)
        ui->BtnBack->setEnabled(true);
    else
        ui->BtnBack->setEnabled(false);

    QString title = ui->stackedWidget->currentWidget()->windowTitle();
    if(title == "欢迎"){
        ui->Btnlogout->setEnabled(true);
        ui->BtnBack->setEnabled(false);
    }else
        ui->Btnlogout->setEnabled(false);
}


void MasterView::on_Btnlogout_clicked()
{
    goPreviousView();
    ui->BtnBack->setEnabled(false);
}

