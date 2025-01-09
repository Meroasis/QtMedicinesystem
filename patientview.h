#ifndef PATIENTVIEW_H
#define PATIENTVIEW_H

#include <QWidget>
#include <QSortFilterProxyModel>
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
private slots:
    void on_tablePatients_horizontalHeader_clicked(int logicalIndex);

    void on_actionImportCSV_clicked();

    void on_actionExportCSV_clicked();
    void updateDeleteButtonState();

private:
    Ui::PatientView *ui;
    int sortColumn; // 当前排序列
    bool sortAscending; // 排序顺序，默认升序
    QSortFilterProxyModel* proxyModel;
    void exportToCSV(); //
    void importFromCSV();
};

#endif // PATIENTVIEW_H
