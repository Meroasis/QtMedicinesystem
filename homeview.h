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
    explicit HomeView(QWidget *parent = nullptr);
    ~HomeView();

private slots:
    void on_BtnRoom_clicked();

    void on_BtnDoctor_clicked();

    void on_BtnPatient_clicked();

signals:
    void goDepartmentView();
    void goDoctorView();
    void goPatientView();

private:
    Ui::HomeView *ui;
};

#endif // HOMEVIEW_H
