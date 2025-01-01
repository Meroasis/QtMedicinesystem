#ifndef REGISTERED_H
#define REGISTERED_H

#include <QDialog>

namespace Ui {
class registered;
}

class registered : public QDialog
{
    Q_OBJECT

public:
    explicit registered(QWidget *parent = nullptr);
    ~registered();

private slots:
    void on_registerButton_clicked();

private:
    Ui::registered *ui;

signals:
    void goPreviousView();

};

#endif // REGISTERED_H
