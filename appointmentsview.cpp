#include "appointmentsview.h"
#include "ui_appointmentsview.h"
#include "idatabase.h"
#include "QFileDialog"
#include "QMessageBox"
AppointmentsView::AppointmentsView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppointmentsView)
{
    ui->setupUi(this);
    IDatabase::getInstance().initDatabase();
    IDatabase::getInstance().initPatientModel();
    IDatabase::getInstance().initDoctorModel();
    IDatabase::getInstance().initMedicineModel();
    // 加载数据到组合框
    populateComboBoxes();
}

AppointmentsView::~AppointmentsView()
{
    delete ui;
}

void AppointmentsView::populateComboBoxes()
{
    QSqlDatabase db = IDatabase::getInstance().database;

    // 病人选择框
    QSqlQueryModel *modelPatients = new QSqlQueryModel(this);
    modelPatients->setQuery("SELECT id, name FROM patient ORDER BY name", db);
    ui->comboboxPatient->setModel(modelPatients);
    ui->comboboxPatient->setModelColumn(1);

    // 医生选择框
    QSqlQueryModel *modelDoctors = new QSqlQueryModel(this);
    modelDoctors->setQuery("SELECT id, name FROM Doctor ORDER BY name", db);
    ui->comboboxDoctor->setModel(modelDoctors);
    ui->comboboxDoctor->setModelColumn(1);

    // 药品选择框
    QSqlQueryModel *modelMedicines = new QSqlQueryModel(this);
    modelMedicines->setQuery("SELECT id, name FROM Medicines ORDER BY name", db);
    ui->comboboxMedicines->setModel(modelMedicines);
    ui->comboboxMedicines->setModelColumn(1); // 显示第二列（name）
}

void AppointmentsView::collectSelectionData(QString &patientName, QString &doctorName, QString &medicineName)
{

    patientName = ui->comboboxPatient->currentText();
    // 获取医生信息
    doctorName = ui->comboboxDoctor->currentText();
    // 获取药品信息
    medicineName = ui->comboboxMedicines->currentText();

}

QString AppointmentsView::getTextDescription()
{
    return ui->texteditDiagnosis->toPlainText();
}

bool AppointmentsView::exportSelectionToFile(const QString &filePath)
{
    QString patientName;
    QString doctorName;
    QString medicineName;


    collectSelectionData(patientName, doctorName, medicineName);
    QString description = getTextDescription();

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to open file for writing:" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::System);
    out << "Patient: " << patientName  << "\n";
    out << "Doctor: " << doctorName <<  "\n";
    out << "Medicine: " << medicineName << "\n";
    out << "Description:\n" << description << "\n";

    file.close();
    qDebug() << "Data exported successfully to" << filePath;
    return true;
}



void AppointmentsView::on_btnsave_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Data"), "", tr("Text Files (*.txt);;All Files (*)"));
    if (!filePath.isEmpty()) {
        if (!exportSelectionToFile(filePath)) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to export data."));
        } else {
            QMessageBox::information(this, tr("Success"), tr("Data exported successfully."));
        }
    }
}

