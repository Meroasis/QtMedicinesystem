#include "roomview.h"
#include "ui_roomview.h"

RoomView::RoomView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomView)
{
    ui->setupUi(this);
}

RoomView::~RoomView()
{
    delete ui;
}
