#include "inventorytransfer.h"
#include "ui_inventorytransfer.h"

#include "selectiondialog.h"
#include "PromotedClass/itemtable.h"
#include "PromotedClass/itemwidget.h"

#include <QCompleter>
#include <QValidator>

/*
 *  Inventory Transfer
 */

InventoryTransfer::InventoryTransfer(QWidget *parent) :
    DialogElement(parent),
    ui(new Ui::InventoryTransfer)
{
    ui->setupUi(this);

    ui->fromName_LineEdit->setReadOnly(true);
    ui->toName_LineEdit->setReadOnly(true);
    ui->date_dateEdit->setReadOnly(true);
    ui->transfer_LineEdit->setReadOnly(true);
    ui->allocated_LineEdit->setReadOnly(true);

    ui->to_LineEdit->installEventFilter(this);

    ui->date_dateEdit->setDate(QDate::currentDate());

    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->release_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(ui->from_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkFromAccount()));
    connect(ui->to_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkToAccount()));
    connect(ui->itemWidget, SIGNAL(passFocus()),
            ui->quantity_LineEdit, SLOT(setFocus()));
}

InventoryTransfer::~InventoryTransfer()
{
    delete ui;
}

void InventoryTransfer::checkFromAccount()
{
    if (!ui->from_LineEdit->text().isEmpty())
        getAccountInformation(0, ui->from_LineEdit->text());
}

void InventoryTransfer::checkToAccount()
{
    if (!ui->to_LineEdit->text().isEmpty())
        getAccountInformation(1, ui->to_LineEdit->text());
}

void InventoryTransfer::getAccountInformation(const int sender, const QString account)
{
    /*
     * Sender :
     * 0 - From
     * 1 - To
     */

    /*
     * Type :
     * 0 - Tech
     * 1 - Site
     */

    QSqlDatabase::database().transaction();

    QSqlQuery checkTechnician;
    QString checkTechnicianString = QString("SELECT technician_id, technician_name "
                                            "FROM technician "
                                            "WHERE technician_id = %1 OR technician_name LIKE %2")
                                            .arg(toQueryTextString(account)).arg(toQueryLikeString(account));
    checkTechnician.exec(checkTechnicianString);

    QSqlQuery checkSite;
    QString checkSiteString = QString("SELECT site_code, name "
                                      "FROM site "
                                      "WHERE site_code = %1 OR name LIKE %2")
                                      .arg(toQueryTextString(account)).arg(toQueryLikeString(account));
    checkSite.exec(checkSiteString);

    QSqlDatabase::database().commit();

    QStringList techIdList;
    QStringList techNameList;
    while (checkTechnician.next())
    {
        techIdList << checkTechnician.value(0).toString();
        techNameList << checkTechnician.value(1).toString();
    }

    QStringList siteCodeList;
    QStringList siteNameList;
    while (checkSite.next())
    {
        siteCodeList << checkSite.value(0).toString();
        siteNameList << checkSite.value(1).toString();
    }

    if (techIdList.count() == 1 && siteCodeList.count() == 0)
    {
        if (sender == 0)
        {
            ui->from_LineEdit->setText(techIdList.at(0));
            ui->fromName_LineEdit->setText(techNameList.at(0));
            fromInventory_m = "TECH_" + techIdList.at(0);
            fromType = 0;
        }
        else
        {
            ui->to_LineEdit->setText(techIdList.at(0));
            ui->toName_LineEdit->setText(techNameList.at(0));
            toInventory_m = "TECH_" + techIdList.at(0);
            toType = 0;
        }
    }
    else if (techIdList.count() == 0 && siteCodeList.count() == 1)
    {
        if (sender == 0)
        {
            ui->from_LineEdit->setText(siteCodeList.at(0));
            ui->fromName_LineEdit->setText(siteNameList.at(0));
            fromInventory_m = "SITE_" + siteCodeList.at(0);
            fromType = 1;
        }
        else
        {
            ui->to_LineEdit->setText(siteCodeList.at(0));
            ui->toName_LineEdit->setText(siteNameList.at(0));
            toInventory_m = "SITE_" + siteCodeList.at(0);
            toType = 1;
        }
    }
    else if (techIdList.count() == 0 && siteCodeList.count() == 0)
    {
        if (sender == 0)
        {
            ui->from_LineEdit->setFocus();
            ui->from_LineEdit->selectAll();
        }
        else
        {
            ui->to_LineEdit->setFocus();
            ui->to_LineEdit->selectAll();
        }
        IVAMessageBox("Technicien ou site");
    }
    else
    {
        QStringList idList;
        idList.append(techIdList);
        idList.append(siteCodeList);

        QStringList nameList;
        nameList.append(techNameList);
        nameList.append(siteNameList);

        QStringList headers;
        headers << "Number" << "Name";
        SelectionDialog *selection = new SelectionDialog(headers, idList, nameList, this);
        selection->show();

        QString id;
        if (selection->exec() == QDialog::Accepted)
            id = selection->getValue();

        if (techIdList.contains(id))
        {
            int pos = techIdList.indexOf(id);

            if (sender == 0)
            {
                ui->from_LineEdit->setText(techIdList.at(pos));
                ui->fromName_LineEdit->setText(techNameList.at(pos));
                fromInventory_m = "TECH_" + techIdList.at(pos);
                fromType = 0;
            }
            else
            {
                ui->to_LineEdit->setText(techIdList.at(pos));
                ui->toName_LineEdit->setText(techNameList.at(pos));
                toInventory_m = "TECH_" + techIdList.at(pos);
                toType = 0;
            }
        }
        else if (siteCodeList.contains(id))
        {
            int pos = siteCodeList.indexOf(id);

            if (sender == 0)
            {
                ui->from_LineEdit->setText(siteCodeList.at(pos));
                ui->fromName_LineEdit->setText(siteNameList.at(pos));
                fromInventory_m = "SITE_" + siteCodeList.at(pos);
                fromType = 1;
            }
            else
            {
                ui->to_LineEdit->setText(siteCodeList.at(pos));
                ui->toName_LineEdit->setText(siteNameList.at(pos));
                toInventory_m = "SITE_" + siteCodeList.at(pos);
                toType = 1;
            }
        }
    }
}

