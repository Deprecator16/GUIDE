#ifndef MANUFACTURER_H
#define MANUFACTURER_H

#include "account.h"

namespace Ui {
class Manufacturer;
}

/*
 *  Manufacturer
 */

class Manufacturer : public Account
{
    Q_OBJECT

public:
    explicit Manufacturer(QWidget *parent = 0);
    ~Manufacturer();

protected:
    Ui::Manufacturer *ui;
    void closeEvent(QCloseEvent *e);

protected slots:
    virtual bool validateFields() = 0;
};

/*
 *  New Manufacturer
 */

class NewManufacturer : public Manufacturer
{
    Q_OBJECT

public:
    explicit NewManufacturer(QWidget *parent = 0);
    ~NewManufacturer();

protected:
    void accept();

protected slots:
    void checkAccount();
    bool validateFields();

private:
    bool validManufacturer;
};

/*
 *  Update Manufacturer
 */

class UpdateManufacturer : public Manufacturer
{
    Q_OBJECT

public:
    explicit UpdateManufacturer(QWidget *parent = 0);
    ~UpdateManufacturer();

    // Function
    void openAccount(QString number);

protected:
    void accept();

protected slots:
    bool validateFields();
    void openAccount();
};

#endif // MANUFACTURER_H
