#ifndef SUPPLIER_H
#define SUPPLIER_H

#include "account.h"

#include <QDialog>

namespace Ui {
class Supplier;
}

class Supplier : public Account
{
    Q_OBJECT

public:
    explicit Supplier(QWidget *parent = 0);
    ~Supplier();

protected:
    void newAccount(QString id, QString name, QString phoneNumber, QString email, QString note);
    void updateAccount(QString id, QString name, QString phoneNumber, QString email, QString note);
    Ui::Supplier *ui;
};

class NewSupplier : public Supplier
{
    Q_OBJECT

public:
    explicit NewSupplier(QWidget *parent = 0);
    ~NewSupplier();

protected:
    void accept();

protected slots:
    void checkPhoneNumber();
    bool validateFields();
};

class UpdateSupplier : public Supplier
{
    Q_OBJECT

public:
    explicit UpdateSupplier(QWidget *parent = 0);
    ~UpdateSupplier();

    void openAccount(QString number);

protected:
    void accept();

protected slots:
    bool validateFields();
};

#endif // SUPPLIER_H
