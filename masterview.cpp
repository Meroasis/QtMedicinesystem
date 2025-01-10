#include "masterview.h"
#include "ui_masterview.h"
#include"idatabase.h"
#include "registered.h"
#include "doctorinfo.h"
MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    goLoginView();

    IDatabase::getInstance();

    // 连接 LoginView 的 signUpClicked 信号与 MasterView 的 handleSignUpClicked 槽函数
    // connect(loginView, SIGNAL(signUpClicked()), this, SLOT(handleSignUpClicked()));

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
    connect(loginView, SIGNAL(signUpClicked()), this, SLOT(handleSignUpClicked())); // 连接注册信号
}

void MasterView::goWelcomView()
{
    // 获取当前登录用户的用户名
        // 从 LoginView 获取用户名

    if (!loginView) {
        qWarning() << "LoginView is null in goWelcomView!";
        return;
    }
    // 获取当前登录用户的用户名
    QString username = loginView->getUsername();
    qDebug() << "Retrieved username:" << username;
        // 创建 HomeView 并传递用户名
    HomeView *homeView = new HomeView(username, this);
    pushWidgetToStackView(homeView); // 将 HomeView 添加到堆栈
    connect(homeView,SIGNAL(goDoctorView()),this,SLOT(goDoctorView()));
    connect(homeView,SIGNAL(goPatientView()),this,SLOT(goPatientView()));
    connect(homeView,SIGNAL(goMedicineView()),this,SLOT(goMedicineView()));
    connect(homeView,SIGNAL(goAppointmentsView()),this,SLOT(goAppointmentsView()));
    connect(homeView,SIGNAL(goDepartmentView()),this,SLOT(goDepartmentView()));//连接到home而不是welcome
    // 获取当前登录用户的权限级别
    DoctorInfo doctorInfo = IDatabase::getInstance().getUserInfo(username);
    // setDoctorPermission(doctorInfo.permissionLevel); // 设置医生权限

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
    connect(patientView,SIGNAL(goPatientEditView(int)),this,SLOT(goPatientEditView(int)));
}

void MasterView::goMedicineView()
{
    medicineView = new MedicineView(this);
    pushWidgetToStackView(medicineView);
    connect(medicineView,SIGNAL(goMedicineEditView(int)),this,SLOT(goMedicineEditView(int)));
}

void MasterView::goAppointmentsView()
{
    appontmentsView =new AppointmentsView(this);
    pushWidgetToStackView(appontmentsView);

}



void MasterView::goPatientEditView(int rowNo)
{
    patientEditView = new PatientEditView(this,rowNo);
    pushWidgetToStackView( patientEditView);
     connect(patientEditView,SIGNAL(goPreviousView()),this,SLOT(goPreviousView()));
}

void MasterView::goMedicineEditView(int rowNo)
{
    medicineEditview = new Medicineeditview(this,rowNo);
    pushWidgetToStackView(medicineEditview);
    connect(medicineEditview,SIGNAL(goPreviousView()),this,SLOT(goPreviousView()));
}


void MasterView::goRegisterView()
{
    loginView = new LoginView(this);
    pushWidgetToStackView(loginView);

    connect(loginView, SIGNAL(loginSuccess()), this, SLOT(goWelcomView()));
    connect(loginView, SIGNAL(signUpClicked()), this, SLOT(handleSignUpClicked())); // 连接注册信号
}
void MasterView::handleSignUpClicked()
{
    registerView = new registered(this); // 创建注册对话框实例
    pushWidgetToStackView(registerView); // 将注册界面推入堆栈

    connect(registerView, SIGNAL(goPreviousView()), this, SLOT(goPreviousView()));
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
    if (widget) {
        ui->stackedWidget->addWidget(widget);
        int count = ui->stackedWidget->count();
        ui->stackedWidget->setCurrentIndex(count - 1);
        ui->labelTile->setText(widget->windowTitle());
    } else {
        qWarning() << "正在尝试将一个空的widget入栈";
    }
}

// void MasterView::setDoctorPermission(int permissionLevel)
// {
//     // ui->btnManageDoctors->setEnabled(permissionLevel >= 1);
// }





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

