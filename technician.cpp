#include "technician.h"
#include "ui_technician.h"
#include "selectiondialog.h"

/*
 *  TECHNICIAN
 */

Technician::Technician(QWidget *parent) :
    Account(parent),
    ui(new Ui::Technician)
{
    ui->setupUi(this);

    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->set_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
}

Technician::~Technician()
{

}

void Technician::closeEvent(QCloseEvent *e)
{
    ui->phoneNumber_LineEdit->blockSignals(true);
    ui->technician_LineEdit->blockSignals(true);
    ui->name_LineEdit->blockSignals(true);
    e->accept();
}

/*
 *  NEW TECHNICIAN
 */

NewTechnician::NewTechnician(QWidget *parent) :
    Technician(parent)
{
    ui->technician_LineEdit->setReadOnly(true);
    ui->phoneNumber_LineEdit->setFocus();

    ui->set_PushButton->setEnabled(false);
    validTechnicien = false;

    ui->set_PushButton->setText("Créer");

    connect(ui->phoneNumber_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkPhoneNumber()));
    connect(ui->name_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(validateFields()));

    // Navigation
    setTabOrder(ui->phoneNumber_LineEdit, ui->name_LineEdit);
    setTabOrder(ui->name_LineEdit, ui->email_LineEdit);
    setTabOrder(ui->email_LineEdit, ui->note_PlainTextEdit);
}

NewTechnician::~NewTechnician()
{

}

void NewTechnician::accept()
{
    if (validateFields() == true)
    {
        bool ret = newTechnician();

        if (ret == true)
            QDialog::accept();
    }
    else
    {
        QString emptyElement;
        if (ui->technician_LineEdit->text().isEmpty() || validTechnicien == false && !ui->name_LineEdit->text().isEmpty())
            emptyElement = "Numéro de compte";
        else if (ui->name_LineEdit->text().isEmpty() && validTechnicien == true &&!ui->technician_LineEdit->text().isEmpty())
            emptyElement = "Nom du compte";
        else
            emptyElement = "Nom et numéro de compte";

        bool ret = IVAMessageBox(emptyElement);

        if (ret == true)
        {
            if (ui->technician_LineEdit->text().isEmpty())
                ui->technician_LineEdit->setFocus();
            else
                ui->name_LineEdit->setFocus();
        }
    }
}

