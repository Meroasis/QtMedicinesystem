#ifndef MEDICINEEDITVIEW_H
#define MEDICINEEDITVIEW_H

#include <QWidget>
#include <QDataWidgetMapper>
namespace Ui {
class Medicineeditview;
}

class Medicineeditview : public QWidget
{
    Q_OBJECT

public:
    explicit Medicineeditview(QWidget *parent = nullptr,int rowNo = 0);
    ~Medicineeditview();

private slots:
    void on_BtnSave_clicked();

    void on_BtnCancel_clicked();

private:
    Ui::Medicineeditview *ui;
    QDataWidgetMapper  *dataMapper;//数据映射
signals:
    void goPreviousView();

};

#endif // MEDICINEEDITVIEW_H
