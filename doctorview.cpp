#include "doctorview.h"
#include "ui_doctorview.h"
#include <QDebug>
#include "doctorinfo.h"
#include "highlightdelegate.h"
#include <QTextDocument>
#include <QTableWidgetItem>
#include <QMessageBox>
#include "idatabase.h"
#include <QFileDialog>
#include <QTextStream>
DoctorView::DoctorView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DoctorView)
{
    ui->setupUi(this);
    loadDoctorsFromDatabase();
    IDatabase &database = IDatabase::getInstance();


    connect(ui->btnAdd, SIGNAL(clicked()), this, SLOT(on_btnAdd_clicked));
    connect(ui->btnUpdate, SIGNAL(clicked()), this, SLOT(on_btn_Update_clicked()));
    connect(ui->btnDelete, SIGNAL(clicked()), this, SLOT(on_btn_Delete_clicked()));
    connect(ui->btnSearch, SIGNAL(clicked()), this, SLOT(on_btnSearch_clicked()));
    // connect(ui->tableDoctors, SIGNAL(itemSelectionChanged()), this, SLOT(on_tableDoctors_itemSelectionChanged()));
    connect(ui->tableDoctors, &QTableWidget::itemSelectionChanged, this, &DoctorView::on_tableDoctors_itemSelectionChanged);
    connect(ui->tableDoctors->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_tableDoctors_horizontalHeader_clicked(int)));
    connect(ui->btnImport, SIGNAL(clicked()), this, SLOT(on_btnImport_clicked()));
    connect(ui->btnExport, SIGNAL(clicked()), this, SLOT(on_btnExport_clicked()));
    ui->tableDoctors->setItemDelegate(new HighlightDelegate(ui->tableDoctors));
    // 初始化表格
    loadDoctorsToTable(true);
}

DoctorView::~DoctorView()
{
    delete ui;
}

void DoctorView::loadDoctorsToTable(bool shouldSave)
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

    for (const auto &doctor : filteredDoctors) {
        int row = ui->tableDoctors->rowCount();
        ui->tableDoctors->insertRow(row);


        // 高亮显示匹配项
        QString idStr = highlightText(QString::number(doctor.id), currentSearchTerm);
        QString nameStr = highlightText(doctor.name, currentSearchTerm);
        QString sexStr = DoctorInfo::sexToString(doctor.sex);
        QString ageStr = QString::number(doctor.age);
        QString certNumStr = highlightText(doctor.certificateNumber, currentSearchTerm);

        // 创建 QTableWidgetItem 并根据情况设置富文本数据
        QTableWidgetItem *idItem = new QTableWidgetItem(idStr);
        QTableWidgetItem *nameItem = new QTableWidgetItem(nameStr);
        QTableWidgetItem *sexItem = new QTableWidgetItem(sexStr);
        QTableWidgetItem *ageItem = new QTableWidgetItem(ageStr);
        QTableWidgetItem *certNumItem = new QTableWidgetItem(certNumStr);

        ui->tableDoctors->setItem(row, 0, idItem);
        ui->tableDoctors->setItem(row, 1, nameItem);
        ui->tableDoctors->setItem(row, 2, sexItem);
        ui->tableDoctors->setItem(row, 3, ageItem);
        ui->tableDoctors->setItem(row, 4, certNumItem);

        // 更新具有高亮显示的项目
        updateItemWithHighlight(idItem, idStr);
        updateItemWithHighlight(nameItem, nameStr);
        updateItemWithHighlight(certNumItem, certNumStr);
    }
    ui->tableDoctors->viewport()->update();
    if (shouldSave) {
        saveDoctorsToDatabase(filteredDoctors);
    }

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
    loadDoctorsToTable(false);
}

QString DoctorView::highlightText(const QString &text, const QString &term)
{
    QString highlightedText = text;
    if (!term.isEmpty() && text.contains(term, Qt::CaseInsensitive)) {
        QTextDocument doc;
        QTextCursor cursor(&doc);
        doc.setPlainText(text);

        QTextCharFormat fmt;
        fmt.setBackground(Qt::yellow); // 设置高亮背景色

        while (!cursor.isNull() && !cursor.atEnd()) {
            cursor.beginEditBlock();
            cursor = doc.find(term, cursor, QTextDocument::FindFlag()); // 使用默认查找标志
            if (!cursor.hasSelection()) break; // 如果没有找到匹配项，则退出循环

            cursor.mergeCharFormat(fmt);

            // 移动光标到选区末尾以继续搜索下一个匹配项
            cursor.movePosition(QTextCursor::EndOfWord);
            cursor.endEditBlock();
        }

        highlightedText = doc.toHtml();
    }
    return highlightedText;
}

