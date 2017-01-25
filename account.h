#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "element.h"

class Account : public DialogElement
{
public:
    explicit Account(QWidget *parent = 0);
    ~Account();

    // Function
    void openAccount(QString number);

protected:
    QString createAccountNumber(const QString phoneNumber);

protected slots:
    void checkAccount();
    void openAccount();
    virtual bool validateFields() = 0;
};

#endif // ACCOUNT_H
