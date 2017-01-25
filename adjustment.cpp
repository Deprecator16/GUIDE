#include "adjustment.h"
#include "ui_adjustment.h"

#include "selectiondialog.h"

#include "PromotedClass/itemtable.h"
#include "PromotedClass/itemwidget.h"

/*
 *  Adjustement
 */

Adjustment::Adjustment(QWidget *parent) :
    DialogElement(parent),
    ui(new Ui::Adjustment)
{
    ui->setupUi(this);

    ui->siteName_LineEdit->setReadOnly(true);
    ui->date_dateEdit->setReadOnly(true);
    ui->adjustement_LineEdit->setReadOnly(true);
    ui->stock_LineEdit->setReadOnly(true);

    ui->site_LineEdit->installEventFilter(this);

    ui->date_dateEdit->setDate(QDate::currentDate());

    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->release_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(ui->site_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkSite()));
    // After all information are added in item Widget
    // Restore focus to adjustement
    connect(ui->itemWidget, SIGNAL(passFocus()),
            ui->quantity_LineEdit, SLOT(setFocus()));
}

Adjustment::~Adjustment()
{
    delete ui;
}

// Check if the site is valid
void Adjustment::checkSite()
{
    if (!ui->site_LineEdit->text().isEmpty())
    {
        QSqlQuery checkSite;
        QString checkSiteString = QString("SELECT site_code, name "
                                          "FROM site "
                                          "WHERE site_code = %1 OR name LIKE %2")
                                          .arg(toQueryTextString(ui->site_LineEdit->text())).arg(toQueryLikeString(ui->site_LineEdit->text()));
        checkSite.exec(checkSiteString);

        QStringList codeList;
        QStringList nameList;
        while (checkSite.next())
        {
            codeList << checkSite.value(0).toString();
            nameList << checkSite.value(1).toString();
        }

        if (codeList.length() == 1)
        {
            ui->site_LineEdit->setText(codeList.at(0));
            ui->siteName_LineEdit->setText(nameList.at(0));
            siteInventory_m = "SITE_" + codeList.at(0);
        }
        else if (codeList.length() > 1)
        {
            QStringList headers;
            headers << "Code" << "Name";
            SelectionDialog *selection = new SelectionDialog(headers, codeList, nameList, this);
            selection->show();

            QString code;
            if (selection->exec() == QDialog::Accepted)
            {
                code = selection->getValue();
                int pos = codeList.indexOf(code);
                ui->site_LineEdit->setText(codeList.at(pos));
                ui->siteName_LineEdit->setText(nameList.at(pos));
                siteInventory_m = "SITE_" + codeList.at(pos);
            }
        }
        else
        {
            ui->siteName_LineEdit->clear();
            siteInventory_m.clear();

            ui->site_LineEdit->setFocus();
            ui->site_LineEdit->selectAll();
            IVAMessageBox("Site");
        }
    }
}

// Send focus to item widget when tab is pressed on site_LineEdit
bool Adjustment::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->site_LineEdit && event->type() == QEvent::KeyPress)
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

/*
 *  New Adjustement
 */

NewAdjustement::NewAdjustement(QWidget *parent) :
    Adjustment(parent)
{
    ui->release_PushButton->setEnabled(false);

    this->setWindowTitle("Nouvel ajustement");

    connect(ui->itemWidget, SIGNAL(validItem(QString,QString,QString,QString,QString)),
            this, SLOT(getStockQuantity(QString,QString,QString,QString,QString)));
    connect(ui->tableWidget, SIGNAL(itemAdded()),
            this, SLOT(clearItemFields()));
    connect(ui->quantity_LineEdit, SIGNAL(returnPressed()),
            this, SLOT(maybeAddItem()));
    connect(ui->itemWidget, SIGNAL(makeUpdateQuantityLabel()),
            this, SLOT(updateQuantityLabel()));
}

NewAdjustement::~NewAdjustement()
{

}

void NewAdjustement::accept()
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

void NewAdjustement::reject()
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