void DoctorView::updateItemWithHighlight(QTableWidgetItem *item, const QString &html)
{
    if (item && !html.isEmpty()) {
        item->setText(""); // 清空文本，因为我们将通过富文本格式化来显示内容
        item->setData(Qt::UserRole + 1, html); // 存储原始 HTML 文本
    }
}

void DoctorView::saveDoctorsToDatabase(const QVector<DoctorInfo> &doctors)
{
    IDatabase &database = IDatabase::getInstance();
    // 开始事务
    if (!database.database.transaction()) {
        qCritical() << "Failed to start transaction.";
        return;
    }

    // 删除所有现有医生记录
    QSqlQuery query(database.database);
    if (!query.exec("DELETE FROM Doctor")) {
        database.database.rollback();
        qCritical() << "Failed to delete existing Doctor:" << query.lastError().text();
        return;
    }

    // 插入新的医生记录
    foreach (const DoctorInfo& doctor, doctors) {
        if (!database.addNewDoctor(doctor)) {
            database.database.rollback();
            qCritical() << "Failed to add doctor:" << doctor.name;
            return;
        }
    }

    // 提交事务
    if (!database.database.commit()) {
        qCritical() << "Failed to commit transaction.";
    } else {
        qDebug() << "All doctors saved successfully.";
    }
}

void DoctorView::loadDoctorsFromDatabase()
{
    IDatabase &database = IDatabase::getInstance();
    doctors = database.getAllDoctors();

}



void DoctorView::clearForm()
{
    ui->lineEdit_Name->clear();
    ui->spinBox_Age->setValue(0);
    ui->lineEdit_CertificateNumber->clear();
    ui->radioButton_Male->setChecked(true);
}

void DoctorView::on_btn_Add_clicked()
{
    qDebug() << "Button Add clicked";
}

void DoctorView::on_btn_Update_clicked()
{
    // 检查是否有选中的医生
    if (currentDoctorId <= 0) {
        QMessageBox::warning(this, tr("No Doctor Selected"), tr("Please select a doctor to update."));
        return;
    }

    // 收集表单数据并进行基本验证
    QString name = ui->lineEdit_Name->text().trimmed();
    int age = ui->spinBox_Age->value();
    QString certificateNumber = ui->lineEdit_CertificateNumber->text().trimmed();
    DoctorInfo::Sex sex = (ui->radioButton_Male->isChecked()) ? DoctorInfo::MALE : DoctorInfo::FEMALE;

    if (name.isEmpty() || certificateNumber.isEmpty()) {
        QMessageBox::warning(this, tr("Incomplete Data"), tr("Please fill in all required fields before updating a doctor."));
        return;
    }

    // 假设 doctors 是一个 QVector<DoctorInfo> 并且它是有序的或有快速查找机制
    auto it = std::find_if(doctors.begin(), doctors.end(), [this](const DoctorInfo& doctor) {
        return doctor.id == currentDoctorId;
    });

    if (it != doctors.end()) {
        // 更新医生信息
        it->name = name;
        it->age = age;
        it->certificateNumber = certificateNumber;
        it->sex = sex;

        loadDoctorsToTable(true); // 刷新表格显示
        QMessageBox::information(this, tr("Update Successful"), tr("The doctor information has been updated successfully."));
        clearForm(); // 清空表单字段
    } else {
        qWarning() << "Failed to find the doctor with ID:" << currentDoctorId;
    }
}

void DoctorView::on_btn_Delete_clicked()
{
    qDebug() << "Current doctor ID for update:" << currentDoctorId;
    if (currentDoctorId > 0) {
        doctors.erase(std::remove_if(doctors.begin(), doctors.end(),
                                     [this](const DoctorInfo &doctor) { return doctor.id == currentDoctorId; }),
                      doctors.end());

        loadDoctorsToTable(true); // 更新表格显示
        clearForm(); // 清空表单字段

        qDebug() << "医生删除成功" << currentDoctorId;
    } else {
        qWarning() << "Please select a doctor to delete.";
    }
}

