#include "doctorview.h"
#include "ui_doctorview.h"
#include <QDebug>
#include "doctorinfo.h"
DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
{
    ui->setupUi(this);

    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(on_btnAdd_clicked));
    connect(ui->btnUpdate, SIGNAL(clicked()), this, SLOT(on_btn_Update_clicked()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(on_btn_Delete_clicked()));
    connect(ui->tableDoctors, SIGNAL(itemSelectionChanged()), this, SLOT(on_tableDoctors_itemSelectionChanged()));
    connect(ui->tableDoctors->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_tableDoctors_horizontalHeader_clicked(int)));
    // 初始化表格
    loadDoctorsToTable();
}

DoctorView::~DoctorView()
{
    delete ui;
}

void DoctorView::loadDoctorsToTable()
{
    ui->tableDoctors->setRowCount(0);
    ui->tableDoctors->setColumnCount(5);
    ui->tableDoctors->setHorizontalHeaderLabels({"ID", "Name", "Sex", "Age", "Certificate Number"});

    QVector<DoctorInfo> filteredDoctors = doctors;

    if (!currentSearchTerm.isEmpty()) {
        filteredDoctors.erase(std::remove_if(filteredDoctors.begin(), filteredDoctors.end(),
                                             [this](const DoctorInfo &doctor) {
                                                 return !doctor.name.contains(currentSearchTerm, Qt::CaseInsensitive) &&
                                                        !doctor.certificateNumber.contains(currentSearchTerm, Qt::CaseInsensitive);
                                             }),
                              filteredDoctors.end());
    }

    for (const auto &doctor : doctors) {
        int row = ui->tableDoctors->rowCount();
        ui->tableDoctors->insertRow(row);
        ui->tableDoctors->setItem(row, 0, new QTableWidgetItem(QString::number(doctor.id)));
        ui->tableDoctors->setItem(row, 1, new QTableWidgetItem(doctor.name));
        ui->tableDoctors->setItem(row, 2, new QTableWidgetItem(DoctorInfo::sexToString(doctor.sex)));
        ui->tableDoctors->setItem(row, 3, new QTableWidgetItem(QString::number(doctor.age)));
        ui->tableDoctors->setItem(row, 4, new QTableWidgetItem(doctor.certificateNumber));
    }
    ui->tableDoctors->viewport()->update();
}

int DoctorView::getNextId()
{
    if (doctors.isEmpty()) {
        return 1;
    } else {
        return doctors.last().id + 1;
    }
}

void DoctorView::searchDoctors(const QString &term)
{
    currentSearchTerm = term;
    loadDoctorsToTable();
}

void DoctorView::clearForm()
{
    ui->lineEdit_Name->clear();
    ui->spinBox_Age->setValue(0);
    ui->lineEdit_CertificateNumber->clear();
    ui->radioButton_Male->setChecked(true); // 默认选中 Male 按钮
}

void DoctorView::on_btn_Add_clicked()
{
    qDebug() << "Button Add clicked";
}

void DoctorView::on_btn_Update_clicked()
{
    if (currentDoctorId > 0) {
        QString name = ui->lineEdit_Name->text().trimmed();
        int age = ui->spinBox_Age->value();
        QString certificateNumber = ui->lineEdit_CertificateNumber->text().trimmed();
        DoctorInfo::Sex sex = (ui->radioButton_Male->isChecked()) ? DoctorInfo::MALE : DoctorInfo::FEMALE;

        if (!name.isEmpty() && !certificateNumber.isEmpty()) {
            for (auto &doctor : doctors) {
                if (doctor.id == currentDoctorId) {
                    doctor.name = name;
                    doctor.age = age;
                    doctor.certificateNumber = certificateNumber;
                    doctor.sex = sex;
                    break;
                }
            }

            loadDoctorsToTable(); // 更新表格显示
            clearForm(); // 清空表单字段

            qDebug() << "Doctor updated:" << name;
        } else {
            qWarning() << "Please fill in all required fields before updating a doctor.";
        }
    } else {
        qWarning() << "Please select a doctor to update.";
    }
}

