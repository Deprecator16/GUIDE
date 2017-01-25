#include "site.h"
#include "ui_manufacturer.h"

/*
 *  SITE
 */

Site::Site(QWidget *parent) :
    Account(parent),
    ui(new Ui::Manufacturer)
{
    ui->setupUi(this);

    connect(ui->set_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->manufacturerName_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(validateFields()));
}

Site::~Site()
{
    delete ui;
}

void Site::closeEvent(QCloseEvent *e)
{
    ui->manufacturerCode_LineEdit->blockSignals(true);
    ui->manufacturerName_LineEdit->blockSignals(true);
    e->accept();
}

/*
 *  NEW SITE
 */

NewSite::NewSite(QWidget *parent) :
    Site(parent)
{
    ui->set_PushButton->setText("Créer");
    ui->set_PushButton->setEnabled(false);
    this->setWindowTitle("Créer un site");

    validSite = false;

    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkAccount()));
}

NewSite::~NewSite()
{

}

void NewSite::accept()
{
    if (validateFields() == true)
    {
        bool ret = newSite();

        if (ret == true)
            QDialog::accept();
    }
    else
    {
        QString emptyElement;
        if (ui->manufacturerCode_LineEdit->text().isEmpty() || validSite == false && !ui->manufacturerName_LineEdit->text().isEmpty())
            emptyElement = "Code de site";
        else if (ui->manufacturerName_LineEdit->text().isEmpty() && validSite == true && !ui->manufacturerCode_LineEdit->text().isEmpty())
            emptyElement = "Nom du site";
        else
            emptyElement = "Nom et code du site";

        bool ret = IVAMessageBox(emptyElement);

        if (ret == true)
        {
            if (ui->manufacturerCode_LineEdit->text().isEmpty())
                ui->manufacturerCode_LineEdit->setFocus();
            else
                ui->manufacturerName_LineEdit->setFocus();
        }
    }
}

bool NewSite::newSite()
{
    QSqlQuery newSite;
    QString newSiteString = QString("INSERT INTO site "
                                        "(site_code, name) "
                                    "VALUES "
                                        "(%1, %2) ")
                                   .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()))
                                   .arg(toQueryTextString(ui->manufacturerName_LineEdit->text()));

    QString tableName = "SITE_" + ui->manufacturerCode_LineEdit->text();

    QSqlQuery newSiteInventory;
    QString newSiteInventoryString = QString("CREATE TABLE %1 "
                                             "("
                                                 "item_id              INT        REFERENCES item (item_id), "
                                                 "quantity             INT        NOT NULL     DEFAULT 0, "
                                                 "unique_identifier    TEXT       NOT NULL, "
                                                 "PRIMARY KEY (item_id, unique_identifier) "
                                             ")")
                                             .arg(toQueryTextString(tableName));

    QSqlDatabase db = QSqlDatabase::database();

    db.transaction();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return false;
    }
    newSite.exec(newSiteString);
    if (newSite.lastError().isValid())
    {
        QERMessageBox(newSite.lastError().text());
        db.rollback();
        if (db.lastError().isValid())
            ROEMessageBox();
        return false;
    }
    newSiteInventory.exec(newSiteInventoryString);
    if (newSiteInventory.lastError().isValid())
    {
        QERMessageBox(newSiteInventory.lastError().text());
        db.rollback();
        if (db.lastError().isValid())
            ROEMessageBox();
        return false;
    }
    db.commit();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        db.rollback();
        if (db.lastError().isValid())
            ROEMessageBox();
        return false;
    }
    return true;
}

