#include "item.h"
#include "selectiondialog.h"

/*
 *  ITEM
 */

Item::Item(QWidget *parent) :
    DialogElement(parent), ui(new Ui::Item)
{
    ui->setupUi(this);

    connect(ui->set_PushButton, SIGNAL(pressed()),
            this, SLOT(accept()));
    connect(ui->cancel_PushButton, SIGNAL(pressed()),
            this, SLOT(reject()));
    connect(ui->name_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(validateFields()));
}

Item::~Item()
{
    delete ui;
}

void Item::closeEvent(QCloseEvent *e)
{
    ui->manufacturerCode_LineEdit->blockSignals(true);
    ui->partNumber_LineEdit->blockSignals(true);
    ui->name_LineEdit->blockSignals(true);
    e->accept();
}

int Item::checkManufacturerCode()
{
    if (!ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery checkManufacturer;
        QString checkManufacturerString = QString("SELECT * FROM manufacturer "
                                                  "WHERE manufacturer_code LIKE %1 OR manufacturer_name LIKE %1")
                                                  .arg(toQueryLikeString(ui->manufacturerCode_LineEdit->text()));
        checkManufacturer.exec(checkManufacturerString);

        QStringList codeList;
        QStringList nameList;
        while (checkManufacturer.next())
        {
            codeList << checkManufacturer.value(0).toString();
            nameList << checkManufacturer.value(1).toString();
        }
        if (codeList.length() == 1){
            ui->manufacturerCode_LineEdit->setText(codeList.at(0));

            ui->partNumber_LineEdit->setFocus();
            ui->partNumber_LineEdit->selectAll();

            return 0;
        }
        else if (codeList.length() > 1)
        {
            QStringList headers;
            headers << "Code" << "Name";
            SelectionDialog *selection = new SelectionDialog(headers, codeList, nameList, this);
            selection->show();

            QString code;
            if (selection->exec() == QDialog::Accepted)
                code = selection->getValue();

            if (!code.isEmpty())
            {
                ui->manufacturerCode_LineEdit->setText(code);
                ui->manufacturerCode_LineEdit->selectAll();

                return 0;
            }
            else
            {
                ui->manufacturerCode_LineEdit->blockSignals(true);
                ui->manufacturerCode_LineEdit->setFocus();
                ui->manufacturerCode_LineEdit->selectAll();

                IVAMessageBox("Code de fabriquant");

                ui->manufacturerCode_LineEdit->blockSignals(false);

                return 1;
            }
        }
        else
        {
            ui->manufacturerCode_LineEdit->blockSignals(true);
            ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->selectAll();
            IEXMessageBox("Code de fabriquant");

            ui->manufacturerCode_LineEdit->blockSignals(false);

            return 1;
        }
    }
    else
        return 2;

    return 1;
    validateFields();
}

/*
 *  NEW ITEM
 */

NewItem::NewItem(QWidget *parent) :
    Item(parent)
{
    ui->set_PushButton->setEnabled(false);
    ui->set_PushButton->setText("Créer");

    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkManufacturerCode()));
    connect(ui->partNumber_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkItem()));

    validManufacturer = false;
    validItem = false;
}

NewItem::~NewItem()
{

}

void NewItem::accept()
{
    if (validateFields() == true)
    {
        QString newItemString;
        QString uniqueIdentifierEnabled;

        if (ui->uniqueIdentifier_CheckBox->isChecked() == true)
            uniqueIdentifierEnabled = "1";
        else
            uniqueIdentifierEnabled = "0";

        QSqlQuery newItem;
        newItemString = QString("INSERT INTO item "
                                    "(manufacturer_code, part_number, product_name, localisation, maximum, "
                                    "minimum, unique_identifier_enabled) "
                                "VALUES "
                                    "(%1, %2, %3, %4, %5, %6, %7) ")
                                .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text())).arg(toQueryTextString(ui->partNumber_LineEdit->text()))
                                .arg(toQueryTextString(ui->name_LineEdit->text())).arg(toQueryTextString(ui->localisation_LineEdit->text()))
                                .arg(toQueryTextString(ui->maximum_SpinBox->text())).arg(toQueryTextString(ui->minimum_SpinBox->text()))
                                .arg(toQueryTextString(uniqueIdentifierEnabled));

        newItem.exec(newItemString);
        if (newItem.lastError().isValid())
            QERMessageBox(newItem.lastError().text());
        else
            QDialog::accept();
    }
    else
    {
        QStringList invalidElementList;
        if (ui->manufacturerCode_LineEdit->text().isEmpty() || validManufacturer == false)
            invalidElementList.append("Fabriquant");
        else if (ui->partNumber_LineEdit->text().isEmpty() || validItem == false)
            invalidElementList.append("Article");
        else if (ui->name_LineEdit->text().isEmpty())
            invalidElementList.append("Nom");

        QString invalidElement;
        if (invalidElementList.count() > 1)
        {
            for (int i = 0; invalidElementList.count(); i++)
            {
                invalidElement.append(invalidElementList.at(i));

                if (i == 0)
                    invalidElement.append(",");
                else if(i == 1)
                    invalidElement.append(" et ");
            }
        }

        bool ret = IVAMessageBox(invalidElement);

        if (ret == true)
        {
            if (ui->partNumber_LineEdit->text().isEmpty() || validItem == false)
            {
                ui->partNumber_LineEdit->setFocus();
                ui->partNumber_LineEdit->selectAll();
            }
            else if (ui->name_LineEdit->text().isEmpty())
            {
                ui->name_LineEdit->setFocus();
                ui->name_LineEdit->selectAll();
            }
            else
            {
                ui->manufacturerCode_LineEdit->setFocus();
                ui->manufacturerCode_LineEdit->selectAll();
            }
        }
    }
}

