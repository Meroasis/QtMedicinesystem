#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "loginview.h"
#include "doctorview.h"
#include "roomview.h"
#include "patienteditview.h"
#include "patientview.h"
#include "homeview.h"
#include "registered.h"
#include "medicineview.h"
#include "medicineeditview.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();


public slots:
    void goLoginView();
    void goWelcomView();
    void goDoctorView();
    void goDepartmentView();
    void goPatientView();
    void goPreviousView();
    void goMedicineView();
    void goPatientEditView(int rowNo);
    void goMedicineEditView(int rowNo);
    void goRegisterView();     // 前往注册界面的方法
    void handleSignUpClicked(); // 处理来自 LoginView 的注册请求

private slots:
    void on_BtnBack_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_Btnlogout_clicked();


private:
    void pushWidgetToStackView(QWidget *widget);
    void setDoctorPermission(int permissionLevel);
    Ui::MasterView *ui;

    HomeView *welcomView;
    DoctorView *doctorView;
    PatientView *patientView;
    RoomView *departmentView;
    LoginView *loginView;
    PatientEditView *patientEditView;
    registered *registerView;
    MedicineView *medicineView;
    Medicineeditview *medicineEditview;
    int doctorPermissionLevel; // 添加医生权限级别成员变量

};
#endif // MASTERVIEW_H
