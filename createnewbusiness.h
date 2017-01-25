#ifndef CREATENEWBUSINESS_H
#define CREATENEWBUSINESS_H

#include "element.h"

#include <QDialog>

namespace Ui {
class CreateNewBusiness;
}

class CreateNewBusiness : public DialogElement
{
    Q_OBJECT

public:
    explicit CreateNewBusiness(QWidget *parent = 0);
    ~CreateNewBusiness();
    void accept();

private slots:
    void getBusinessDirectory();

signals:
    void doReadSettings();

private:
    Ui::CreateNewBusiness *ui;
    bool createDatabase(QString pathToDatabase);
};

#endif // CREATENEWBUSINESS_H
