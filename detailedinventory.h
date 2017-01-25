#ifndef DETAILEDINVENTORY_H
#define DETAILEDINVENTORY_H

#include <QDialog>

namespace Ui {
class DetailedInventory;
}

class DetailedInventory : public QDialog
{
    Q_OBJECT

public:
    explicit DetailedInventory(QWidget *parent = 0);
    ~DetailedInventory();

    void getItem(QString manufacturer, QString partNumber);

private:
    Ui::DetailedInventory *ui;

    void showDetails(QString manufacturer, QString partNumber);
};

#endif // DETAILEDINVENTORY_H