bool InventoryTransfer::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->to_LineEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            checkToAccount();
            ui->itemWidget->getFocus();
            return true;
        }
        else
            return false;
    }
    return false;
}

/*
 *  New Inventory Transfer
 */

NewInventoryTransfer::NewInventoryTransfer(QWidget *parent) :
    InventoryTransfer(parent)
{
    ui->release_PushButton->setEnabled(false);

    this->setWindowTitle("Nouveau transfert");

    connect(ui->itemWidget, SIGNAL(validItem(QString,QString,QString,QString,QString)),
            this, SLOT(getAllocatedQuantity(QString,QString,QString,QString,QString)));
    connect(ui->tableWidget, SIGNAL(itemAdded()),
            this, SLOT(clearItemFields()));
    connect(ui->quantity_LineEdit, SIGNAL(returnPressed()),
            this, SLOT(maybeAddItem()));
    connect(ui->itemWidget, SIGNAL(makeUpdateQuantityLabel()),
            this, SLOT(updateQuantityLabel()));
}

NewInventoryTransfer::~NewInventoryTransfer()
{

}

void NewInventoryTransfer::accept()
{
    if (ui->tableWidget->rowCount() > 0)
    {
        QSqlDatabase::database().commit();
        if (QSqlDatabase::database().lastError().isValid())
            QERMessageBox(QSqlDatabase::database().lastError().text());
        else
            QDialog::accept();
    }
    else
        NTRMessageBox();
}

void NewInventoryTransfer::reject()
{
    if (ui->tableWidget->rowCount() > 0)
    {
        QSqlDatabase::database().rollback();
        if (QSqlDatabase::database().lastError().isValid())
            ROEMessageBox();
        else
            QDialog::reject();
    }
    else
        QDialog::reject();
}

void NewInventoryTransfer::closeEvent(QCloseEvent *e)
{
    if (ui->tableWidget->rowCount() > 0)
    {
        QSqlDatabase::database().rollback();
        if (QSqlDatabase::database().lastError().isValid())
        {
            ROEMessageBox();
            e->ignore();
        }
        else
            e->accept();
    }
    else
        e->accept();
}

