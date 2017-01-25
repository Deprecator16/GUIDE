#include "supplier.h"
#include "ui_supplier.h"

#include "selectiondialog.h"

Supplier::Supplier(QWidget *parent) :
    Account(parent),
    ui(new Ui::Supplier)
{
    ui->setupUi(this);
}

Supplier::~Supplier()
{

}

void Supplier::newAccount(QString id, QString name, QString phoneNumber, QString email, QString note)
{
    QSqlQuery newSupplier;
    QString newSupplierString = QString("INSERT INTO suppliers"
                                        "(supplier_id, supplier_name, phone_number, email, note)"
                                        "VALUES"
                                        "(%1, %2, %3, %4, %5)")
                                        .arg(toQueryTextString(id))
                                        .arg(toQueryTextString(name))
                                        .arg(toQueryTextString(phoneNumber))
                                        .arg(toQueryTextString(email))
                                        .arg(toQueryTextString(note));
    if (!newSupplier.exec(newSupplierString))
    {
        qDebug() << "[SQL Error at Supplier -> newAccount()] " << newSupplier.lastError();
        QMessageBox msgBox;
        msgBox.setText(QString("SQL Error"));
        msgBox.setInformativeText(newSupplier.lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}


void Supplier::updateAccount(QString id, QString name, QString phoneNumber, QString email, QString note)
{
    QSqlQuery updateSupplier;
    QString updateSupplierString = QString("UPDATE suppliers "
                                           "SET "
                                           "supplier_name = %1, "
                                           "phone_number = %2, "
                                           "email = %3, "
                                           "note = %4 "
                                           "WHERE supplier_id = %5 ")
                                           .arg(toQueryTextString(name))
                                           .arg(toQueryTextString(phoneNumber))
                                           .arg(toQueryTextString(email))
                                           .arg(toQueryTextString(note))
                                           .arg(toQueryTextString(id));
    if (!updateSupplier.exec(updateSupplierString))
    {
        qDebug() << "[SQL Error at Supplier -> updateAccount()] " << updateSupplier.lastError();
        QMessageBox msgBox;
        msgBox.setText(QString("SQL Error"));
        msgBox.setInformativeText(updateSupplier.lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

NewSupplier::NewSupplier(QWidget *parent) :
    Supplier(parent)
{
    ui->supplier_LineEdit->setReadOnly(true);
    ui->phoneNumber_LineEdit->setFocus();

    ui->set_PushButton->setText("Create");

    connect(ui->cancel_PushButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    connect(ui->set_PushButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(ui->phoneNumber_LineEdit, SIGNAL(returnPressed()),
            this, SLOT(checkPhoneNumber()));

    // Navigation
    setTabOrder(ui->phoneNumber_LineEdit, ui->name_LineEdit);
    connect(ui->phoneNumber_LineEdit, SIGNAL(returnPressed()),
            ui->name_LineEdit, SLOT(setFocus()));
    setTabOrder(ui->name_LineEdit, ui->email_LineEdit);
    connect(ui->name_LineEdit, SIGNAL(returnPressed()),
            ui->email_LineEdit, SLOT(setFocus()));
    setTabOrder(ui->email_LineEdit, ui->note_PlainTextEdit);
    connect(ui->email_LineEdit, SIGNAL(returnPressed()),
            ui->note_PlainTextEdit, SLOT(setFocus()));
}

NewSupplier::~NewSupplier()
{

}

void NewSupplier::accept()
{
    QString supplierId = ui->supplier_LineEdit->text();
    QString supplierName = ui->name_LineEdit->text();

    if (!supplierId.isEmpty() && !supplierName.isEmpty())
    {
        QString supplierPhoneNumber = ui->phoneNumber_LineEdit->text();
        QString supplierEmail = ui->email_LineEdit->text();
        QString supplierNote = ui->note_PlainTextEdit->toPlainText();

        newAccount(supplierId, supplierName, supplierPhoneNumber, supplierEmail, supplierNote);
        QDialog::accept();
    }
    else
    {
        QString emptyElement;
        if (supplierId.isEmpty())
            emptyElement.append("account number ");
        if (supplierName.isEmpty())
            emptyElement.append("name ");

        QMessageBox msgBox;
        msgBox.setText(QString("Invalid %1format"));
        msgBox.setInformativeText("Please enter a valid value to continue");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok)
        {
            if (supplierId.isEmpty())
                ui->supplier_LineEdit->setFocus();
            else
                ui->name_LineEdit->setFocus();
        }
    }
}

bool NewSupplier::validateFields()
{
    return true;
}

void NewSupplier::checkPhoneNumber()
{
    QString supplierPhoneNumber = ui->phoneNumber_LineEdit->text();
    QString id = createAccountNumber(supplierPhoneNumber);

    QSqlQuery checkSupplier;
    QString checkSupplierString = QString("SELECT supplier_id, supplier_name FROM suppliers "
                                         "WHERE supplier_id = %1")
                                         .arg(toQueryTextString(id));
    checkSupplier.exec(checkSupplierString);

    QStringList idList;
    QStringList nameList;

    while (checkSupplier.next())
    {
        idList << checkSupplier.value(0).toString();
        nameList << checkSupplier.value(1).toString();
    }

    if (idList.length() == 1)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("The account %1 already exist").arg(idList.at(0)));
        msgBox.setInformativeText("Do you want to open this account");
        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        if (ret == QMessageBox::No)
        {
            ui->supplier_LineEdit->clear();
            ui->name_LineEdit->clear();
            ui->phoneNumber_LineEdit->clear();
            ui->email_LineEdit->clear();
            ui->note_PlainTextEdit->clear();

            ui->phoneNumber_LineEdit->setFocus();
        }
        else
        {
            UpdateSupplier *updateSupplier = new UpdateSupplier(this);
            updateSupplier->openAccount(id);
            updateSupplier->show();

            QDialog::reject();
        }
    }
    else
    {
        ui->supplier_LineEdit->setText(id);
    }
}

UpdateSupplier::UpdateSupplier(QWidget *parent) :
    Supplier(parent)
{
    ui->supplier_LineEdit->setReadOnly(true);
    ui->name_LineEdit->setFocus();
    ui->set_PushButton->setText("Update");

    connect(ui->cancel_PushButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    connect(ui->set_PushButton, SIGNAL(clicked()),
            this, SLOT(accept()));

    // Navigation
    setTabOrder(ui->name_LineEdit, ui->phoneNumber_LineEdit);
    connect(ui->name_LineEdit, SIGNAL(returnPressed()),
            ui->phoneNumber_LineEdit, SLOT(selectAll()));
    setTabOrder(ui->phoneNumber_LineEdit, ui->email_LineEdit);
    connect(ui->phoneNumber_LineEdit, SIGNAL(returnPressed()),
            ui->email_LineEdit, SLOT(selectAll()));
    setTabOrder(ui->email_LineEdit, ui->note_PlainTextEdit);
    connect(ui->email_LineEdit, SIGNAL(returnPressed()),
            ui->note_PlainTextEdit, SLOT(selectAll()));
}

UpdateSupplier::~UpdateSupplier()
{

}

bool UpdateSupplier::validateFields()
{
    return true;
}

void UpdateSupplier::accept()
{
    QString supplierName = ui->name_LineEdit->text();

    if (!supplierName.isEmpty())
    {
        QString supplierId = ui->supplier_LineEdit->text();
        QString supplierPhoneNumber = ui->phoneNumber_LineEdit->text();
        QString supplierEmail = ui->email_LineEdit->text();
        QString supplierNote = ui->note_PlainTextEdit->toPlainText();

        updateAccount(supplierId, supplierName, supplierPhoneNumber, supplierEmail, supplierNote);
        QDialog::accept();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Invalid name format"));
        msgBox.setInformativeText("Please enter a valid value to continue");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Ok)
        {
            ui->name_LineEdit->setFocus();
        }
    }
}

void UpdateSupplier::openAccount(QString number)
{
    QSqlQuery openSupplier;
    QString openSupplierString = QString("SELECT supplier_id, supplier_name, phone_number, email, note FROM suppliers "
                                         "WHERE supplier_id = %1")
                                         .arg(toQueryTextString(number));
    openSupplier.exec(openSupplierString);

    openSupplier.first();

    QString supplierId = openSupplier.value(0).toString();
    QString supplierName = openSupplier.value(1).toString();
    QString supplierPhoneNumber = openSupplier.value(2).toString();
    QString supplierEmail = openSupplier.value(3).toString();
    QString supplierNote = openSupplier.value(4).toString();

    ui->supplier_LineEdit->setText(supplierId);
    ui->name_LineEdit->setText(supplierName);
    ui->phoneNumber_LineEdit->setText(supplierPhoneNumber);
    ui->email_LineEdit->setText(supplierEmail);
    ui->note_PlainTextEdit->setPlainText(supplierNote);
}
