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
    dataMapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);

    dataMapper->addMapping(ui->dbEditID, tabModel->fieldIndex("ID"));
    dataMapper->addMapping(ui->dbEditIDcard,tabModel->fieldIndex("ID_CARD"));
    dataMapper->addMapping(ui->dbEditName, tabModel->fieldIndex("NAME"));
    dataMapper->addMapping(ui->dbcombosex, tabModel->fieldIndex("SEX"));
    dataMapper->addMapping(ui->dbspinHeight,tabModel->fieldIndex("HEIGHT"));
    dataMapper->addMapping(ui->dbspinweight,tabModel->fieldIndex("WEIGHT"));
    dataMapper->addMapping(ui->dbEditMobile,tabModel->fieldIndex("MOBILEPHONE"));
    dataMapper->addMapping(ui->dbDateEditDOB, tabModel->fieldIndex("BIRTHDAY"));

    dataMapper->addMapping(ui->dbcreatedrimeStamp, tabModel->fieldIndex("CREATEDTIMESTAMP"));
    dataMapper->setCurrentIndex(rowNo);
    connect(ui->dbDateEditDOB, &QDateEdit::dateChanged, this, &PatientEditView::updateAge);
    updateAge();

}

PatientEditView::~PatientEditView()
{
    delete ui;
}



void PatientEditView::on_pushButton_clicked()
{
    dataMapper->submit();
    IDatabase::getInstance().submitPatientEdit();
    emit goPreviousView();
}


void PatientEditView::on_pushButton_2_clicked()
{
    // IDatabase::getInstance().reverPatientEidt();
    emit goPreviousView();
}

void PatientEditView::updateAge() {
    QDate birthday = ui->dbDateEditDOB->date();
    if (birthday.isValid()) {
        int age = calculateAge(birthday);
        // 获取当前行号
        int currentRow = dataMapper->currentIndex();
        QSqlTableModel *model = qobject_cast<QSqlTableModel*>(dataMapper->model());
        if (!model) {
            qWarning() << "Model is not a QSqlTableModel.";
            return;
        }
        // 更新 AGE 字段
        int ageColumn = model->fieldIndex("AGE");
        if (ageColumn != -1) {
            QModelIndex index = model->index(currentRow, ageColumn);
            model->setData(index, age);
        }
    }
}
int PatientEditView::calculateAge(const QDate &birthDate)
{
    QDate currentDate = QDate::currentDate();
    int age = currentDate.year() - birthDate.year();

    if (currentDate.month() < birthDate.month() ||
        (currentDate.month() == birthDate.month() && currentDate.day() < birthDate.day())) {
        age--;
    }

    return age;
}



