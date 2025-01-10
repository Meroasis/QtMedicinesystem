#ifndef APPOINTMENTSVIEW_H
#define APPOINTMENTSVIEW_H

#include <QWidget>

namespace Ui {
class AppointmentsView;
}

class AppointmentsView : public QWidget
{
    Q_OBJECT

public:
    explicit AppointmentsView(QWidget *parent = nullptr);
    ~AppointmentsView();

private slots:
    void on_btnsave_clicked();

private:
    Ui::AppointmentsView *ui;
    void populateComboBoxes();
    void collectSelectionData(QString &patientName,
                                QString &doctorName,
                              QString &medicineName);
    QString getTextDescription();
    bool exportSelectionToFile(const QString &filePath);
};

#endif // APPOINTMENTSVIEW_H
