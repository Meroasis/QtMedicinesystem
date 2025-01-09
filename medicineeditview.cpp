#include "medicineeditview.h"
#include "ui_medicineeditview.h"
#include "idatabase.h"
#include <QMessageBox>
Medicineeditview::Medicineeditview(QWidget *parent ,int rowNo )
    : QWidget(parent)
    , ui(new Ui::Medicineeditview)
{
    ui->setupUi(this);

    dataMapper = new QDataWidgetMapper();
    QSqlTableModel *model = IDatabase::getInstance().medicineModel;


    dataMapper->setModel(IDatabase::getInstance().medicineModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    // 设置映射关系
    dataMapper->addMapping(ui->dbEditID, model->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, model->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditDosage, model->fieldIndex("DOSAGE"));
    dataMapper->addMapping(ui->spinBoxStock, model->fieldIndex("STOCK_QUANTITY"));
    dataMapper->addMapping(ui->dbcreatedTimestamp, model->fieldIndex("CREATED_TIMESTAMP"));

    dataMapper->setCurrentIndex(rowNo);
}

Medicineeditview::~Medicineeditview()
{
    delete ui;
}

void Medicineeditview::on_BtnSave_clicked()
{
    if (dataMapper->submit()) {
        IDatabase::getInstance().submitMedicineEdit();
        emit goPreviousView();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save changes."));
    }
}


void Medicineeditview::on_BtnCancel_clicked()
{
    emit goPreviousView();
}

