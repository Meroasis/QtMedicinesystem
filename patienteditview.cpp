#include "patienteditview.h"
#include "ui_patienteditview.h"
#include <QDataWidgetMapper>
#include <QSqlTableModel>
#include "idatabase.h"
PatientEditView::PatientEditView(QWidget *parent,int rowNo)
    : QWidget(parent)
    , ui(new Ui::PatientEditView)
{
    ui->setupUi(this);
    dataMapper =new QDataWidgetMapper();
    QSqlTableModel *tabModel = IDatabase::getInstance().patientTabModel;
    dataMapper->setModel(IDatabase::getInstance().patientTabModel);
    dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbEditIDcard,tabModel->fieldIndex("ID_CARD"));
    dataMapper->addMapping(ui->dbspinHeight,tabModel->fieldIndex("HEIGHT"));
    dataMapper->addMapping(ui->dbspinweight,tabModel->fieldIndex("WEIGHT"));
    dataMapper->addMapping(ui->dbEditMobile,tabModel->fieldIndex("MOBILEPHONE"));
    dataMapper->addMapping(ui->dbDateEditDOB, tabModel->fieldIndex("D0B"));
    dataMapper->addMapping(ui->dbcombosex, tabModel->fieldIndex("SEX"));
    dataMapper->addMapping(ui->dbcreatedrimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));

    dataMapper->setCurrentIndex(rowNo);

}

PatientEditView::~PatientEditView()
{
    delete ui;
}

void PatientEditView::on_pushButton_clicked()
{
    IDatabase::getInstance().submitPatientEdit();
    emit goPreviousView();
}


void PatientEditView::on_pushButton_2_clicked()
{
    IDatabase::getInstance().reverPatientEidt();
    emit goPreviousView();
}

