#ifndef HOMEVIEW_H
#define HOMEVIEW_H

#include <QWidget>

namespace Ui {
class HomeView;
}

class HomeView : public QWidget
{
    Q_OBJECT

public:
    explicit HomeView(const QString &username,QWidget *parent = nullptr);
    ~HomeView();

private slots:
    void on_BtnRoom_clicked();

    void on_BtnDoctor_clicked();

    void on_BtnPatient_clicked();

    void on_BtnDrug_clicked();

    void on_BtnLog_clicked();

signals:
    void goDepartmentView();
    void goDoctorView();
    void goPatientView();

private:
    Ui::HomeView *ui;
    void setupPermissions(const QString &username);
};

#endif // HOMEVIEW_H