void DoctorView::on_tableDoctors_itemSelectionChanged()
{
    // 获取选中的行
    QList<QTableWidgetItem*> selectedItems = ui->tableDoctors->selectedItems();
    if (selectedItems.isEmpty()) {
        currentDoctorId = -1;
        clearForm();
        qDebug() << "No doctor selected.";
        return;
    }

    int row = selectedItems.first()->row();
    qDebug() << "Selected row:" << row;

    // 检查行号有效性
    if (row < 0 || row >= ui->tableDoctors->rowCount()) {
        qWarning() << "Invalid row index:" << row;
        currentDoctorId = -1;
        clearForm();
        return;
    }

    // 获取ID列的数据，并解析为纯文本
    QTableWidgetItem* idItem = ui->tableDoctors->item(row, 0);
    if (!idItem) {
        qWarning() << "ID item is null for selected row:" << row;
        currentDoctorId = -1;
        clearForm();
        return;
    }

    QVariant dataVariant = idItem->data(Qt::UserRole + 1);
    QString idStr;
    if (dataVariant.isValid()) {
        QTextDocument doc;
        doc.setHtml(dataVariant.toString());
        idStr = doc.toPlainText().trimmed();
    } else {
        idStr = idItem->text().trimmed();
    }

    bool ok;
    currentDoctorId = idStr.toInt(&ok);
    if (!ok || currentDoctorId <= 0) {
        qWarning() << "Invalid ID for selected row:" << row;
        currentDoctorId = -1;
        clearForm();
        return;
    }

    qDebug() << "Selected doctor ID:" << currentDoctorId;

    // 尝试直接使用 doctors 容器来获取医生信息，避免重复解析表格数据
    auto it = std::find_if(doctors.begin(), doctors.end(), [this](const DoctorInfo& doctor) {
        return doctor.id == currentDoctorId;
    });

    if (it != doctors.end()) {
        const DoctorInfo& doctor = *it;

        // 设置表单字段
        ui->lineEdit_Name->setText(doctor.name);
        ui->spinBox_Age->setValue(doctor.age);
        ui->lineEdit_CertificateNumber->setText(doctor.certificateNumber);

        // 根据医生性别选择对应的radio button
        if (doctor.sex == DoctorInfo::MALE) {
            ui->radioButton_Male->setChecked(true);
        } else if (doctor.sex == DoctorInfo::FEMALE) {
            ui->radioButton_Female->setChecked(true);
        }
        qDebug() << "Set sex to:" << (ui->radioButton_Male->isChecked() ? "Male" : "Female");

    } else {
        qWarning() << "Failed to find the doctor with ID:" << currentDoctorId;
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

        loadDoctorsToTable(true); // 更新表格显示
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
        case 3: return sortAscending ? a.age < b.age : a.age > b.age;
        case 4: return sortAscending ? a.certificateNumber.compare(b.certificateNumber, Qt::CaseInsensitive) < 0 : a.certificateNumber.compare(b.certificateNumber, Qt::CaseInsensitive) > 0;
        default: return false;
        }
    });

    loadDoctorsToTable(true);
}


void DoctorView::on_btnImport_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) {
        return; // 用户取消选择
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for reading.");
        return;
    }

    // 设置 QTextStream 使用 UTF-8 编码
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8); // 使用 setEncoding 设置编码为 UTF-8

    QVector<DoctorInfo> importedDoctors;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        if (fields.size() >= 5) { // 假设 CSV 文件至少有 5 列
            bool ok;
            int id = fields[0].toInt(&ok);
            if (!ok) continue; // 如果转换失败，跳过该行

            DoctorInfo doctor;
            doctor.id = id;
            doctor.name = fields[1];
            doctor.sex = DoctorInfo::sexFromString(fields[2]); // 使用 sexFromString 转换性别
            bool ageOk;
            doctor.age = fields[3].toInt(&ageOk);
            if (!ageOk) continue; // 如果转换失败，跳过该行

            doctor.certificateNumber = fields[4];

            importedDoctors.append(doctor);
        }
    }

    file.close();

    // 插入导入的数据到数据库
    IDatabase &database = IDatabase::getInstance();
    foreach (const DoctorInfo &doctor, importedDoctors) {
        if (!database.addNewDoctor(doctor)) {
            qCritical() << "Failed to add doctor:" << doctor.name;
        }
    }

    // 重新加载表格显示最新数据
    loadDoctorsFromDatabase();
}




void DoctorView::on_btnExport_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("CSV Files (*.csv)"));
    if (fileName.isEmpty()) {
        return; // 用户取消选择
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file for writing.");
        return;
    }

    // 设置 QTextStream 使用 UTF-8 编码，并写入 BOM
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8); // 使用 setEncoding 设置编码为 UTF-8
    out << "\uFEFF";

    out << "ID,Name,Sex,Age,Certificate Number\n";

    for (const auto &doctor : doctors) {
        out << QString("%1,%2,%3,%4,%5\n").arg(doctor.id).arg(doctor.name)
        .arg(DoctorInfo::sexToString(doctor.sex)).arg(doctor.age)
            .arg(doctor.certificateNumber);
    }

    file.close();

    QMessageBox::information(this, "Success", "Doctors' information has been exported successfully.");
}

