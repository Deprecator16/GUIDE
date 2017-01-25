#ifndef ITEM_H
#define ITEM_H

#include "element.h"
#include "ui_item.h"

namespace Ui {
class Item;
}

/*
 *  ITEM
 */

class Item : public DialogElement
{
    Q_OBJECT

public:
    explicit Item(QWidget *parent = 0);
    ~Item();

protected:
    Ui::Item *ui;
    void closeEvent(QCloseEvent *e);

protected slots:
    virtual bool validateFields() = 0;
    int checkManufacturerCode();
};

/*
 *  NEW ITEM
 */

class NewItem : public Item
{
    Q_OBJECT

public :
    explicit NewItem(QWidget *parent = 0);
    ~NewItem();
    void accept();

protected slots:
    bool validateFields();
    int checkManufacturerCode();

private slots:
    void checkItem();

private:
    // Variable
    bool validManufacturer;
    bool validItem;
};

/*
 *  UPDATE ITEM
 */

class UpdateItem : public Item
{
    Q_OBJECT

public:
    explicit UpdateItem(QWidget *parent = 0);
    ~UpdateItem();
    void accept();

    // Function
    void openItem(QString manufacturer, QString part);

protected slots:
    bool validateFields();
    void openItem();

private:
    int previousQuantity;
    QStringList newUniqueIdentifier;
};

class ViewItem : public UpdateItem
{
    Q_OBJECT

public:
    explicit ViewItem(QWidget *parent = 0);
    ~ViewItem();

protected:
    void accept();
};

#endif // ITEM_H