int NewItem::checkManufacturerCode()
{
    int ret = Item::checkManufacturerCode();

    if (ret == 0)
        validManufacturer = true;
    else if (ret == 2)
    {
        ui->manufacturerCode_LineEdit->blockSignals(true);

        bool ret = EELMessageBox("Code de fabriquant");

        if (ret == true)
            ui->manufacturerCode_LineEdit->setFocus();

        ui->manufacturerCode_LineEdit->blockSignals(false);
    }

    return 0;
}

bool NewItem::validateFields()
{
    if (validManufacturer == true && validItem == true && !ui->manufacturerCode_LineEdit->text().isEmpty()
            && !ui->partNumber_LineEdit->text().isEmpty() && !ui->name_LineEdit->text().isEmpty())
    {
        ui->set_PushButton->setEnabled(true);
        return true;
    }
    else
        ui->set_PushButton->setEnabled(false);

    return false;
}

void NewItem::checkItem()
{
    QString manufacturerCode = ui->manufacturerCode_LineEdit->text();
    QString partNumber = ui->partNumber_LineEdit->text();

    QSqlQuery checkItem;
    QString checkItemString = QString("SELECT product_name "
                                      "FROM item "
                                      "WHERE manufacturer_code = %1 AND part_number = %2 ")
                                      .arg(toQueryTextString(manufacturerCode))
                                      .arg(toQueryTextString(partNumber));
    checkItem.exec(checkItemString);

    QStringList nameList;

    while(checkItem.next())
    {
        nameList << checkItem.value(0).toString();
    }

    if (nameList.length() == 1)
    {
        ui->partNumber_LineEdit->blockSignals(true);

        bool ret = AEXMessageBox(QString("L'article %1").arg(nameList.at(0)));

        if (ret == false)
        {
            ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->selectAll();

            validItem = false;
        }
        else
        {
            UpdateItem *updateItem = new UpdateItem(this);
            updateItem->openItem(manufacturerCode, partNumber);
            updateItem->show();

            QDialog::reject();
        }
        ui->partNumber_LineEdit->blockSignals(false);
    }
    else
        validItem = true;

    validateFields();
}

/*
 *  UPDATE ITEM
 */

UpdateItem::UpdateItem(QWidget *parent) :
    Item(parent)
{
    ui->uniqueIdentifier_CheckBox->setEnabled(false);
    ui->set_PushButton->setEnabled(false);
    ui->set_PushButton->setText("Modifier");
    this->setWindowTitle("Modifier un article");

    connect(ui->partNumber_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(openItem()));
    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkManufacturerCode()));
}

UpdateItem::~UpdateItem()
{

}

bool UpdateItem::validateFields()
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

