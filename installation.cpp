#include "installation.h"
#include "ui_installation.h"

#include "selectiondialog.h"
#include "PromotedClass/itemtable.h"
#include "PromotedClass/itemwidget.h"

#include <QValidator>
#include <QCompleter>

/*
 *  Installation
 */

Installation::Installation(QWidget *parent) :
    DialogElement(parent),
    ui(new Ui::Installation)
{
    ui->setupUi(this);

    ui->technicianName_LineEdit->setReadOnly(true);
    ui->date_dateEdit->setReadOnly(true);
    ui->installation_LineEdit->setReadOnly(true);
    ui->allocated_LineEdit->setReadOnly(true);

    ui->tower_LineEdit->installEventFilter(this);

    ui->date_dateEdit->setDate(QDate::currentDate());

    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->release_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(ui->technician_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkTechnician()));
    // After all information are added in item Widget
    // Restore focus to installation
    connect(ui->itemWidget, SIGNAL(passFocus()),
            ui->quantity_LineEdit, SLOT(setFocus()));
}

Installation::~Installation()
{
    delete ui;
}

// Send focus to item widget when tab is pressed on site_LineEdit
bool Installation::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->tower_LineEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab)
        {
            ui->itemWidget->getFocus();
            return true;
        }
        else
            return false;
    }
    return false;
}

// Check if technician is valid
void Installation::checkTechnician()
{
    if (!ui->technician_LineEdit->text().isEmpty())
    {
        QSqlQuery checkTechnician;
        QString checkTechnicianString = QString("SELECT technician_id, technician_name "
                                                "FROM technician "
                                                "WHERE technician_id = %1 OR technician_name LIKE %2")
                                                .arg(toQueryTextString(ui->technician_LineEdit->text())).arg(toQueryLikeString(ui->technician_LineEdit->text()));
        checkTechnician.exec(checkTechnicianString);

        QStringList idList;
        QStringList nameList;
        while (checkTechnician.next())
        {
            idList << checkTechnician.value(0).toString();
            nameList << checkTechnician.value(1).toString();
        }

        if (idList.count() == 1)
        {
            ui->technician_LineEdit->setText(idList.at(0));
            ui->technicianName_LineEdit->setText(nameList.at(0));
            technicianInventory_m = "TECH_" + idList.at(0);
        }
        else if (idList.count() > 1)
        {
            QStringList headers;
            headers << "Number" << "Name";
            SelectionDialog *selection = new SelectionDialog(headers, idList, nameList, this);
            selection->show();

            QString id;
            if (selection->exec() == QDialog::Accepted)
            {
                id = selection->getValue();
            }

            int pos = idList.indexOf(id);
            ui->technician_LineEdit->setText(idList.at(pos));
            ui->technicianName_LineEdit->setText(nameList.at(pos));
            technicianInventory_m = "TECH_" + idList.at(pos);
        }
        else
        {
            ui->technician_LineEdit->setFocus();
            ui->technician_LineEdit->selectAll();
            IVAMessageBox("Technicien");
        }
    }
}

/*
 *  New Installation
 */

NewInstallation::NewInstallation(QWidget *parent) :
    Installation(parent)
{
    ui->release_PushButton->setEnabled(false);

    this->setWindowTitle("Nouvelle installation");

    connect(ui->itemWidget, SIGNAL(validItem(QString,QString,QString,QString,QString)),
            this, SLOT(getAllocatedQuantity(QString,QString,QString,QString,QString)));
    connect(ui->tableWidget, SIGNAL(itemAdded()),
            this, SLOT(clearItemFields()));
    connect(ui->tableWidget, SIGNAL(commentAdded()),
            this, SLOT(clearCommentFields()));
    connect(ui->quantity_LineEdit, SIGNAL(returnPressed()),
            this, SLOT(maybeAddItem()));
    connect(ui->comment_LineEdit, SIGNAL(returnPressed()),
            this, SLOT(maybeAddComment()));
    connect(ui->itemWidget, SIGNAL(makeUpdateQuantityLabel()),
            this, SLOT(updateQuantityLabel()));
}

NewInstallation::~NewInstallation()
{

}

