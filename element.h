#ifndef ELEMENT_H
#define ELEMENT_H

#include <QDialog>
#include <QtSql>
#include <QMessageBox>
#include <QCloseEvent>
#include <QException>


/*
 *  Element
 */

class Element
{
public:
    explicit Element();
    ~Element();

    static inline QString toQueryTextString(QString str)
    {
        return "'" + str + "'";
    }

    static inline QString toQueryLikeString(QString str)
    {
        return "'%" + str + "%'";
    }


protected:
    bool IQYMessageBox(QString item, QString technician) const;
    bool TREMessageBox(QString str) const;
    bool IVAMessageBox(QString str) const;
    bool AEXMessageBox(QString str) const;
    bool EELMessageBox(QString str) const;
    bool IEXMessageBox(QString str) const;
    bool QERMessageBox(QString str) const;
    bool ITCMessageBox(QString str) const;
    bool NTRMessageBox() const;
    bool INTMessageBox() const;
    bool ROEMessageBox() const;
    bool IQTMessageBox() const;
};

/*
 *  Dialog Element
 */

class DialogElement : public QDialog,
                      public Element
{
    Q_OBJECT

    friend class WidgetElement;

public:
   explicit DialogElement(QWidget *parent = 0);
   ~DialogElement();
};

/*
 *  Widget Element
 */

class WidgetElement : public QWidget,
                      public Element
{
    Q_OBJECT

public:
   explicit WidgetElement(QWidget *parent = 0);
   ~WidgetElement();
};

#endif // ELEMENT_H
