#include "doctorview.h"
#include "ui_doctorview.h"
#include "doctorinfo.h"
#include <QDebug>
#include "idatabase.h"
DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
    , currentDoctorId(-1) // 初始化为无效 ID
{
    ui->setupUi(this);

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(on_btn_Add_clicked()));
    connect(ui->btnUpdate, SIGNAL(clicked()), this, SLOT(on_btn_Update_clicked()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(on_btn_Delete_clicked()));
    connect(ui->tableDoctor, SIGNAL(itemSelectionChanged()), this, SLOT(on_table_Doctors_itemSelectionChanged()));

    // 加载已有医生信息到表格
    loadDoctors();
}

DoctorView::~DoctorView()
{
    delete ui;
}

void DoctorView::loadDoctors()
{
    ui->tableDoctor->setRowCount(0);
    ui->tableDoctor->setColumnCount(6);
    ui->tableDoctor->setHorizontalHeaderLabels({"ID", "Name", "Sex","Age", "Certificate", "Permission Level", "Created At"});

    // 假设我们有一个 IDatabase 单例类来访问数据库
    QVector<DoctorInfo> doctors = IDatabase::getInstance().getAllDoctors();
    for (const auto &doctor : doctors) {
        int row = ui->table_Doctors->rowCount();
        ui->table_Doctors->insertRow(row);
        ui->table_Doctors->setItem(row, 0, new QTableWidgetItem(QString::number(doctor.id)));
        ui->table_Doctors->setItem(row, 1, new QTableWidgetItem(doctor.name));
        ui->table_Doctors->setItem(row, 2, new QTableWidgetItem(QString::number(doctor.age)));
        ui->table_Doctors->setItem(row, 3, new QTableWidgetItem(doctor.certificateNumber));
        ui->table_Doctors->setItem(row, 4, new QTableWidgetItem(QString::number(doctor.permissionLevel)));
        ui->table_Doctors->setItem(row, 5, new QTableWidgetItem(doctor.createdTimestamp.toString()));
    }
}

void DoctorView::addDoctor(const DoctorInfo &doctor)
{
    // 将新医生信息添加到数据库
    IDatabase::getInstance().addDoctor(doctor);

    // 更新表格显示
    int row = ui->table_Doctors->rowCount();
    ui->table_Doctors->insertRow(row);
    ui->table_Doctors->setItem(row, 0, new QTableWidgetItem(QString::number(doctor.id)));
    ui->table_Doctors->setItem(row, 1, new QTableWidgetItem(doctor.name));
    ui->table_Doctors->setItem(row, 2, new QTableWidgetItem(QString::number(doctor.age)));
    ui->table_Doctors->setItem(row, 3, new QTableWidgetItem(doctor.certificateNumber));
    ui->table_Doctors->setItem(row, 4, new QTableWidgetItem(QString::number(doctor.permissionLevel)));
    ui->table_Doctors->setItem(row, 5, new QTableWidgetItem(doctor.createdTimestamp.toString()));

    qDebug() << "Doctor added:" << doctor.name;
}

void DoctorView::updateDoctor(int id, const DoctorInfo &doctor)
{
    if (id > 0) {
        // 更新数据库中的医生信息
        IDatabase::getInstance().updateDoctor(id, doctor);

        // 更新表格显示
        for (int row = 0; row < ui->table_Doctors->rowCount(); ++row) {
            if (ui->table_Doctors->item(row, 0)->text().toInt() == id) {
                ui->table_Doctors->item(row, 1)->setText(doctor.name);
                ui->table_Doctors->item(row, 2)->setText(QString::number(doctor.age));
                ui->table_Doctors->item(row, 3)->setText(doctor.certificateNumber);
                ui->table_Doctors->item(row, 4)->setText(QString::number(doctor.permissionLevel));
                break;
            }
        }

        qDebug() << "Doctor updated:" << doctor.name;
    } else {
        qWarning() << "Invalid doctor ID for updating.";
    }
}

