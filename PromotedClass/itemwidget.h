#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "element.h"

namespace Ui {
class ItemWidget;
}

class ItemWidget : public WidgetElement
{
    Q_OBJECT

public:
    explicit ItemWidget(QWidget *parent = 0);
    ~ItemWidget();

    void getFocus();
    void setItemInformation(QString manufacturer, QString partNumber, QString productName);
    QString manufacturer();
    QString partNumber();
    void clearItemFields();

protected slots:
    void checkManufacturer();
    void checkItem();

signals:
    void validItem(QString itemId, QString manufacturer, QString partNumber, QString productName,  QString uniqueIdentifierEnabled);
    void passFocus();
    void makeUpdateQuantityLabel();

private:
    Ui::ItemWidget *ui;
};

#endif // ITEMWIDGET_H