// List of query from NoUID item
void NewInventoryTransfer::addItem(const QString itemId, const QString line, const QString quantity) const
{
    QStringList queryList;

    // Add to transfered item table
    queryList.append(QString("INSERT INTO transfered "
                             "(item_id, transfer_id, line, quantity, unique_identifier) "
                             "VALUES "
                             "(%1, %2, %3, %4, '') ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(ui->transfer_LineEdit->text()))
                             .arg(toQueryTextString(line)).arg(toQueryTextString(quantity)));

    // Add item to 'to' inventory
    queryList.append(QString("INSERT OR REPLACE INTO %1 "
                             "(item_id, quantity, unique_identifier) "
                             "VALUES "
                             "(%2, "
                                   "(CASE "
                                   "WHEN ((SELECT quantity FROM %1 WHERE item_id = %2) IS NULL) "
                                   "THEN %3 "
                                   "ELSE ((SELECT quantity FROM %1 WHERE item_id = %2) + %3)"
                                   "END), "
                             "'') ")
                             .arg(toQueryTextString(toInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(quantity)));

    // Remove item from 'from' inventory
    queryList.append(QString("UPDATE %1 "
                             "SET "
                             "quantity  = ((SELECT quantity FROM %1 WHERE item_id = %2) - %3) "
                             "WHERE item_id = %2 ")
                             .arg(toQueryTextString(fromInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(quantity)));

    queryList.append(QString("DELETE FROM %1 WHERE item_id = %2 AND quantity = '0'")
                             .arg(toQueryTextString(fromInventory_m)).arg(toQueryTextString(itemId)));

    QString eventType;
    if (toType == 0)
        eventType = "3";
    else if (toType == 1)
        eventType = "5"; // If item is to site

    /*
     * Add quantity to new state
     *
     * *** QUERY MUST BE AN UPSERT ***
     * To ensure history trigger are executed properly
     */

    // If item is to technician
    if (toType == 0)
    {
        queryList.append(QString("UPDATE state "
                                 "SET "
                                 "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND event_type = '3') + %2), "
                                 "date = %3, "
                                 "reference = %4 "
                                 "WHERE item_id = %1 AND event_type = '3' ")
                                 .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                                 .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->transfer_LineEdit->text())));

        queryList.append(QString("INSERT OR IGNORE INTO state "
                                 "(item_id, quantity, date, event_type, unique_identifier, reference) "
                                 "VALUES "
                                 "(%1, %2, %3, '3', '', %4) ")
                                 .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                                 .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->transfer_LineEdit->text())));
    }
    // If item is to site
    else if (toType == 1)
    {
        queryList.append(QString("UPDATE state "
                                 "SET "
                                 "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND is_stock = '0') + %2), "
                                 "date = %3, "
                                 "event_type = '5', "
                                 "reference = %4 "
                                 "WHERE item_id = %1 AND is_stock = '0'")
                                 .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                                 .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->transfer_LineEdit->text())));

        queryList.append(QString("INSERT OR IGNORE INTO state "
                                 "(item_id, quantity, date, event_type, is_stock, unique_identifier, reference) "
                                 "VALUES "
                                 "(%1, %2, %3, '5', '0', '', %4) ")
                                 .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                                 .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->transfer_LineEdit->text())));
    }

    //Remove quantity from old state

    // If item is from technician
    if (fromType == 0)
    {
        queryList.append(QString("UPDATE state "
                                 "SET "
                                 "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND event_type = '3') - %2) "
                                 "WHERE item_id = %1 AND event_type = '3'")
                                 .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity)));

        queryList.append(QString("DELETE FROM state WHERE item_id = %1 AND event_type = '3' AND quantity = '0'")
                                 .arg(toQueryTextString(itemId)));
    }
    // If item is from site
    else if (fromType == 1)
    {
        queryList.append(QString("UPDATE state "
                                 "SET "
                                 "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND is_stock = '0') - %2) "
                                 "WHERE item_id = %1 AND is_stock = '0'")
                                 .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity)));

        queryList.append(QString("DELETE FROM state WHERE item_id = %1 AND is_stock = '0' AND quantity = '0'")
                                 .arg(toQueryTextString(itemId)));
    }

    processItem(queryList, quantity, "");
}

