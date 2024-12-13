#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include <QWidget>
#include "loginview.h"
#include "doctorview.h"
#include "roomview.h"
#include "patienteditview.h"
#include "patientview.h"
#include "homeview.h"
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
    void goPatientEditView(int rowNo);


private slots:
    void on_BtnBack_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_Btnlogout_clicked();

private:
    void pushWidgetToStackView(QWidget *widget);

    Ui::MasterView *ui;

    HomeView *welcomView;
    DoctorView *doctorView;
    PatientView *patientView;
    RoomView *departmentView;
    LoginView *loginView;
    PatientEditView *patientEditView;


};
#endif // MASTERVIEW_H
