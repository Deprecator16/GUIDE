#include "detailedinventory.h"
#include "ui_detailedinventory.h"

#include <QtSql>
#include <QDebug>

DetailedInventory::DetailedInventory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DetailedInventory)
{
    ui->setupUi(this);

    ui->treeWidget->setColumnCount(1);

    connect(ui->ok_PushButton, SIGNAL(clicked()),
            this, SLOT(accept()));
}

DetailedInventory::~DetailedInventory()
{
    delete ui;
}

void DetailedInventory::getItem(QString manufacturer, QString partNumber) {
    QString manu = "'" + manufacturer + "'";
    QString pn = "'" + partNumber + "'";
    showDetails(manu, pn);
}

void DetailedInventory::showDetails(QString manufacturer, QString partNumber) {
    QSqlQuery inventoryQuery;
    QString inventoryQueryString = QString("SELECT stock_quantity, unique_identifier_enabled, unique_identifier "
                                           "FROM inventory "
                                           "WHERE manufacturer_code = %1 AND part_number = %2 ")
                                           .arg(manufacturer).arg(partNumber);
    inventoryQuery.exec(inventoryQueryString);

    qDebug() << inventoryQuery.lastError();

    inventoryQuery.first();
    QString stock = inventoryQuery.value(0).toString();
    int identifierEnabled = inventoryQuery.value(1).toInt();
    QString identifier = inventoryQuery.value(2).toString();

    QTreeWidgetItem *inventory = new QTreeWidgetItem(ui->treeWidget);
    inventory->setText(0, "Inventory");
    if (identifierEnabled == 1) {
        QStringList identifierList = identifier.split(",");
        for (int i = 0; i < identifierList.length(); i++) {
            QTreeWidgetItem *id = new QTreeWidgetItem();
            id->setText(0, identifierList.at(i));
            inventory->addChild(id);
        }
    }
    else {
        QTreeWidgetItem *quantity = new QTreeWidgetItem();
        quantity->setText(0, stock);
        inventory->addChild(quantity);
    }

    QSqlQuery allocatedQuery;
    QString allocationQueryString = QString("SELECT allocation_type, reference, allocated_identifier, allocated_quantity "
                                            "FROM allocated "
                                            "WHERE manufacturer_code = %1 AND part_number = %2 ")
                                            .arg(manufacturer).arg(partNumber);
    allocatedQuery.exec(allocationQueryString);

    QStringList typeList;
    QStringList referenceList;
    QStringList allocatedIdentifierList;
    QStringList allocatedList;
    while (allocatedQuery.next()) {
        typeList << allocatedQuery.value(0).toString();
        referenceList << allocatedQuery.value(1).toString();
        allocatedIdentifierList << allocatedQuery.value(2).toString();
        allocatedList << allocatedQuery.value(3).toString();
    }
    int e = 0;
    for (int i = 0; i < typeList.length(); i++) {
        QTreeWidgetItem *submit = 0;
        int type = typeList.at(i).toInt();
        QString reference = referenceList.at(i);
        QStringList allocatedIdentifier = allocatedIdentifierList.at(i).split(",");
        QString allocated = allocatedList.at(i);

        if (type == 1) {
            QTreeWidgetItem *technician = new QTreeWidgetItem(ui->treeWidget);
            technician->setText(0, reference);

            if (identifierEnabled == 1) {
                for (int j = 0; j < allocatedIdentifier.length(); j++) {
                    QTreeWidgetItem *id = new QTreeWidgetItem();
                    id->setText(0, allocatedIdentifier.at(j));
                    technician->addChild(id);
                }
            }
            else {
                QTreeWidgetItem *quantity = new QTreeWidgetItem();
                quantity->setText(0, allocated);
                technician->addChild(quantity);
            }
        }
        else {
            // POUR LES CEUSES QUI FAUT QUE C'EST SUBMIT ALLOCATE
            if (e == 0) {
                submit = new QTreeWidgetItem(ui->treeWidget);
                submit->setText(0, "Submit");
            }

            if (identifierEnabled == 1) {
                for (int j = 0; j < allocatedIdentifier.length(); j++) {
                    QTreeWidgetItem *ref = new QTreeWidgetItem();
                    ref->setText(0, reference);
                    submit->addChild(ref);
                    QTreeWidgetItem *id = new QTreeWidgetItem();
                    id->setText(0, allocatedIdentifier.at(j));
                    ref->addChild(id);
                }
            }
            else {
                QTreeWidgetItem *ref = new QTreeWidgetItem();
                ref->setText(0, reference);
                submit->addChild(ref);
                QTreeWidgetItem *quantity = new QTreeWidgetItem();
                quantity->setText(0, allocated);
                ref->addChild(quantity);
            }
            e++;
        }
    }
}
