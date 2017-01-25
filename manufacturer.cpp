#include "manufacturer.h"
#include "ui_manufacturer.h"

/*
 *  Manufacturer
 */

Manufacturer::Manufacturer(QWidget *parent) :
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

Manufacturer::~Manufacturer()
{
    delete ui;
}

void Manufacturer::closeEvent(QCloseEvent *e)
{
    ui->manufacturerCode_LineEdit->blockSignals(true);
    ui->manufacturerName_LineEdit->blockSignals(true);
    e->accept();
}

/*
 *  NEW MANUFACTURER
 */

NewManufacturer::NewManufacturer(QWidget *parent) :
    Manufacturer(parent)
{
    ui->set_PushButton->setText("Créer");
    ui->set_PushButton->setEnabled(false);
    this->setWindowTitle("Créer un fabriquant");

    validManufacturer = false;

    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkAccount()));
}

NewManufacturer::~NewManufacturer()
{

}

bool NewManufacturer::validateFields()
{
    if (validManufacturer == true && !ui->manufacturerCode_LineEdit->text().isEmpty() && !ui->manufacturerName_LineEdit->text().isEmpty())
    {
        ui->set_PushButton->setEnabled(true);
        return true;
    }
    else
        ui->set_PushButton->setEnabled(false);

    return false;
}

void NewManufacturer::checkAccount()
{
    if (!ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery checkManufacturer;
        QString checkManufacturerString = QString("SELECT * FROM manufacturer WHERE manufacturer_code = %1")
                                                  .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        checkManufacturer.exec(checkManufacturerString);

        QStringList codeList;
        QStringList nameList;

        while(checkManufacturer.next())
        {
            codeList << checkManufacturer.value(0).toString();
            nameList << checkManufacturer.value(1).toString();
        }

        if (codeList.length() == 1)
        {
            validManufacturer = false;

            ui->manufacturerCode_LineEdit->blockSignals(true);
            bool ret = AEXMessageBox(QString("Le fabriquant").arg(nameList.at(0)));

            if (ret == false)
            {
                ui->manufacturerCode_LineEdit->setFocus();
                ui->manufacturerCode_LineEdit->selectAll();
            }
            else
            {
                UpdateManufacturer *updateManufacturer = new UpdateManufacturer(this);
                updateManufacturer->openAccount(codeList.at(0));
                updateManufacturer->show();

                QDialog::reject();
            }
            ui->manufacturerCode_LineEdit->blockSignals(false);
        }
        else
            validManufacturer = true;
    }
    else
    {
        ui->manufacturerCode_LineEdit->blockSignals(true);
        bool ret = EELMessageBox("Code de frabriquant ");

        if (ret == true)
            ui->manufacturerCode_LineEdit->setFocus();
        ui->manufacturerCode_LineEdit->blockSignals(false);
    }

    validateFields();
}

void NewManufacturer::accept()
{
    if (validateFields() == true)
    {
        QSqlQuery newManufacturer;
        QString newManufacturerString = QString("INSERT INTO manufacturer "
                                                   "(manufacturer_code, manufacturer_name) "
                                                "VALUES "
                                                   "(%1, %2)")
                                                .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()))
                                                .arg(toQueryTextString(ui->manufacturerName_LineEdit->text()));
        newManufacturer.exec(newManufacturerString);

        if (newManufacturer.lastError().isValid())
            QERMessageBox(newManufacturer.lastError().text());
        else
            QDialog::accept();
    }
    else
    {
        QString emptyElement;
        if (ui->manufacturerCode_LineEdit->text().isEmpty() || validManufacturer == false && !ui->manufacturerName_LineEdit->text().isEmpty())
            emptyElement = "Numéro de compte";
        else if (!ui->manufacturerCode_LineEdit->text().isEmpty() && validManufacturer == true && ui->manufacturerName_LineEdit->text().isEmpty())
            emptyElement = "Nom de compte";
        else
            emptyElement = "Nom et numéro de compte";

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

/*
 *  UPDATE MANUFACTURER
 */

UpdateManufacturer::UpdateManufacturer(QWidget *parent) :
    Manufacturer(parent)
{
    ui->set_PushButton->setText("Modifier");
    ui->set_PushButton->setEnabled(false);
    this->setWindowTitle("Modifier un fabriquant");

    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(openAccount()));
}

UpdateManufacturer::~UpdateManufacturer()
{

}

bool UpdateManufacturer::validateFields()
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

void UpdateManufacturer::openAccount(QString number)
{
    QSqlQuery openManufacturer;
    QString openManufacturerString = QString("SELECT * "
                                             "FROM manufacturer "
                                             "WHERE manufacturer_code = %1")
                                             .arg(toQueryTextString(number));
    openManufacturer.exec(openManufacturerString);

    openManufacturer.first();

    QString manufacturerCode = openManufacturer.value(0).toString();
    QString manufacturerName = openManufacturer.value(1).toString();

    ui->manufacturerCode_LineEdit->setText(manufacturerCode);
    ui->manufacturerName_LineEdit->setText(manufacturerName);

    ui->manufacturerName_LineEdit->selectAll();
    ui->manufacturerName_LineEdit->setFocus();

    validateFields();
}

void UpdateManufacturer::openAccount()
{
    ui->manufacturerName_LineEdit->clear();

    if (!ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery openManufacturer;
        QString openManufacturerString = QString("SELECT * "
                                                 "FROM manufacturer "
                                                 "WHERE manufacturer_code = %1")
                                                 .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        openManufacturer.exec(openManufacturerString);

        if (openManufacturer.first())
        {
            QString manufacturerCode = openManufacturer.value(0).toString();
            QString manufacturerName = openManufacturer.value(1).toString();

            ui->manufacturerCode_LineEdit->setText(manufacturerCode);
            ui->manufacturerName_LineEdit->setText(manufacturerName);
        }
        else
        {
            ui->manufacturerCode_LineEdit->blockSignals(true);
            bool ret = IEXMessageBox("Code de fabriquant");

            if (ret == true)
                ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->blockSignals(false);
        }
    }
    else
    {
        ui->manufacturerCode_LineEdit->blockSignals(true);
        bool ret = EELMessageBox("Code de fabriquant");

        if (ret == true)
            ui->manufacturerCode_LineEdit->setFocus();
        ui->manufacturerCode_LineEdit->blockSignals(false);
    }

    validateFields();
}

void UpdateManufacturer::accept()
{
    if (validateFields() == true)
    {
        QSqlQuery updateManufacturer;
        QString updateManufacturerString = QString("UPDATE manufacturer "
                                                   "SET manufacturer_name = %1 "
                                                   "WHERE manufacturer_code = %2 ")
                                                   .arg(toQueryTextString(ui->manufacturerName_LineEdit->text()))
                                                   .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        updateManufacturer.exec(updateManufacturerString);

        if (updateManufacturer.lastError().isValid())
            QERMessageBox(updateManufacturer.lastError().text());
        else
            QDialog::accept();
    }
    else
    {
        bool ret = IVAMessageBox("Nom du fabriquant");

        if (ret == true)
            ui->manufacturerName_LineEdit->setFocus();
    }
}
