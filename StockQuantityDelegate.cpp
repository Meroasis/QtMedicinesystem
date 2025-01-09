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