bool NewTechnician::newTechnician()
{
    QSqlQuery newTechnician;
    QString newTechnicianString = QString("INSERT INTO technician"
                                          "(technician_id, technician_name, phone_number, email, note)"
                                          "VALUES"
                                          "(%1, %2, %3, %4, %5)")
                                          .arg(toQueryTextString(ui->technician_LineEdit->text()))
                                          .arg(toQueryTextString(ui->name_LineEdit->text()))
                                          .arg(toQueryTextString(ui->phoneNumber_LineEdit->text()))
                                          .arg(toQueryTextString(ui->email_LineEdit->text()))
                                          .arg(toQueryTextString(ui->note_PlainTextEdit->toPlainText()));

    QString tableName = "TECH_" + ui->technician_LineEdit->text();

    QSqlQuery newTechnicienInventory;
    QString newTechnicienInventoryString = QString("CREATE TABLE %1"
                                                   "("
                                                       "item_id              INT        REFERENCES item (item_id),"
                                                       "quantity             INT        NOT NULL     DEFAULT 0,"
                                                       "unique_identifier    TEXT,"
                                                       "PRIMARY KEY (item_id, unique_identifier)"
                                                   ")").arg(toQueryTextString(tableName));

    QSqlDatabase db = QSqlDatabase::database();

    db.transaction();
    if (db.lastError().isValid())
    {
        QERMessageBox(db.lastError().text());
        return false;
    }
    newTechnician.exec(newTechnicianString);
    if (newTechnician.lastError().isValid())
    {
        QERMessageBox(newTechnician.lastError().text());
        db.rollback();
        if (db.lastError().isValid())
            ROEMessageBox();
        return false;
    }
    newTechnicienInventory.exec(newTechnicienInventoryString);
    if (newTechnicienInventory.lastError().isValid())
    {
        QERMessageBox(newTechnicienInventory.lastError().text());
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

bool NewTechnician::validateFields()
{
    if (validTechnicien == true && !ui->technician_LineEdit->text().isEmpty() && !ui->name_LineEdit->text().isEmpty())
    {
        ui->set_PushButton->setEnabled(true);
        return true;
    }
    else
        ui->set_PushButton->setEnabled(false);

    return false;
}

void NewTechnician::checkPhoneNumber()
{
    ui->technician_LineEdit->text().clear();

    if (!ui->phoneNumber_LineEdit->text().isEmpty())
    {
        QSqlQuery checkTechnician;
        QString checkTechnicianString = QString("SELECT technician_id, technician_name FROM technician "
                                                "WHERE technician_id = %1")
                                                .arg(toQueryTextString(createAccountNumber(ui->phoneNumber_LineEdit->text())));
        checkTechnician.exec(checkTechnicianString);

        QStringList idList;
        QStringList nameList;

        while (checkTechnician.next())
        {
            idList << checkTechnician.value(0).toString();
            nameList << checkTechnician.value(1).toString();
        }

        if (idList.length() == 1)
        {
            validTechnicien = false;

            ui->phoneNumber_LineEdit->blockSignals(true);
            bool ret = AEXMessageBox(QString("Le technicien %1").arg(nameList.at(0)));

            if (ret == false)
            {
                ui->technician_LineEdit->clear();
                ui->phoneNumber_LineEdit->clear();

                ui->phoneNumber_LineEdit->setFocus();
            }
            else
            {
                UpdateTechnician *updateTechnician = new UpdateTechnician(this);
                updateTechnician->openAccount(idList.at(0));
                updateTechnician->show();

                QDialog::reject();
            }
            ui->phoneNumber_LineEdit->blockSignals(false);
        }
        else
        {
            ui->technician_LineEdit->setText(createAccountNumber(ui->phoneNumber_LineEdit->text()));
            validTechnicien = true;
        }
    }
    else
    {
        ui->phoneNumber_LineEdit->blockSignals(true);
        bool ret = EELMessageBox("Numéro de téléphone");

        if (ret == true)
            ui->phoneNumber_LineEdit->setFocus();
        ui->phoneNumber_LineEdit->blockSignals(false);
    }

    validateFields();
}

/*
 *  UPDATE TECHNICIAN
 */

UpdateTechnician::UpdateTechnician(QWidget *parent) :
    Technician(parent)
{
    this->setWindowTitle("Modifier un technicien");

    ui->technician_LineEdit->setFocus();

    ui->set_PushButton->setEnabled(false);
    ui->set_PushButton->setText("Modifier");

    connect(ui->technician_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(openAccount()));
    connect(ui->name_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(validateFields()));
}

UpdateTechnician::~UpdateTechnician()
{

}

void UpdateTechnician::accept()
{
    if (validateFields() == true)
    {
        QSqlQuery updateTechnician;
        const QString updateTechnicianString = QString("UPDATE technician "
                                                           "SET "
                                                               "technician_name = %1, "
                                                               "phone_number = %2, "
                                                               "email = %3, "
                                                               "note = %4 "
                                                           "WHERE technician_id = %5 ")
                                                 .arg(toQueryTextString(ui->name_LineEdit->text()))
                                                 .arg(toQueryTextString(ui->phoneNumber_LineEdit->text()))
                                                 .arg(toQueryTextString(ui->email_LineEdit->text()))
                                                 .arg(toQueryTextString(ui->note_PlainTextEdit->toPlainText()))
                                                 .arg(toQueryTextString(ui->technician_LineEdit->text()));
        updateTechnician.exec(updateTechnicianString);

        if (updateTechnician.lastError().isValid())
            QERMessageBox(updateTechnician.lastError().text());
        else
            QDialog::accept();
    }
    else
    {
        bool ret = IVAMessageBox("Nom");

        if (ret == true)
            ui->name_LineEdit->setFocus();
    }
}

bool UpdateTechnician::validateFields()
{
    if (!ui->name_LineEdit->text().isEmpty())
    {
        ui->set_PushButton->setEnabled(true);
        return true;
    }
    else
        ui->set_PushButton->setEnabled(false);

    return false;
}

void UpdateTechnician::openAccount(QString number)
{
    QSqlQuery openTechnician;
    QString openTechnicianString = QString("SELECT technician_id, technician_name, phone_number, email, note FROM technician "
                                         "WHERE technician_id = %1")
                                         .arg(toQueryTextString(number));
    openTechnician.exec(openTechnicianString);

    openTechnician.first();

    QString technicianId = openTechnician.value(0).toString();
    QString technicianName = openTechnician.value(1).toString();
    QString technicianPhoneNumber = openTechnician.value(2).toString();
    QString technicianEmail = openTechnician.value(3).toString();
    QString technicianNote = openTechnician.value(4).toString();

    ui->technician_LineEdit->setText(technicianId);
    ui->name_LineEdit->setText(technicianName);
    ui->phoneNumber_LineEdit->setText(technicianPhoneNumber);
    ui->email_LineEdit->setText(technicianEmail);
    ui->note_PlainTextEdit->setPlainText(technicianNote);

    ui->name_LineEdit->setFocus();
    ui->name_LineEdit->selectAll();
}

void UpdateTechnician::openAccount()
{
    ui->name_LineEdit->clear();
    ui->phoneNumber_LineEdit->clear();
    ui->email_LineEdit->clear();
    ui->note_PlainTextEdit->clear();

    if (!ui->technician_LineEdit->text().isEmpty())
    {
        QSqlQuery openTechnician;
        QString openTechnicianString = QString("SELECT technician_id, technician_name, phone_number, email, note FROM technician "
                                               "WHERE technician_id = %1")
                                               .arg(toQueryTextString(ui->technician_LineEdit->text()));
        openTechnician.exec(openTechnicianString);

        if (openTechnician.first())
        {
            QString technician = openTechnician.value(0).toString();
            QString name = openTechnician.value(1).toString();
            QString phoneNumber = openTechnician.value(2).toString();
            QString email = openTechnician.value(3).toString();
            QString note = openTechnician.value(4).toString();

            ui->technician_LineEdit->setText(technician);
            ui->name_LineEdit->setText(name);
            ui->phoneNumber_LineEdit->setText(phoneNumber);
            ui->email_LineEdit->setText(email);
            ui->note_PlainTextEdit->setPlainText(note);
        }
        else
        {
            ui->technician_LineEdit->blockSignals(true);
            bool ret = IEXMessageBox("Technicien");

            if (ret == true)
                ui->technician_LineEdit->setFocus();
            ui->technician_LineEdit->blockSignals(false);
        }
    }
    else
    {
        ui->technician_LineEdit->blockSignals(true);
        bool ret = EELMessageBox("Numéro du technicien");

        if (ret == true)
            ui->technician_LineEdit->setFocus();
        ui->technician_LineEdit->blockSignals(false);
    }
}

ViewTechnician::ViewTechnician(QWidget *parent) :
    UpdateTechnician(parent)
{
    this->setWindowTitle("Consulter un compte technicien");
    ui->set_PushButton->setText("Ok");

    ui->name_LineEdit->setReadOnly(true);
    ui->phoneNumber_LineEdit->setReadOnly(true);
    ui->email_LineEdit->setReadOnly(true);
    ui->note_PlainTextEdit->setReadOnly(true);
}

ViewTechnician::~ViewTechnician()
{

}

void ViewTechnician::accept()
{
    QDialog::accept();
}