void UpdateItem::accept()
{
    if (validateFields() == true)
    {
        QSqlQuery updateItem;
        QString updateItemString = QString ("UPDATE item "
                                            "SET "
                                            "product_name = %1, "
                                            "localisation = %2, "
                                            "maximum = %3, "
                                            "minimum = %4 "
                                            "WHERE manufacturer_code = %5 AND part_number = %6 ")
                                            .arg(toQueryTextString(ui->name_LineEdit->text())).arg(toQueryTextString(ui->localisation_LineEdit->text()))
                                            .arg(toQueryTextString(QString::number(ui->maximum_SpinBox->value())))
                                            .arg(toQueryTextString(QString::number(ui->minimum_SpinBox->value())))
                                            .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text())).arg(toQueryTextString(ui->partNumber_LineEdit->text()));
        updateItem.exec(updateItemString);

        updateItem.exec(updateItemString);
        if (updateItem.lastError().isValid())
            QERMessageBox(updateItem.lastError().text());
        else
            QDialog::accept();
    }
    else
    {
        bool ret = IVAMessageBox("Nom d'article");

        if (ret == true)
            ui->name_LineEdit->setFocus();
    }
}

void UpdateItem::openItem(QString manufacturer, QString part)
{
    ui->manufacturerCode_LineEdit->clear();
    ui->partNumber_LineEdit->clear();
    ui->name_LineEdit->clear();
    ui->localisation_LineEdit->clear();
    ui->maximum_SpinBox->clear();
    ui->minimum_SpinBox->clear();
    ui->uniqueIdentifier_CheckBox->setChecked(false);

    QSqlQuery openItem;
    QString openItemString = QString("SELECT manufacturer_code, part_number, product_name, localisation, maximum, "
                                     "minimum, unique_identifier_enabled "
                                     "FROM item "
                                     "WHERE manufacturer_code = %1 AND part_number = %2 ")
                                     .arg(toQueryTextString(manufacturer))
                                     .arg(toQueryTextString(part));
    openItem.exec(openItemString);

    openItem.first();

    QString manufacturerCode = openItem.value(0).toString();
    QString partNumber = openItem.value(1).toString();
    QString productName = openItem.value(2).toString();
    QString localisation = openItem.value(3).toString();
    int maximum = openItem.value(4).toInt();
    int minimum = openItem.value(5).toInt();
    int uniqueIdentifierEnabled = openItem.value(6).toInt();

    ui->manufacturerCode_LineEdit->setText(manufacturerCode);
    ui->partNumber_LineEdit->setText(partNumber);
    ui->name_LineEdit->setText(productName);
    ui->localisation_LineEdit->setText(localisation);
    ui->maximum_SpinBox->setValue(maximum);
    ui->minimum_SpinBox->setValue(minimum);

    if (uniqueIdentifierEnabled == 1)
        ui->uniqueIdentifier_CheckBox->setChecked(true);
    else
        ui->uniqueIdentifier_CheckBox->setChecked(false);

    validateFields();
}

