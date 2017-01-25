#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "element.h"

#include <QMainWindow>
#include <QDockWidget>
#include <QTableWidget>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
   // void showTable(QListWidgetItem *item);
    bool openBusiness(QString pathToDatabase);
    void readSettings();

    void openNewItem();
    void openEditItem();
    void openViewItem();
    void openNewManufacturer();
    void openEditManufacturer();
    void openNewSupplier();
    void openEditSupplier();
    void openViewSupplier();
    void openNewTechnicien();
    void openEditTechnicien();
    void openViewTechnicien();
    void openNewInstallation();
    void openNewPurchase();
    void openReceivePurchase();
    void openNewInventoryTransfer();
    void openNewSite();
    void openEditSite();
    void openNewAdjustment();
/*
    // Open dialog
    void openSelectedItem(int r, int c);
    void openSelectedManufacturer(int r, int c);
    void openSelectedSupplier(int r, int c);
    void openSelectedTechnician(int r, int c);

    // Refine
    void refineItem(bool underMinimum, bool overMaximum, QString manufacturer, QString name, QString localisation);
    void refineManufacturer(QString name, QString code);
    void refineTechnician(QString number, QString name);
    void refineSupplier(QString number, QString name);*/

private:
    // Fonction
    //void createDockWidget();
    void writeSettings();
    //void disconnectUnusedTableSlots();
    void openNewBusinessDialog();

    // Widget
   // QTableWidget *table;

  //  QDockWidget *refine_DockWidget;
   // QDockWidget *inventory_DockWidget;
   // QDockWidget *invoice_DockWidget;
   // QDockWidget *account_DockWidget;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
