#ifndef ITEMTABLE_H
#define ITEMTABLE_H

#include "element.h"
#include <QTableWidget>

class ItemTable : public WidgetElement
{
    Q_OBJECT

public:
    explicit ItemTable(QWidget *parent = 0);
    ~ItemTable();

    void addItem(const QString itemId, const QString manufacturerCode, const QString partNumber, const QString quantity);
    void addItem(const QString itemId, const QString manufacturerCode, const QString partNumber, const QString quantity, const QString uniqueIdentifier);
    void addComment(const QString comment);
    int rowCount();

signals:
    void itemAdded();
    void commentAdded();

private:
    QTableWidget *tableWidget;
};


#endif // ITEMTABLE_H