void NewSite::checkAccount()
{
    if (!ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery checkSite;
        QString checkSiteString = QString("SELECT * FROM site WHERE site_code = %1")
                                                  .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        checkSite.exec(checkSiteString);

        QStringList codeList;
        QStringList nameList;

        while(checkSite.next())
        {
            codeList << checkSite.value(0).toString();
            nameList << checkSite.value(1).toString();
        }

        if (codeList.length() == 1)
        {
            validSite = false;

            ui->manufacturerCode_LineEdit->blockSignals(true);

            bool ret = IEXMessageBox(QString("Le site %1").arg(nameList.at(0)));

            if (ret == false)
            {
                ui->manufacturerCode_LineEdit->setFocus();
                ui->manufacturerCode_LineEdit->selectAll();
            }
            else
            {
                UpdateSite *updateSite = new UpdateSite(this);
                updateSite->openAccount(codeList.at(0));
                updateSite->show();

                QDialog::reject();
            }
            ui->manufacturerCode_LineEdit->blockSignals(false);
        }
        else
            validSite = true;
    }
    else
    {
        ui->manufacturerCode_LineEdit->blockSignals(true);

        bool ret = EELMessageBox("Code de site");

        if (ret == true)
            ui->manufacturerCode_LineEdit->setFocus();

        ui->manufacturerCode_LineEdit->blockSignals(false);
    }

    validateFields();
}

bool NewSite::validateFields()
{
    if (validSite == true && !ui->manufacturerCode_LineEdit->text().isEmpty() && !ui->manufacturerName_LineEdit->text().isEmpty())
    {
        ui->set_PushButton->setEnabled(true);
        return true;
    }
    else
        ui->set_PushButton->setEnabled(false);

    return false;
}

/*
 *  UPDATE SITE
 */

UpdateSite::UpdateSite(QWidget *parent)
    : Site(parent)
{
    ui->set_PushButton->setText("Modifier");
    ui->set_PushButton->setEnabled(false);
    this->setWindowTitle("Modifier un site");

    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(openAccount()));
}

UpdateSite::~UpdateSite()
{

}

void UpdateSite::accept()
{
    if (validateFields() == true)
    {
        QSqlQuery updateSite;
        QString updateSiteString = QString("UPDATE site "
                                           "SET name = %1 "
                                           "WHERE site_code = %2 ")
                                           .arg(toQueryTextString(ui->manufacturerName_LineEdit->text()))
                                           .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        updateSite.exec(updateSiteString);
        if (updateSite.lastError().isValid())
            QERMessageBox(updateSite.lastError().text());
        else
            QDialog::accept();
    }
    else
    {
        bool ret = IVAMessageBox("Nom du site");
        if (ret == true)
            ui->manufacturerName_LineEdit->setFocus();
    }
}

bool UpdateSite::validateFields()
{
    if (!ui->manufacturerName_LineEdit->text().isEmpty())
    {
        ui->set_PushButton->setEnabled(true);
        return true;
    }
    else
        ui->set_PushButton->setEnabled(false);

    return false;
}

void UpdateSite::openAccount(QString number)
{
    QSqlQuery openSite;
    QString openSiteString = QString("SELECT * "
                                     "FROM site "
                                     "WHERE site_code = %1")
                                     .arg(toQueryTextString(number));
    openSite.exec(openSiteString);

    openSite.first();

    QString siteCode = openSite.value(0).toString();
    QString siteName = openSite.value(1).toString();

    ui->manufacturerCode_LineEdit->setText(siteCode);
    ui->manufacturerName_LineEdit->setText(siteName);

    ui->manufacturerName_LineEdit->selectAll();
    ui->manufacturerName_LineEdit->setFocus();

    validateFields();
}

void UpdateSite::openAccount()
{
    ui->manufacturerName_LineEdit->clear();

    if (!ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery openSite;
        QString openSiteString = QString("SELECT * "
                                         "FROM site "
                                         "WHERE site_code = %1")
                                         .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        openSite.exec(openSiteString);

        if (openSite.first())
        {
            QString siteCode = openSite.value(0).toString();
            QString siteName = openSite.value(1).toString();

            ui->manufacturerCode_LineEdit->setText(siteCode);
            ui->manufacturerName_LineEdit->setText(siteName);
        }
        else
        {
            ui->manufacturerCode_LineEdit->blockSignals(true);

            bool ret = IVAMessageBox("Code du site");

            if (ret == true)
                ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->blockSignals(false);
        }
    }
    else
    {
        ui->manufacturerCode_LineEdit->blockSignals(true);

        bool ret = EELMessageBox("Code du site");

        if (ret == true)
            ui->manufacturerCode_LineEdit->setFocus();
        ui->manufacturerCode_LineEdit->blockSignals(false);
    }

    validateFields();
}
