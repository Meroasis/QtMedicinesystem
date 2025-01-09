#ifndef MEDICINEVIEW_H
#define MEDICINEVIEW_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QDataWidgetMapper>
namespace Ui {
class MedicineView;
}

class MedicineView : public QWidget
{
    Q_OBJECT

public:
    explicit MedicineView(QWidget *parent = nullptr);
    ~MedicineView();

private slots:
    void on_BtnFind_clicked();
    void on_BtnAdd_clicked();
    void on_BtnDelete_clicked();
    void on_BtnChange_clicked();

    void on_actionImportCSV_clicked();
    void on_actionExportCSV_clicked();
    void updateDeleteButtonState();
signals:
    void goMedicineEditView(int row);

private:
    Ui::MedicineView *ui;
    QSortFilterProxyModel *proxyModel;
    int sortColumn = 0;
    bool sortAscending = true;
public slots:
    void handleHorizontalHeaderSectionClicked(int logicalIndex);
};

#endif // MEDICINEVIEW_H
