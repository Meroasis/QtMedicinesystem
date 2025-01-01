#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

namespace Ui {
class LoginView;
}

class LoginView : public QWidget
{
    Q_OBJECT

public:
    explicit LoginView(QWidget *parent = nullptr);
    ~LoginView();
    QString getUsername() const;
private slots:
    void on_btnSignIn_clicked();

    void on_btnSignUp_clicked();

signals:
    void loginSuccess();
    void loginFailed();
    void signUpClicked();
private:
    Ui::LoginView *ui;
};

#endif // LOGINVIEW_H
