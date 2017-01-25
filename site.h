#ifndef SITE_H
#define SITE_H

#include "account.h"

namespace Ui {
class Manufacturer;
}

class Site : public Account
{
    Q_OBJECT

public:
    explicit Site(QWidget *parent = 0);
    ~Site();

protected:
    Ui::Manufacturer *ui;
    void closeEvent(QCloseEvent *e);

protected slots:
    virtual bool validateFields() = 0;
};

class NewSite : public Site
{
    Q_OBJECT

public:
    explicit NewSite(QWidget *parent = 0);
    ~NewSite();

protected:
    void accept();
    bool newSite();

protected slots:
    void checkAccount();
    bool validateFields();

private:
    bool validSite;
};

class UpdateSite : public Site
{
    Q_OBJECT

public:
    explicit UpdateSite(QWidget *parent = 0);
    ~UpdateSite();

    // Function
    void openAccount(QString number);

protected:
    void accept();

protected slots:
    void openAccount();
    bool validateFields();
};

#endif // SITE_H
