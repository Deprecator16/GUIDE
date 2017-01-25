#include "account.h"

Account::Account(QWidget *parent) :
    DialogElement(parent)
{

}

Account::~Account()
{

}

QString Account::createAccountNumber(const QString phoneNumber)
{
    QString accountNumber;
    for(int i = 0; i < phoneNumber.length(); i++)
    {
        if (phoneNumber.at(i).isDigit() == true)
        {
            accountNumber.append(phoneNumber.at(i));
        }
    }
    return accountNumber;
}

void Account::openAccount()
{

}

void Account::openAccount(QString number)
{

}

void Account::checkAccount()
{

}
