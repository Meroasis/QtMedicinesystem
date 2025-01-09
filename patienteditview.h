#ifndef PATIENTEDITVIEW_H
#define PATIENTEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>
namespace Ui {
class PatientEditView;
}

class PatientEditView : public QWidget
{
    Q_OBJECT

public:
    explicit PatientEditView(QWidget *parent = nullptr,int rowNo = 0);
    ~PatientEditView();
    int calculateAge();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::PatientEditView *ui;
    QDataWidgetMapper  *dataMapper;//数据映射
    void updateAge();
    int calculateAge(const QDate &birthDate);
signals:
    void goPreviousView();
};

#endif // PATIENTEDITVIEW_H
