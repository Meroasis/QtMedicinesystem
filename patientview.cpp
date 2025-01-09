#include "patientview.h"
#include "ui_patientview.h"
#include "idatabase.h"
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QFileDialog>
PatientView::PatientView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PatientView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);
    IDatabase &iDatabase = IDatabase::getInstance();
    if (iDatabase.initPatientModel()) {
        QSqlTableModel *patientTabModel = iDatabase.patientTabModel;

        // 创建 QSortFilterProxyModel 并设置源模型
        proxyModel = new QSortFilterProxyModel(this);
        proxyModel->setSourceModel(patientTabModel);

        // 设置表格视图的模型和选择模型
        ui->tableView->setModel(proxyModel);
        ui->tableView->setSelectionModel(new QItemSelectionModel(proxyModel));

        ui->tableView->resizeColumnsToContents();

        // 启用表头点击排序
        ui->tableView->horizontalHeader()->setSectionsClickable(true);
        ui->tableView->horizontalHeader()->setSortIndicatorShown(true);
        connect(ui->tableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &PatientView::on_tablePatients_horizontalHeader_clicked);

        // 连接选择变化信号以更新按钮状态
        connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PatientView::updateDeleteButtonState);

        // 初始禁用删除按钮
        updateDeleteButtonState();
    }
}

PatientView::~PatientView()
{
    delete ui;
    delete proxyModel;
}

void PatientView::on_BtnAdd_clicked()
{
    int currow = IDatabase::getInstance().addNewPatient();
    emit goPatientEditView(currow );
}


void PatientView::on_BtnFind_clicked()
{
    QString filter = QString("name like '%%1%'").arg(ui->lineEdit->text());
    IDatabase::getInstance().searchPatient(filter);
}


void PatientView::on_BtnDelete_clicked()
{

    // 调用删除当前病人的函数并检查返回值
    IDatabase::getInstance().deleteCurrentPatient(ui->tableView->selectionModel());

}


void PatientView::updateDeleteButtonState()
{
    // 检查是否有一行被选中
    bool hasSelection = !ui->tableView->selectionModel()->selection().isEmpty();
    // 根据是否有选中项来启用或禁用删除按钮
    ui->BtnDelete->setEnabled(hasSelection);
}


void PatientView::on_BtnChange_clicked()
{
    QModelIndex curIndex = ui->tableView->selectionModel()->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::information(this, tr("Information"), tr("Please select a patient to edit."));
        return;
    }

    int sourceRow = proxyModel->mapToSource(curIndex).row();

    // 发射信号，传递源模型的行号
    emit goPatientEditView(sourceRow);
}

void PatientView::on_tablePatients_horizontalHeader_clicked(int logicalIndex)
{
    if (logicalIndex == sortColumn) {
        sortAscending = !sortAscending;
    } else {
        sortColumn = logicalIndex;
        sortAscending = true;
    }

    // 设置代理模型的排序规则
    if (proxyModel) { // 检查 proxyModel 是否有效
        proxyModel->sort(logicalIndex, sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
    }
}



void PatientView::on_actionImportCSV_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("CSV Files (*.csv)"));
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for reading."));
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::System);
    QStringList headers;
    QSqlTableModel *model = IDatabase::getInstance().patientTabModel;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // 如果是第一行，则假设为表头
        if (headers.isEmpty()) {
            headers = fields;
            continue;
        }

        // 插入新行并设置数据
        int row = model->rowCount();
        model->insertRow(row);

        for (int i = 0; i < fields.size() && i < headers.size(); ++i) {
            int column = model->fieldIndex(headers[i]);
            if (column != -1) {
                model->setData(model->index(row, column), fields[i].trimmed());
            }
        }
    }

    file.close();

    // 提交所有更改到数据库
    if (model->submitAll()) {
        QMessageBox::information(this, tr("Success"), tr("Data imported successfully."));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to import data."));
    }
}


void PatientView::on_actionExportCSV_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save CSV File"), "", tr("CSV Files (*.csv)"));
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file for writing."));
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::System);


    QSqlTableModel *model = IDatabase::getInstance().patientTabModel;

    // 写入表头
    QStringList headers;
    for (int col = 0; col < model->columnCount(); ++col) {
        headers.append(model->headerData(col, Qt::Horizontal).toString());
    }
    out << headers.join(",") << "\n";

    // 写入数据行
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList fields;
        for (int col = 0; col < model->columnCount(); ++col) {
            fields.append(model->data(model->index(row, col)).toString());
        }
        out << fields.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("Success"), tr("Data exported successfully."));
}