// List of query from UID item transfered in database
void NewInventoryTransfer::addItem(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier) const
{
    QStringList queryList;

    // Add to transfered item table
    queryList.append(QString("INSERT INTO transfered "
                             "(item_id, transfer_id, line, quantity, unique_identifier) "
                             "VALUES "
                             "(%1, %2, %3, %4, %5) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(ui->transfer_LineEdit->text()))
                             .arg(toQueryTextString(line)).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(uniqueIdentifier)));

    // Add to 'to' inventory
    queryList.append(QString("INSERT INTO %1 "
                             "(item_id, quantity, unique_identifier) "
                             "VALUES "
                             "(%2, %3, %4) ")
                             .arg(toQueryTextString(toInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(quantity)).arg(toQueryTextString(uniqueIdentifier)));

    // Remove from 'from' inventory
    queryList.append(QString("DELETE FROM %1 WHERE item_id = %2 AND unique_identifier = %3")
                             .arg(toQueryTextString(fromInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(uniqueIdentifier)));

    QString eventType;
    if (toType == 0)
        eventType = "3"; // If item is to technician
    else if (toType == 1)
        eventType = "5"; // If item is to site

    // Update the UID state
    queryList.append(QString("UPDATE state "
                             "SET "
                             "date = %1, "
                             "event_type = %2,"
                             "reference = %3"
                             "WHERE item_id = %4 AND unique_identifier = %5 ")
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(eventType))
                             .arg(toQueryTextString(ui->transfer_LineEdit->text())).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(uniqueIdentifier)));

    processItem(queryList, quantity, uniqueIdentifier);
}

bool NewInventoryTransfer::processItem(const QStringList queryList, const QString quantity, const QString uniqueIdentifier) const
{
    for (int i = 0; i < queryList.count(); i++)
    {
        QSqlQuery query;
        query.exec(queryList.at(i));
        if (query.lastError().isValid())
        {
            QERMessageBox(query.lastError().text());
            QSqlDatabase::database().rollback();
            return false;
        }
    }

    if (uniqueIdentifierEnabled_m == "1")
        ui->tableWidget->addItem(itemId_m, ui->itemWidget->manufacturer(), ui->itemWidget->partNumber(), quantity, uniqueIdentifier);
    else
        ui->tableWidget->addItem(itemId_m, ui->itemWidget->manufacturer(), ui->itemWidget->partNumber(), quantity);
    return true;
}

void NewInventoryTransfer::openNewTransaction(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier)
{
    QSqlDatabase db = QSqlDatabase::database();

    db.transaction();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return;
    }

    QSqlQuery createNewTransaction;
    QString createNewTransactionString = "INSERT INTO inventory_transfer ";

    if (fromType == 0 && toType == 0)
        createNewTransactionString.append("(from_technician_id, to_technician_id, date) ");
    else if (fromType == 0 && toType == 1)
        createNewTransactionString.append("(from_technician_id, from_site_id, date) ");
    else if (fromType == 1 && toType == 0)
        createNewTransactionString.append("(from_site_id, to_technician_id, date) ");
    else if (fromType == 1 && toType == 1)
        createNewTransactionString.append("(from_site_id, from_site_id, date) ");

    createNewTransactionString.append(QString("VALUES "
                                              "(%1, %2, %3) ")
                                              .arg(toQueryTextString(ui->from_LineEdit->text())).arg(toQueryTextString(ui->to_LineEdit->text()))
                                              .arg(toQueryTextString(ui->date_dateEdit->date().toString())));

    createNewTransaction.exec(createNewTransactionString);
    if (createNewTransaction.lastError().isValid())
    {
        QERMessageBox(createNewTransaction.lastError().text());
        db.rollback();
        if (db.lastError().isValid())
            ROEMessageBox();
        return;
    }

    QSqlQuery getLastInsertRowid;
    QString getLastInsertRowidString = "SELECT last_insert_rowid()";

    getLastInsertRowid.exec(getLastInsertRowidString);
    if (getLastInsertRowid.lastError().isValid())
    {
        QERMessageBox(getLastInsertRowid.lastError().text());
        db.rollback();
        if (db.lastError().isValid())
            ROEMessageBox();
        return;
    }

    getLastInsertRowid.first();

    QString transferId = getLastInsertRowid.value(0).toString();
    ui->transfer_LineEdit->setText(transferId);

    ui->release_PushButton->setEnabled(true);
    ui->from_LineEdit->setReadOnly(true);
    ui->to_LineEdit->setReadOnly(true);

    if (uniqueIdentifierEnabled_m == "0")
        addItem(itemId, line, quantity);
    else
        addItem(itemId, line, quantity, uniqueIdentifier);
}

