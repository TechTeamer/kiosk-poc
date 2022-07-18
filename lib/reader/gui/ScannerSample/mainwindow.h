#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define PrOutputString QString
#define ToPrOutputString Pr22::Util::ToOutString

#include <QMainWindow>
#include "Pr22.hpp"
#include <QStringListModel>
#include <QLabel>
#include <QMetaType>
#include <QStandardItemModel>
#include <QVariant>
#include <QBitmap>
#include <QMessageBox>

using namespace Pr22;
using namespace Pr22::Processing;

typedef Util::ToOutString ToQString;

template<class T>
T string2enum(const QString &text) {
    return Util::EnumInfo<T>::Parse((const ushort*)text.constData());
}

#define REGION(x) extern "C++"
#define ENDREGION

namespace Ui {
class MainWindow;
}

class EventHandler :  public QObject, public Events::DocEventHandler
{
    Q_OBJECT

public:
    explicit EventHandler(QObject *parent = 0) { (void) parent; }
    ~EventHandler() { }

    void RegisterEvents(Pr22::DocumentReaderDevice* PR);

private:
    void OnConnection(int device);
    void OnDocFrameFound(int page, const Exceptions::ErrorCodes &);
    void OnScanFinished(int, const Exceptions::ErrorCodes &);
    void OnImageScanned(int page, const Imaging::Light & light);
    void OnPresenceStateChanged(const Util::PresenceState & state);

signals:
    void Connection(QStringList stringList);
    void ImageScanned(int page, Imaging::Light);
    void ScanFinished();
    void DocFrameFound(int page);
    void PresenceStateChanged();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Pr22::DocumentReaderDevice* pr;
    EventHandler* eventHandler;

    bool DeviceIsConnected;
    Document AnalyzeResult;

    void AddTabPage(const QString &lightName);
    void DrawImage(QLabel* lbl, QPixmap pm);
    QString GetFieldValue(const Processing::FieldId &Id);
    QString GetAmid(const Field &field);
    void FillOcrDataGrid();
    void FillDataPage();
    void ClearOCRData();
    void ClearDataPage();
    void RemoveTabs();
    void RemoveGridRows();

    Task::TaskControl ScanCtrl;

    QStandardItemModel* modelGrid;

    QString PrintBinary(const std::vector<gxu8> &arr, int pos, int sz);

    void UpdateFieldDisplay();

private slots:
    void GetDeviceModel(QStringList stringList);
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonStart_clicked();

    void DrawImage(int page, Imaging::Light light);
    void CloseScan();
    void Analyze();
    void RunDocFrameFound(int page);
    void closeEvent(QCloseEvent* event);
    bool eventFilter(QObject* obj, QEvent* ev);
    void on_tableViewGrid_clicked(const QModelIndex&);

private:
    Ui::MainWindow*     ui;
};

#endif // MAINWINDOW_H
