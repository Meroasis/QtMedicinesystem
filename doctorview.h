#ifndef DOCTORVIEW_H
#define DOCTORVIEW_H

#include <QWidget>
#include "ui_doctorview.h"
#include "doctorInfo.h"
class DoctorView : public QWidget
{
    Q_OBJECT

public:
    explicit DoctorView(QWidget *parent = nullptr);
    ~DoctorView();

private slots:
    void on_btn_Add_clicked();
    void on_btn_Update_clicked();
    void on_btn_Delete_clicked();
    void on_table_Doctors_itemSelectionChanged();

    void on_btnAdd_clicked();

    void on_btnDelete_clicked();

    void on_btnUpdate_clicked();

private:
    Ui::DoctorView *ui;
    void loadDoctors(); // 加载医生信息到表格
    void addDoctor(const DoctorInfo &doctor); // 添加医生
    void updateDoctor(int id, const DoctorInfo &doctor); // 更新医生信息
    void deleteDoctor(int id); // 删除医生信息
    int getCurrentDoctorId() const; // 获取当前选中的医生 ID
    int currentDoctorId; // 存储当前选中的医生 ID
};

#endif // DOCTORVIEW_H