void NewInventoryTransfer::getAllocatedQuantity(const QString itemId, const QString manufacturer, const QString partNumber, const QString productName, const QString uniqueIdentifierEnabled)
{
    if (uniqueIdentifierEnabled == "1")
    {
        QSqlQuery getIdentifier;
        QString getIdentifierString = QString("SELECT unique_identifier "
                                              "FROM %1 "
                                              "WHERE item_id = %2")
                                              .arg(toQueryTextString(fromInventory_m)).arg(toQueryTextString(itemId));
        getIdentifier.exec(getIdentifierString);

        QStringList identifier;

        while (getIdentifier.next())
        {
            identifier << getIdentifier.value(0).toString();
        }

        if (identifier.count() != 0)
        {
            ui->itemWidget->setItemInformation(manufacturer, partNumber, productName);
            ui->allocated_LineEdit->setText(QString::number(identifier.length()));
            ui->quantity_Label->setText("Identifiant :");
            ui->quantity_LineEdit->setValidator(0);
            QCompleter *completer = new QCompleter(identifier, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setMaxVisibleItems(5);
            ui->quantity_LineEdit->setCompleter(completer);

            allocatedIdentifierList_m = identifier;
            uniqueIdentifierEnabled_m = "1";
            itemId_m = itemId;
        }
        else
        {
            ui->itemWidget->getFocus();
            IQYMessageBox(productName, ui->fromName_LineEdit->text());
        }
    }
    else
    {
        QSqlQuery getQuantity;
        QString getQuantityString = QString("SELECT quantity "
                                            "FROM %1 "
                                            "WHERE item_id = %2")
                                            .arg(toQueryTextString(fromInventory_m)).arg(toQueryTextString(itemId));
        getQuantity.exec(getQuantityString);

        getQuantity.first();

        QString quantity = getQuantity.value(0).toString();

        if (quantity.toInt() > 0)
        {
            ui->itemWidget->setItemInformation(manufacturer, partNumber, productName);
            ui->allocated_LineEdit->setText(quantity);
            ui->quantity_Label->setText("Quantité :");
            ui->quantity_LineEdit->setCompleter(0);
            QValidator *validator = new QIntValidator(0, quantity.toInt(), this);
            ui->quantity_LineEdit->setValidator(validator);

            uniqueIdentifierEnabled_m = "0";
            allocatedQuantity_m = quantity;
            itemId_m = itemId;
        }
        else
        {
            ui->itemWidget->getFocus();
            IQYMessageBox(productName, ui->fromName_LineEdit->text());
        }
    }
}

void NewInventoryTransfer::maybeAddItem()
{
    if (!ui->itemWidget->partNumber().isEmpty())
    {
        if (!itemId_m.isNull())
        {
            if (uniqueIdentifierEnabled_m == "1")
            {
                if (allocatedIdentifierList_m.count() != 0)
                {
                    if (allocatedIdentifierList_m.contains(ui->quantity_LineEdit->text()))
                        maybeCreateNewTransaction();
                    else
                        IQTMessageBox();
                }
                else
                    INTMessageBox();
            }
            else
            {
                if (!allocatedQuantity_m.isNull())
                {
                    if (allocatedQuantity_m.toInt() >= ui->quantity_LineEdit->text().toInt())
                        maybeCreateNewTransaction();
                    else
                        IQTMessageBox();
                }
                else
                    INTMessageBox();
            }
        }
    else
        INTMessageBox();
    }
    else
        EELMessageBox("Article");
}

void NewInventoryTransfer::maybeCreateNewTransaction()
{
    if (ui->tableWidget->rowCount() == 0)
    {
        if (!ui->from_LineEdit->text().isEmpty() || !ui->to_LineEdit->text().isEmpty())
        {
            if (uniqueIdentifierEnabled_m == "1")
                openNewTransaction(itemId_m, QString::number(ui->tableWidget->rowCount()), "1", ui->quantity_LineEdit->text());
            else
                openNewTransaction(itemId_m, QString::number(ui->tableWidget->rowCount()), ui->quantity_LineEdit->text(), "");
        }
        else
            EELMessageBox("Technicien ou site");
    }
    else
    {
        if (uniqueIdentifierEnabled_m == "1")
            addItem(itemId_m, QString::number(ui->tableWidget->rowCount()), "1", ui->quantity_LineEdit->text());
        else
            addItem(itemId_m, QString::number(ui->tableWidget->rowCount()), ui->quantity_LineEdit->text());
    }
}

void NewInventoryTransfer::clearItemFields()
{
    ui->allocated_LineEdit->clear();
    ui->quantity_LineEdit->clear();
    ui->quantity_Label->setText("Quantité : ");
    allocatedIdentifierList_m.clear();
    allocatedQuantity_m.clear();
    itemId_m.clear();
    uniqueIdentifierEnabled_m.clear();

    ui->itemWidget->clearItemFields();
}

void NewInventoryTransfer::updateQuantityLabel()
{
    ui->allocated_LineEdit->clear();
    ui->quantity_Label->setText("Quantité : ");
}