void DoctorView::deleteDoctor(int id)
{
    if (id > 0) {
        // 从数据库中删除医生信息
        IDatabase::getInstance().deleteDoctor(id);

        // 更新表格显示
        for (int row = 0; row < ui->table_Doctors->rowCount(); ++row) {
            if (ui->table_Doctors->item(row, 0)->text().toInt() == id) {
                ui->table_Doctors->removeRow(row);
                break;
            }
        }

        qDebug() << "Doctor deleted with ID:" << id;
    } else {
        qWarning() << "Invalid doctor ID for deleting.";
    }
}

void DoctorView::on_btnAdd_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed();
    QString specialty = ui->lineEdit_Specialty->text().trimmed();
    QString contact = ui->lineEdit_Contact->text().trimmed();
    int age = ui->spinBox_Age->value();
    QString certificateNumber = ui->lineEdit_CertificateNumber->text().trimmed();
    QString password = ui->lineEdit_Password->text().trimmed();
    int permissionLevel = ui->comboBox_PermissionLevel->currentIndex(); // 假设有权限级别下拉框

    if (!name.isEmpty() && !specialty.isEmpty() && !contact.isEmpty() && !certificateNumber.isEmpty() && !password.isEmpty()) {
        DoctorInfo doctor(0, "", name, age, certificateNumber, password, permissionLevel);
        addDoctor(doctor);
        ui->lineEdit_Name->clear();
        ui->lineEdit_Specialty->clear();
        ui->lineEdit_Contact->clear();
        ui->lineEdit_CertificateNumber->clear();
        ui->lineEdit_Password->clear();
    } else {
        qWarning() << "Please fill in all required fields before adding a doctor.";
    }
}

void DoctorView::on_btnUpdate_clicked()
{
    int id = getCurrentDoctorId();
    if (id > 0) {
        QString name = ui->lineEdit_Name->text().trimmed();
        QString specialty = ui->lineEdit_Specialty->text().trimmed();
        QString contact = ui->lineEdit_Contact->text().trimmed();
        int age = ui->spinBox_Age->value();
        QString certificateNumber = ui->lineEdit_CertificateNumber->text().trimmed();
        QString password = ui->lineEdit_Password->text().trimmed();
        int permissionLevel = ui->comboBox_PermissionLevel->currentIndex(); // 假设有权限级别下拉框

        if (!name.isEmpty() && !specialty.isEmpty() && !contact.isEmpty() && !certificateNumber.isEmpty() && !password.isEmpty()) {
            DoctorInfo doctor(id, "", name, age, certificateNumber, password, permissionLevel);
            updateDoctor(id, doctor);
        } else {
            qWarning() << "Please fill in all required fields before updating a doctor.";
        }
    } else {
        qWarning() << "Please select a doctor to update.";
    }
}

void DoctorView::on_btnDelete_clicked()
{
    int id = getCurrentDoctorId();
    if (id > 0) {
        deleteDoctor(id);
    } else {
        qWarning() << "Please select a doctor to delete.";
    }
}

void DoctorView::on_table_Doctors_itemSelectionChanged()
{
    QList<QTableWidgetItem*> selectedItems = ui->table_Doctors->selectedItems();
    if (!selectedItems.isEmpty()) {
        int row = selectedItems.first()->row();
        currentDoctorId = ui->table_Doctors->item(row, 0)->text().toInt();

        // 填充表单字段以供编辑
        ui->lineEdit_Name->setText(ui->table_Doctors->item(row, 1)->text());
        ui->spinBox_Age->setValue(ui->table_Doctors->item(row, 2)->text().toInt());
        ui->lineEdit_CertificateNumber->setText(ui->table_Doctors->item(row, 3)->text());
        ui->comboBox_PermissionLevel->setCurrentIndex(ui->table_Doctors->item(row, 4)->text().toInt());

        // 注意：这里没有填充密码和专业（specialty），因为它们不是表格的一部分。
        // 如果需要编辑这些字段，应该在表格中添加相应的列或提供其他方式获取完整信息。
    } else {
        currentDoctorId = -1;
        // 清空表单字段
        ui->lineEdit_Name->clear();
        ui->spinBox_Age->setValue(0);
        ui->lineEdit_CertificateNumber->clear();
        ui->comboBox_PermissionLevel->setCurrentIndex(0);
    }
}

int DoctorView::getCurrentDoctorId() const
{
    return currentDoctorId;
}







