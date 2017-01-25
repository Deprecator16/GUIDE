#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "account.h"

#include <QDialog>

namespace Ui {
class Technician;
}

class Technician : public Account
{
    Q_OBJECT

public:
    explicit Technician(QWidget *parent = 0);
    ~Technician();

protected:
    Ui::Technician *ui;
    void closeEvent(QCloseEvent *e);
};

class NewTechnician : public Technician
{
    Q_OBJECT

public:
    explicit NewTechnician(QWidget *parent = 0);
    ~NewTechnician();

protected:
    void accept();
    bool newTechnician();

protected slots:
    void checkPhoneNumber();
    bool validateFields();

private:
    bool validTechnicien;
};

class UpdateTechnician : public Technician
{
    Q_OBJECT

public:
    explicit UpdateTechnician(QWidget *parent = 0);
    ~UpdateTechnician();

    void openAccount(QString number);

protected:
    void accept();

protected slots:
    bool validateFields();
    void openAccount();
};

class ViewTechnician : public UpdateTechnician
{
    Q_OBJECT

public:
    explicit ViewTechnician(QWidget *parent =0);
    ~ViewTechnician();

protected:
    void accept();
};

#endif // CUSTOMER_H