void UpdateItem::openItem()
{
    ui->name_LineEdit->clear();
    ui->localisation_LineEdit->clear();
    ui->maximum_SpinBox->clear();
    ui->minimum_SpinBox->clear();
    ui->uniqueIdentifier_CheckBox->setChecked(false);

    if (!ui->partNumber_LineEdit->text().isEmpty() && ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery openItem;
        QString openItemString = QString("SELECT manufacturer_code, part_number, product_name, localisation, maximum, "
                                         "minimum, unique_identifier_enabled "
                                         "FROM item "
                                         "WHERE part_number = %1 ")
                                         .arg(toQueryTextString(ui->partNumber_LineEdit->text()));
        openItem.exec(openItemString);

        QStringList manufacturer;
        QStringList partNumber;
        QStringList name;
        QStringList localisation;
        QStringList maximum;
        QStringList minimum;
        QStringList uniqueIdentifierEnabled;

        while (openItem.next())
        {
            manufacturer << openItem.value(0).toString();
            partNumber << openItem.value(1).toString();
            name << openItem.value(2).toString();
            localisation << openItem.value(3).toString();
            maximum << openItem.value(4).toString();
            minimum << openItem.value(5).toString();
            uniqueIdentifierEnabled << openItem.value(6).toString();
        }

        if (manufacturer.length() == 1)
        {
            ui->manufacturerCode_LineEdit->setText(manufacturer.at(0));
            ui->partNumber_LineEdit->setText(partNumber.at(0));
            ui->name_LineEdit->setText(name.at(0));
            ui->localisation_LineEdit->setText(localisation.at(0));
            ui->maximum_SpinBox->setValue(maximum.at(0).toInt());
            ui->minimum_SpinBox->setValue(minimum.at(0).toInt());

            if (uniqueIdentifierEnabled.at(0) == "1")
                ui->uniqueIdentifier_CheckBox->setChecked(true);
            else
                ui->uniqueIdentifier_CheckBox->setChecked(false);

            ui->name_LineEdit->setFocus();
            ui->name_LineEdit->selectAll();
        }
        else if (manufacturer.length() > 1)
        {
            QStringList headers;
            headers << "Fabriquant" << "Numéro de pièce";
            SelectionDialog *selection = new SelectionDialog(headers, manufacturer, partNumber, this);
            selection->show();

            QString code;
            if (selection->exec() == QDialog::Accepted)
            {
                code = selection->getValue();
            }

            if (!code.isEmpty())
            {
                int pos = manufacturer.indexOf(code);

                ui->manufacturerCode_LineEdit->setText(manufacturer.at(pos));
                ui->partNumber_LineEdit->setText(partNumber.at(pos));
                ui->name_LineEdit->setText(name.at(pos));
                ui->localisation_LineEdit->setText(localisation.at(pos));
                ui->maximum_SpinBox->setValue(maximum.at(pos).toInt());
                ui->minimum_SpinBox->setValue(minimum.at(pos).toInt());

                if (uniqueIdentifierEnabled.at(pos) == "1")
                    ui->uniqueIdentifier_CheckBox->setChecked(false);
                else
                    ui->uniqueIdentifier_CheckBox->setChecked(true);

                ui->name_LineEdit->setFocus();
                ui->name_LineEdit->selectAll();
            }
            else
            {
                ui->partNumber_LineEdit->blockSignals(true);
                bool ret = IVAMessageBox("Article");

                if (ret == true)
                {
                    ui->partNumber_LineEdit->setFocus();
                    ui->partNumber_LineEdit->selectAll();
                }
                ui->partNumber_LineEdit->blockSignals(false);
            }
        }
        else
        {
            ui->partNumber_LineEdit->blockSignals(true);
            bool ret = IVAMessageBox("Article");

            if (ret == true)
                ui->partNumber_LineEdit->setFocus();
            ui->partNumber_LineEdit->blockSignals(false);
        }
    }
    else if (!ui->partNumber_LineEdit->text().isEmpty() && !ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery openItem;
        QString openItemString = QString("SELECT manufacturer_code, part_number, product_name, localisation, maximum, "
                                         "minimum, unique_identifier_enabled "
                                         "FROM item "
                                         "WHERE manufacturer_code = %1 AND part_number = %2 ")
                                         .arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()))
                                         .arg(toQueryTextString(ui->partNumber_LineEdit->text()));
        openItem.exec(openItemString);

        openItem.first();

        QString manufacturerCode = openItem.value(0).toString();
        QString partNumber = openItem.value(1).toString();
        QString productName = openItem.value(2).toString();
        QString localisation = openItem.value(3).toString();
        int maximum = openItem.value(4).toInt();
        int minimum = openItem.value(5).toInt();
        int uniqueIdentifierEnabled = openItem.value(6).toInt();

        ui->manufacturerCode_LineEdit->setText(manufacturerCode);
        ui->partNumber_LineEdit->setText(partNumber);
        ui->name_LineEdit->setText(productName);
        ui->localisation_LineEdit->setText(localisation);
        ui->maximum_SpinBox->setValue(maximum);
        ui->minimum_SpinBox->setValue(minimum);

        if (uniqueIdentifierEnabled == 1)
            ui->uniqueIdentifier_CheckBox->setChecked(true);
        else
            ui->uniqueIdentifier_CheckBox->setChecked(false);
    }
    else
    {
        QString emptyElement;
        if (ui->partNumber_LineEdit->text().isEmpty() && !ui->manufacturerCode_LineEdit->text().isEmpty())
            emptyElement = "Numéro d'article";
        else if (ui->partNumber_LineEdit->text().isEmpty() && ui->manufacturerCode_LineEdit->text().isEmpty())
            emptyElement = "Numéro d'article et fabriquant";

        ui->partNumber_LineEdit->blockSignals(true);
        bool ret = EELMessageBox(emptyElement);

        if (ret == true)
        {
            ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->selectAll();
        }
        ui->partNumber_LineEdit->blockSignals(false);
    }

    validateFields();
}

ViewItem::ViewItem(QWidget *parent) :
    UpdateItem(parent)
{
    this->setWindowTitle("Consultation d'article");
    ui->set_PushButton->setText("Ok");

    ui->name_LineEdit->setReadOnly(true);
    ui->localisation_LineEdit->setReadOnly(true);
    ui->maximum_SpinBox->setReadOnly(true);
    ui->minimum_SpinBox->setReadOnly(true);
}

ViewItem::~ViewItem()
{

}

void ViewItem::accept()
{
    QDialog::accept();
}
