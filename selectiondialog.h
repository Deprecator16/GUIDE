#ifndef SELECTIONDIALOG_H
#define SELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class SelectionDialog;
}

class SelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectionDialog(QStringList headers, QStringList list1, QStringList list2, QWidget *parent = 0);
    ~SelectionDialog();

    QString getValue();

private slots:
    void sendSelectedItem(int r, int c); // Get selected item and send it to parent

private:
    // Variable
    QStringList m_list1;
    QStringList m_list2;
    QStringList m_headers;

    // Fonctions
    void createTable() const;

    Ui::SelectionDialog *ui;

    QString value;
};

#endif // SELECTIONDIALOG_H
