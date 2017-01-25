#include "selectiondialog.h"
#include "ui_selectiondialog.h"

#include <QDebug>

SelectionDialog::SelectionDialog(QStringList headers, QStringList list1, QStringList list2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectionDialog),
    m_list1(list1), m_list2(list2), m_headers(headers)
{
    ui->setupUi(this);

    createTable();

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT(sendSelectedItem(int,int)));
}

SelectionDialog::~SelectionDialog()
{
    delete ui;
}

void SelectionDialog::createTable() const
{
    ui->tableWidget->setColumnCount(m_headers.length());
    ui->tableWidget->setColumnWidth(1, 100);
    ui->tableWidget->setColumnWidth(2, 300);
    ui->tableWidget->setHorizontalHeaderLabels(m_headers);

    int size = m_list1.size();
    ui->tableWidget->setRowCount(size);

    for (int i = 0; i < size; i++) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(m_list1.at(i)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(m_list2.at(i)));
    }
}

void SelectionDialog::sendSelectedItem(int r, int c) {
    Q_UNUSED(c)

    value = ui->tableWidget->item(r, 0)->text();
    getValue();

    QDialog::accept();
}

QString SelectionDialog::getValue()
{
    return  value;
}