void DoctorView::on_btn_Delete_clicked()
{
    if (currentDoctorId > 0) {
        doctors.erase(std::remove_if(doctors.begin(), doctors.end(),
                                     [this](const DoctorInfo &doctor) { return doctor.id == currentDoctorId; }),
                      doctors.end());

        loadDoctorsToTable(); // 更新表格显示
        clearForm(); // 清空表单字段

        qDebug() << "医生删除成功" << currentDoctorId;
    } else {
        qWarning() << "Please select a doctor to delete.";
    }
}

void DoctorView::on_tableDoctors_itemSelectionChanged()
{
    QList<QTableWidgetItem*> selectedItems = ui->tableDoctors->selectedItems();
    if (!selectedItems.isEmpty()) {
        int row = selectedItems.first()->row();
        currentDoctorId = ui->tableDoctors->item(row, 0)->text().toInt();

        ui->lineEdit_Name->setText(ui->tableDoctors->item(row, 1)->text());
        ui->spinBox_Age->setValue(ui->tableDoctors->item(row, 3)->text().toInt());
        ui->lineEdit_CertificateNumber->setText(ui->tableDoctors->item(row, 4)->text());

        QString sexStr = ui->tableDoctors->item(row, 2)->text();
        if (sexStr.toLower() == "male") {
            ui->radioButton_Male->setChecked(true);
        } else if (sexStr.toLower() == "female") {
            ui->radioButton_Female->setChecked(true);
        }
    } else {
        currentDoctorId = -1;
        clearForm();
    }
}

void DoctorView::on_btnAdd_clicked()
{
    QString name = ui->lineEdit_Name->text().trimmed();
    int age = ui->spinBox_Age->value();
    DoctorInfo::Sex sex = (ui->radioButton_Male->isChecked()) ? DoctorInfo::MALE : DoctorInfo::FEMALE;
    QString certificateNumber = ui->lineEdit_CertificateNumber->text().trimmed();

    if (!name.isEmpty() && !certificateNumber.isEmpty()) {
        int id = getNextId();
        DoctorInfo doctor(id, name, sex, age, certificateNumber);
        doctors.append(doctor);

        loadDoctorsToTable(); // 更新表格显示
        clearForm(); // 清空表单字段

        qDebug() << "Doctor added:" << name;
    } else {
        qWarning() << "请正确地添加医生信息";
    }
}


void DoctorView::on_btnSearch_clicked()
{
    QString searchTerm = ui->lineEdit_Search->text().trimmed();
    searchDoctors(searchTerm);
}

void DoctorView::on_tableDoctors_horizontalHeader_clicked(int logicalIndex)
{
    if (logicalIndex == sortColumn) {
        sortAscending = !sortAscending;
    } else {
        sortColumn = logicalIndex;
        sortAscending = true;
    }

    std::sort(doctors.begin(), doctors.end(), [this, logicalIndex](const DoctorInfo &a, const DoctorInfo &b) {
        switch (logicalIndex) {
        case 0: return sortAscending ? a.id < b.id : a.id > b.id;
        case 1: return sortAscending ? a.name.compare(b.name, Qt::CaseInsensitive) < 0 : a.name.compare(b.name, Qt::CaseInsensitive) > 0;
        case 2: return sortAscending ? a.sex < b.sex : a.sex > b.sex;
        case 3: return sortAscending ? a.age < b.age : a.age > b.age;
        case 4: return sortAscending ? a.certificateNumber.compare(b.certificateNumber, Qt::CaseInsensitive) < 0 : a.certificateNumber.compare(b.certificateNumber, Qt::CaseInsensitive) > 0;
        default: return false;
        }
    });

    loadDoctorsToTable();
}