void NewAdjustement::closeEvent(QCloseEvent *e)
{
    if (ui->tableWidget->rowCount() > 0)
    {
        QSqlDatabase::database().rollback();
        if (QSqlDatabase::database().lastError().isValid())
        {
            ROEMessageBox();
            e->ignore();
        }
    }
    e->accept();
}

bool NewAdjustement::processItem(const QStringList queryList, const QString quantity, const QString uniqueIdentifier) const
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

void NewAdjustement::addItem(const QString itemId, const QString line, const QString quantity) const
{
    QStringList queryList;
    queryList.append(QString("INSERT INTO adjusted "
                             "(item_id, line, adjustment_id, quantity, unique_identifier) "
                             "VALUES "
                             "(%1, %2, %3, %4, '') ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(line))
                             .arg(toQueryTextString(ui->adjustement_LineEdit->text())).arg(toQueryTextString(quantity)));


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
                             .arg(toQueryTextString(siteInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(quantity)));

    queryList.append(QString("UPDATE state "
                             "SET "
                             "quantity = ((SELECT quantity FROM state WHERE item_id = %1 AND is_stock = '0') + %2), "
                             "date = %3, "
                             "reference = %4, "
                             "event_type = '2' "
                             "WHERE item_id = %1 AND is_stock = '0' ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->adjustement_LineEdit->text())));

    queryList.append(QString("INSERT OR IGNORE INTO state "
                             "(item_id, quantity, date, event_type, is_stock, unique_identifier, reference) "
                             "VALUES "
                             "(%1, %2, %3, '2', '0', '', %4) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(ui->adjustement_LineEdit->text())));

    processItem(queryList, quantity, "");
}

void NewAdjustement::addItem(const QString itemId, const QString line, const QString quantity, const QString uniqueIdentifier) const
{
    QStringList queryList;
    queryList.append(QString("INSERT INTO adjusted "
                             "(item_id, line, adjustment_id, quantity, unique_identifier) "
                             "VALUES "
                             "(%1, %2, %3, %4, %5) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(line))
                             .arg(toQueryTextString(ui->adjustement_LineEdit->text())).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(uniqueIdentifier)));

    queryList.append(QString("INSERT INTO %1 "
                             "(item_id, quantity, unique_identifier) "
                             "VALUES "
                             "(%2, %3, %4) ")
                             .arg(toQueryTextString(siteInventory_m)).arg(toQueryTextString(itemId))
                             .arg(toQueryTextString(quantity)).arg(toQueryTextString(uniqueIdentifier)));

    queryList.append(QString("INSERT INTO state "
                             "(item_id, quantity, date, unique_identifier, event_type, reference) "
                             "VALUES "
                             "(%1, %2, %3, %4, '2', %5) ")
                             .arg(toQueryTextString(itemId)).arg(toQueryTextString(quantity))
                             .arg(toQueryTextString(ui->date_dateEdit->date().toString())).arg(toQueryTextString(uniqueIdentifier))
                             .arg(toQueryTextString(ui->adjustement_LineEdit->text())));

    processItem(queryList, quantity, uniqueIdentifier);
}

void NewAdjustement::openNewTransaction(QString itemId, QString line, QString quantity, QString uniqueIdentifier)
{
    QSqlDatabase db = QSqlDatabase::database();

    db.transaction();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return;
    }

    QSqlQuery createNewTransaction;
    QString createNewTransactionString = QString("INSERT INTO adjustment "
                                                 "(site_code, date) "
                                                 "VALUES "
                                                 "(%1, %2) ")
                                                 .arg(toQueryTextString(ui->site_LineEdit->text())).arg(toQueryTextString(ui->date_dateEdit->date().toString()));

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

    QString adjustmentId = getLastInsertRowid.value(0).toString();
    ui->adjustement_LineEdit->setText(adjustmentId);

    ui->release_PushButton->setEnabled(true);
    ui->site_LineEdit->setReadOnly(true);

    if (uniqueIdentifierEnabled_m == "0")
        addItem(itemId, line, quantity);
    else
        addItem(itemId, line, quantity, uniqueIdentifier);
}

void NewAdjustement::getStockQuantity(QString itemId, QString manufacturer, QString partNumber, QString productName, QString uniqueIdentifierEnabled)
{
    QString quantity;
    if (uniqueIdentifierEnabled == "1")
    {
        QSqlQuery getQuantity;
        QString getQuantityString = QString("SELECT unique_identifier "
                                            "FROM state "
                                            "WHERE item_id = %1 AND is_stock = '0'")
                                            .arg(toQueryTextString(itemId));
        getQuantity.exec(getQuantityString);

        QStringList identifier;

        while (getQuantity.next())
        {
            identifier << getQuantity.value(0).toString();
        }

        ui->quantity_Label->setText("Identifiant : ");
        quantity = QString::number(identifier.count());
    }
    else
    {
        QSqlQuery getQuantity;
        QString getQuantityString = QString("SELECT quantity "
                                            "FROM state "
                                            "WHERE item_id = %1 AND is_stock = '0'")
                                            .arg(toQueryTextString(itemId));
        getQuantity.exec(getQuantityString);

        QList<int> quantityList;
        while (getQuantity.next())
        {
            quantityList << getQuantity.value(0).toInt();
        }

        int qty = 0;
        for (int i = 0; i < quantityList.count(); i++)
        {
            qty += quantityList.at(i);
        }

        quantity = QString::number(qty);
    }

    uniqueIdentifierEnabled_m = uniqueIdentifierEnabled;
    itemId_m = itemId;
    ui->stock_LineEdit->setText(quantity);
    ui->itemWidget->setItemInformation(manufacturer, partNumber, productName);
}

void NewAdjustement::maybeAddItem()
{
    if (!ui->itemWidget->partNumber().isEmpty())
    {
        if (uniqueIdentifierEnabled_m == "1")
        {
            QSqlQuery getExistingUniqueIdentifier;
            QString getExistingUniqueIdentifierString = QString ("SELECT unique_identifier "
                                                                 "FROM state "
                                                                 "WHERE item_id = %1) ");
            getExistingUniqueIdentifier.exec(getExistingUniqueIdentifierString);

            QStringList identifier;
            while (getExistingUniqueIdentifier.next())
            {
                identifier << getExistingUniqueIdentifier.value(0).toString();
            }

            if (identifier.contains(ui->quantity_LineEdit->text()))
            {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Gestionnaire d'inventaire Ondenet");
                msgBox.setText("Article existant");
                msgBox.setInformativeText(QString("L'identifiant %1 existe dégà.").arg(ui->quantity_LineEdit->text()));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();
            }
            else
                maybeCreateNewTransaction();
        }
        else
            maybeCreateNewTransaction();
    }
    else
        EELMessageBox("Article");
}

void NewAdjustement::maybeCreateNewTransaction()
{
    if (ui->tableWidget->rowCount() == 0)
    {
        if (!ui->site_LineEdit->text().isEmpty())
        {
            if (uniqueIdentifierEnabled_m == "1")
                openNewTransaction(itemId_m, QString::number(ui->tableWidget->rowCount()), "1", ui->quantity_LineEdit->text());
            else
                openNewTransaction(itemId_m, QString::number(ui->tableWidget->rowCount()), ui->quantity_LineEdit->text(), "");
        }
        else
            EELMessageBox("Site");
    }
    else
    {
        if (uniqueIdentifierEnabled_m == "1")
            addItem(itemId_m, QString::number(ui->tableWidget->rowCount()), "1", ui->quantity_LineEdit->text());
        else
            addItem(itemId_m, QString::number(ui->tableWidget->rowCount()), ui->quantity_LineEdit->text());
    }
}

void NewAdjustement::clearItemFields()
{
    ui->quantity_LineEdit->clear();
    ui->stock_LineEdit->clear();
    ui->itemWidget->clearItemFields();

    ui->quantity_Label->setText("Quantité : ");
}

void NewAdjustement::updateQuantityLabel()
{
    ui->stock_LineEdit->clear();
    ui->quantity_Label->setText("Quantité : ");
}