// List of query for NoUID item
void NewInstallation::addItem(QString itemId, QString line, QString quantity)
{
    QStringList queryList;

    // Add to transfered table
    queryList.append(QString("INSERT INTO installed "
                             "(item_id, line, installation_id, quantity) "
                             "VALUES "
                             "(%1, %2, %3, %4) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(line))
                             .arg(toQueryTextString(ui->installation_LineEdit->text())).arg(toQueryTextString(quantity)));

    // Remove item from technician inventory
    queryList.append(QString("UPDATE %1 "
                             "SET "
                             "quantity = (SELECT quantity FROM state WHERE item_id = %2) - %3 "
                             "WHERE item_id = %2 ")
                             .arg(toQueryTextString(technicianInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(quantity)));

    queryList.append(QString("DELETE FROM %1 WHERE item_id = %2 AND quantity = '0'")
                             .arg(toQueryTextString(technicianInventory_m)).arg(toQueryTextString(itemId)));

    /*
     * Add quantity to installed state
     *
     * *** QUERY MUST BE AN UPSERT ***
     * To ensure history trigger are executed properly
     */

    queryList.append(QString("UPDATE state "
                             "SET "
                             "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND event_type = '4') + %2), "
                             "date = %3, "
                             "reference = %4 "
                             "WHERE item_id = %1 AND event_type = '4' ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->installation_LineEdit->text())));

    queryList.append(QString("INSERT OR IGNORE INTO state "
                             "(item_id, quantity, date, event_type, unique_identifier, reference) "
                             "VALUES "
                             "(%1, %2, %3, '4', '', %4) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->installation_LineEdit->text())));

    //Remove quantity from old state
    queryList.append(QString("UPDATE state "
                             "SET "
                             "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND event_type = '3') - %2) "
                             "WHERE item_id = %1 AND event_type = '3'")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity)));

    queryList.append(QString("DELETE FROM state WHERE item_id = %1 AND event_type = '3' AND quantity = '0'")
                             .arg(toQueryTextString(itemId)));

    processItem(queryList, quantity, "");
}

// List of query for UID item
void NewInstallation::addItem(QString itemId, QString line, QString quantity, QString uniqueIdentifier)
{
    QStringList queryList;

    // Add to installed item table
    queryList.append(QString("INSERT INTO installed "
                             "(item_id, line, installation_id, quantity, unique_identifier) "
                             "VALUES "
                             "(%1, %2, %3, %4, %5) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(line))
                             .arg(toQueryTextString(ui->installation_LineEdit->text())).arg(toQueryTextString("1"))
                             .arg(toQueryTextString(uniqueIdentifier)));

    // Remove from technician inventory
    queryList.append(QString("DELETE FROM %1 WHERE item_id = %2 AND unique_identifier = %3")
                             .arg(toQueryTextString(technicianInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(uniqueIdentifier)));

    // Update state
    queryList.append(QString("UPDATE state "
                             "SET "
                             "date = %1, "
                             "event_type = %2, "
                             "reference = %3 "
                             "WHERE item_id = %4 AND unique_identifier = %5 ")
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString("4"))
                             .arg(toQueryTextString(ui->installation_LineEdit->text())).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(uniqueIdentifier)));

    processItem(queryList, quantity, uniqueIdentifier);
}

bool NewInstallation::processItem(QStringList queryList, const QString quantity, const QString uniqueIdentifier) const
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

void NewInstallation::addComment(QString line, QString comment)
{
    QSqlQuery addCommentToInvoice;
    QString addCommentToInvoiceString = QString("INSERT INTO installed "
                                                    "(installation_id, line, comment) "
                                                "VALUES "
                                                    "(%1, %2, %3) ")
                                                .arg(toQueryTextString(ui->installation_LineEdit->text())).arg(toQueryTextString(line))
                                                .arg(toQueryTextString(comment));

    addCommentToInvoice.exec(addCommentToInvoiceString);
    if (addCommentToInvoice.lastError().isValid())
    {
        QERMessageBox(addCommentToInvoice.lastError().text());
        QSqlDatabase::database().rollback();
        if (QSqlDatabase::database().lastError().isValid())
            ROEMessageBox();
    }

    ui->tableWidget->addComment(ui->comment_LineEdit->text());
}

void NewInstallation::openNewTransaction(QString itemId, QString comment, QString line, QString quantity, QString uniqueIdentifier, int type)
{
    QSqlDatabase db = QSqlDatabase::database();

    db.transaction();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return;
    }

    QSqlQuery createNewTransaction;
    QString createNewTransactionString = QString("INSERT INTO installation "
                                                     "(technician_id, customer, tower, date) "
                                                 "VALUES "
                                                     "(%1, %2, %3, %4) ")
                                                 .arg(toQueryTextString(ui->technician_LineEdit->text())).arg(toQueryTextString(ui->customer_LineEdit->text()))
                                                 .arg(toQueryTextString(ui->tower_LineEdit->text())).arg(toQueryTextString(ui->date_dateEdit->date().toString()));

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

    QString installationId = getLastInsertRowid.value(0).toString();
    ui->installation_LineEdit->setText(installationId);

    ui->release_PushButton->setEnabled(true);
    ui->technician_LineEdit->setReadOnly(true);

    if (type == 0)
    {
        if (uniqueIdentifierEnabled_m == "0")
            addItem(itemId, line, quantity);
        else
            addItem(itemId, line, quantity, uniqueIdentifier);
    }
    else
        addComment(comment, line);
}

void NewInstallation::accept()
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

void NewInstallation::reject()
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

void NewInstallation::closeEvent(QCloseEvent *e)
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

void NewInstallation::maybeAddComment()
{
    if (!ui->comment_LineEdit->text().isEmpty())
    {
        maybeCreateNewTransaction(1);
    }
    else
        EELMessageBox("Commentaire");
}

void NewInstallation::maybeAddItem()
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
                        maybeCreateNewTransaction(0);
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
                        maybeCreateNewTransaction(0);
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

void NewInstallation::maybeCreateNewTransaction(int type)
{
    if (ui->tableWidget->rowCount() == 0)
    {
        if (!ui->technician_LineEdit->text().isEmpty())
        {
            if (uniqueIdentifierEnabled_m == "1")
                openNewTransaction(itemId_m, ui->comment_LineEdit->text(), QString::number(ui->tableWidget->rowCount()),
                                   "1", ui->quantity_LineEdit->text(), type);
            else
                openNewTransaction(itemId_m, ui->comment_LineEdit->text(), QString::number(ui->tableWidget->rowCount()),
                                   ui->quantity_LineEdit->text(), "", type);
        }
        else
            EELMessageBox("Technicien");
    }
    else
    {
        if (type == 0)
        {
            if (uniqueIdentifierEnabled_m == "1")
                addItem(itemId_m, QString::number(ui->tableWidget->rowCount()), "1", ui->quantity_LineEdit->text());
            else
                addItem(itemId_m, QString::number(ui->tableWidget->rowCount()), ui->quantity_LineEdit->text());
        }
        else
            addComment(QString::number(ui->tableWidget->rowCount()), ui->comment_LineEdit->text());
    }
}

void NewInstallation::clearItemFields()
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

void NewInstallation::clearCommentFields()
{
    ui->comment_LineEdit->clear();
}

void NewInstallation::getAllocatedQuantity(const QString itemId, const QString manufacturer, const QString partNumber, const QString productName, const QString uniqueIdentifierEnabled)
{
    if (uniqueIdentifierEnabled == "1")
    {
        QSqlQuery getIdentifier;
        QString getIdentifierString = QString("SELECT unique_identifier "
                                              "FROM %1 "
                                              "WHERE item_id = %2")
                                              .arg(toQueryTextString(technicianInventory_m)).arg(toQueryTextString(itemId));
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
            IQYMessageBox(productName, ui->technicianName_LineEdit->text());
        }
    }
    else
    {
        QSqlQuery getQuantity;
        QString getQuantityString = QString("SELECT quantity "
                                            "FROM %1 "
                                            "WHERE item_id = %2")
                                            .arg(technicianInventory_m).arg(toQueryTextString(itemId));
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
            IQYMessageBox(productName, ui->technicianName_LineEdit->text());
        }
    }
}

void NewInstallation::updateQuantityLabel()
{
    ui->allocated_LineEdit->clear();
    ui->quantity_Label->setText("Quantité : ");
}
