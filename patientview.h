#ifndef PATIENTVIEW_H
#define PATIENTVIEW_H

#include <QWidget>

namespace Ui {
class PatientView;
}

class PatientView : public QWidget
{
    Q_OBJECT

public:
    explicit PatientView(QWidget *parent = nullptr);
    ~PatientView();

private slots:
    void on_BtnAdd_clicked();
    void on_BtnFind_clicked();

    void on_BtnDelete_clicked();

    void on_BtnChange_clicked();

signals:
    void goPatientEditView(int idx);

private:
    Ui::PatientView *ui;
};

#endif // PATIENTVIEW_H
