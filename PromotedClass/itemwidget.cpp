#include "itemwidget.h"
#include "ui_itemwidget.h"

#include "selectiondialog.h"

ItemWidget::ItemWidget(QWidget *parent) :
    WidgetElement(parent),
    ui(new Ui::ItemWidget)
{
    ui->setupUi(this);

    ui->itemName_LineEdit->setReadOnly(true);

    connect(ui->manufacturerCode_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkManufacturer()));
    connect(ui->partNumber_LineEdit, SIGNAL(editingFinished()),
            this, SLOT(checkItem()));
}

ItemWidget::~ItemWidget()
{
    delete ui;
}

void ItemWidget::checkItem()
{
    ui->itemName_LineEdit->clear();
    emit makeUpdateQuantityLabel();

    if (!ui->partNumber_LineEdit->text().isEmpty() && ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery checkItem;
        QString checkItemString = QString("SELECT item_id, manufacturer_code, part_number, product_name, unique_identifier_enabled "
                                          "FROM item "
                                          "WHERE part_number = %1 ")
                                          .arg(toQueryTextString(ui->partNumber_LineEdit->text()));
        checkItem.exec(checkItemString);

        QStringList itemId;
        QStringList manufacturer;
        QStringList partNumber;
        QStringList productName;
        QStringList uniqueIdentifierEnabled;

        while (checkItem.next())
        {
            itemId << checkItem.value(0).toString();
            manufacturer << checkItem.value(1).toString();
            partNumber << checkItem.value(2).toString();
            productName << checkItem.value(3).toString();
            uniqueIdentifierEnabled << checkItem.value(4).toString();
        }

        if (itemId.count() == 1)
        {
            emit passFocus();
            emit validItem(itemId.at(0), manufacturer.at(0), partNumber.at(0), productName.at(0), uniqueIdentifierEnabled.at(0));
        }

        else if (itemId.count() > 1)
        {
            QStringList headers;
            headers << "Fabriquant" << "Numéro de pièce";
            SelectionDialog *selection = new SelectionDialog(headers, manufacturer, partNumber, this);
            selection->show();

            QString code;
            if (selection->exec() == QDialog::Accepted)
                code = selection->getValue();

            if (!code.isEmpty())
            {
                int pos = manufacturer.indexOf(code);
                emit passFocus();
                emit validItem(itemId.at(pos), manufacturer.at(pos), partNumber.at(pos), productName.at(pos), uniqueIdentifierEnabled.at(pos));

            }
        }
        else
        {
            IVAMessageBox("Numéro d'article");
            ui->manufacturerCode_LineEdit->selectAll();
            ui->manufacturerCode_LineEdit->setFocus();
        }
    }
    else if (!ui->partNumber_LineEdit->text().isEmpty() && !ui->manufacturerCode_LineEdit->text().isEmpty())
    {
        QSqlQuery checkItem;
        QString checkItemString = QString("SELECT item_id, manufacturer_code, part_number, product_name, unique_identifier_enabled "
                                          "FROM item "
                                          "WHERE part_number = %1 AND manufacturer_code = %2")
                                          .arg(toQueryTextString(ui->partNumber_LineEdit->text())).arg(toQueryTextString(ui->manufacturerCode_LineEdit->text()));
        checkItem.exec(checkItemString);

        QString itemId;
        QString manufacturer;
        QString partNumber;
        QString productName;
        QString uniqueIdentifierEnabled;

        if (checkItem.first() == true)
        {
            itemId = checkItem.value(0).toString();
            manufacturer = checkItem.value(1).toString();
            partNumber = checkItem.value(2).toString();
            productName = checkItem.value(3).toString();
            uniqueIdentifierEnabled = checkItem.value(4).toString();

            emit passFocus();
            emit validItem(itemId, manufacturer, partNumber, productName, uniqueIdentifierEnabled);
        }
        else
        {
            IVAMessageBox("Numéro d'article");
            ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->selectAll();
        }
    }
    else
        emit passFocus();
}

void ItemWidget::checkManufacturer()
{
    emit makeUpdateQuantityLabel();
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
            }

            if (!code.isEmpty())
            {
                ui->manufacturerCode_LineEdit->setText(code);
                ui->manufacturerCode_LineEdit->selectAll();
            }
            else
            {
                ui->manufacturerCode_LineEdit->setFocus();
                ui->manufacturerCode_LineEdit->selectAll();
                IVAMessageBox("Code de fabriquant");
            }
        }
        else
        {
            ui->manufacturerCode_LineEdit->setFocus();
            ui->manufacturerCode_LineEdit->selectAll();
            IEXMessageBox("Code de fabriquant");
        }
    }
}

void ItemWidget::getFocus()
{
    ui->manufacturerCode_LineEdit->setFocus();
    ui->manufacturerCode_LineEdit->selectAll();
}

void ItemWidget::setItemInformation(QString manufacturer, QString partNumber, QString productName)
{
    ui->manufacturerCode_LineEdit->setText(manufacturer);
    ui->partNumber_LineEdit->setText(partNumber);
    ui->itemName_LineEdit->setText(productName);
}

QString ItemWidget::manufacturer()
{
    return ui->manufacturerCode_LineEdit->text();
}

QString ItemWidget::partNumber()
{
    return ui->partNumber_LineEdit->text();
}

void ItemWidget::clearItemFields()
{
    ui->manufacturerCode_LineEdit->clear();
    ui->partNumber_LineEdit->clear();
    ui->itemName_LineEdit->clear();

    ui->manufacturerCode_LineEdit->setFocus();
}
