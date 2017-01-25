#include "itemtable.h"

ItemTable::ItemTable(QWidget *parent)
    : WidgetElement(parent)
{
    tableWidget = new QTableWidget(this);

    QStringList headers;
    headers << "ITEMID" << "Fabriquant" << "Numéro de pièce" << "Quantité" << "Identifiant";

    tableWidget->setColumnCount(5);
    tableWidget->setColumnHidden(0, true);
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QRect size = QRect(0,0,720,340);
    tableWidget->setGeometry(size);
}

ItemTable::~ItemTable()
{

}

void ItemTable::addItem(const QString itemId, const QString manufacturerCode, const QString partNumber, const QString quantity)
{
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, new QTableWidgetItem(itemId));
    tableWidget->setItem(row, 1, new QTableWidgetItem(manufacturerCode));
    tableWidget->setItem(row, 2, new QTableWidgetItem(partNumber));
    tableWidget->setItem(row, 3, new QTableWidgetItem(quantity));

    emit itemAdded();
}

void ItemTable::addItem(const QString itemId, const QString manufacturerCode, const QString partNumber, const QString quantity, const QString uniqueIdentifier)
{
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 0, new QTableWidgetItem(itemId));
    tableWidget->setItem(row, 1, new QTableWidgetItem(manufacturerCode));
    tableWidget->setItem(row, 2, new QTableWidgetItem(partNumber));
    tableWidget->setItem(row, 3, new QTableWidgetItem(quantity));
    tableWidget->setItem(row, 4, new QTableWidgetItem(uniqueIdentifier));

    emit itemAdded();
}

void ItemTable::addComment(const QString comment)
{
    int row = tableWidget->rowCount();
    tableWidget->insertRow(row);
    tableWidget->setItem(row, 1, new QTableWidgetItem(comment));
    tableWidget->setSpan(row, 1, 1, 4);

    emit commentAdded();
}

int ItemTable::rowCount()
{
    return tableWidget->rowCount();
}

