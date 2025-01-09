#include "medicineview.h"
#include "ui_medicineview.h"
#include "idatabase.h"
#include <QMessageBox>
#include <QFileDialog>

#include <QStyledItemDelegate>
#include <QPainter>

class StockQuantityDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if (index.column() == 3) { // 假设库存数量列是第3列（索引为2）
            int quantity = index.model()->data(index, Qt::DisplayRole).toInt();
            if (quantity < 10) {
                QStyleOptionViewItem opt = option;
                opt.palette.setColor(QPalette::Text, Qt::red);
                QStyledItemDelegate::paint(painter, opt, index);
                return;
            }
        }
        QStyledItemDelegate::paint(painter, option, index);
    }
};






MedicineView::MedicineView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MedicineView)
{
    ui->setupUi(this);

    // 设置表格视图的行为
    ui->myTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->myTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->myTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->myTableView->setAlternatingRowColors(true);

    IDatabase &iDatabase = IDatabase::getInstance();
    if (!iDatabase.initMedicineModel()) {
        qCritical() << "Failed to initialize medicine model.";
        return;
    }

    QSqlTableModel *medicineModel = iDatabase.medicineModel;
    if (!medicineModel) {
        qCritical() << "Medicine model is not initialized.";
        return;
    }

    // 创建 QSortFilterProxyModel 并设置源模型
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(medicineModel);



    // 显式连接信号和槽

    // 设置表格视图的模型和选择模型
    ui->myTableView->setModel(proxyModel);
    ui->myTableView->setSelectionModel(new QItemSelectionModel(proxyModel, this));

    ui->myTableView->resizeColumnsToContents();

    // 启用表头点击排序
    ui->myTableView->horizontalHeader()->setSectionsClickable(true);
    ui->myTableView->horizontalHeader()->setSortIndicatorShown(true);

    // 显式连接信号和槽
    connect(ui->myTableView->horizontalHeader(), &QHeaderView::sectionClicked,
            this, &MedicineView::handleHorizontalHeaderSectionClicked);

    // 连接选择变化信号以更新按钮状态
    connect(ui->myTableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MedicineView::updateDeleteButtonState);
    // 初始禁用删除按钮
    updateDeleteButtonState();


    StockQuantityDelegate *delegate = new StockQuantityDelegate(this);
    ui->myTableView->setItemDelegateForColumn(3, delegate);
}

MedicineView::~MedicineView()
{
    delete ui;
}

void MedicineView::updateDeleteButtonState()
{
    bool hasSelection = !ui->myTableView->selectionModel()->selection().isEmpty();
    ui->BtnDelete->setEnabled(hasSelection);
}

void MedicineView::on_BtnFind_clicked()
{
    QRegularExpression filter;
    if (!ui->lineEdit->text().isEmpty()) {
        QString pattern = ui->lineEdit->text();
        filter.setPattern(QRegularExpression::escape(pattern) + ".*");
        filter.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    // 设置代理模型的过滤规则
    proxyModel->setFilterRegularExpression(filter);
    proxyModel->setFilterKeyColumn(-1); // -1 表示应用到所有列
}

void MedicineView::on_BtnAdd_clicked()
{
    int currow = IDatabase::getInstance().addMedicine();
    emit goMedicineEditView(currow);
}

void MedicineView::on_BtnDelete_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to delete the selected medicine?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        IDatabase::getInstance().deleteMedicine(ui->myTableView->selectionModel());
    }
}

void MedicineView::on_BtnChange_clicked()
{
    QModelIndex curIndex = ui->myTableView->selectionModel()->currentIndex();
    if (!curIndex.isValid()) {
        QMessageBox::information(this, tr("Information"), tr("Please select a medicine to edit."));
        return;
    }

    int sourceRow = proxyModel->mapToSource(curIndex).row();
    emit goMedicineEditView(sourceRow);
}

void MedicineView::handleHorizontalHeaderSectionClicked(int logicalIndex)
{
    if (logicalIndex == sortColumn) {
        sortAscending = !sortAscending;
    } else {
        sortColumn = logicalIndex;
        sortAscending = true;
    }

    if (proxyModel) {
        proxyModel->sort(logicalIndex, sortAscending ? Qt::AscendingOrder : Qt::DescendingOrder);
    }
}

void MedicineView::on_actionImportCSV_clicked()
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
    QSqlTableModel *model = IDatabase::getInstance().medicineModel;

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


void MedicineView::on_actionExportCSV_clicked()
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

    QSqlTableModel *model = IDatabase::getInstance().medicineModel;

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



