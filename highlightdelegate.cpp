#include "highlightdelegate.h"

HighlightDelegate::HighlightDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void HighlightDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::UserRole + 1);
    if (data.isValid()) {
        QString html = data.toString();
        QTextDocument doc;
        doc.setHtml(html);
        doc.setTextWidth(option.rect.width());

        painter->save();
        painter->translate(option.rect.topLeft());
        QRectF rect(0, 0, option.rect.width(), option.rect.height());
        doc.drawContents(painter, rect);
        painter->restore();
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize HighlightDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::UserRole + 1);
    if (data.isValid()) {
        QTextDocument doc;
        doc.setHtml(data.toString());
        doc.setTextWidth(option.rect.width());
        return QSize(doc.idealWidth(), doc.size().height());
    } else {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}
