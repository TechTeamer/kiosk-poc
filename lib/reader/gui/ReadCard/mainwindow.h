#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define PrOutputString QString
#define ToPrOutputString Pr22::Util::ToOutString

#include <QMainWindow>
#include "Pr22.hpp"
#include <QStringListModel>
#include <QLabel>
#include <QMetaEnum>
#include <QThread>
#include <QSettings>
#include <QFileDialog>
#include <QDir>
#include <QMenu>
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
    void OnAuthBegin(const ECardHandling::AuthProcess & authentication);
    void OnAuthFinished(const ECardHandling::AuthProcess & authentication, const Exceptions::ErrorCodes & status);
    void OnAuthWaitForInput(const ECardHandling::AuthProcess & authentication);
    void OnReadBegin(const ECardHandling::File & fileId);
    void OnReadFinished(const ECardHandling::File & fileId, const Exceptions::ErrorCodes & status);
    void OnFileChecked(const ECardHandling::File & fileId, const Exceptions::ErrorCodes & status);

signals:
    void Connection(QStringList stringList);
    void AuthBegin(const ECardHandling::AuthProcess & authentication);
    void AuthFinished(const ECardHandling::AuthProcess & authentication, const Exceptions::ErrorCodes & status);
    void AuthWaitForInput(const ECardHandling::AuthProcess & authentication);
    void ReadBegin(ECardHandling::File file);
    void ReadFinished(ECardHandling::File file, const Exceptions::ErrorCodes & status);
    void FileChecked(ECardHandling::File file, const Exceptions::ErrorCodes & status);
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
    Pr22::ECard card;
    Task::TaskControl readCtrl;
    Document vizResult;
    Document faceDoc;

    void ClearControls();
    void DrawImage(QLabel* lbl, QPixmap pm);
    void ColorAuthLabel(ECardHandling::AuthProcess auth, const QString &color);
    void ColorFileName(ECardHandling::File file, const QString &color);
    void ColorLabel(QLabel *label, const QString &color);

    void StartRead(Pr22::ECardReader *cardReader);
    void ProcessAfterFileRead(const ECardHandling::File &file);
    void ProcessAfterAllRead();

    void LoadCertificates(const QString &dir);
    QStringList FileList(const QString &dirname, const QStringList &masks);

    void SaveFile(QString text);

private slots:
    void GetDeviceModel(QStringList stringList);
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonRead_clicked();

    void AuthBegin(const ECardHandling::AuthProcess & authentication);
    void AuthFinished(const ECardHandling::AuthProcess & authentication, const Exceptions::ErrorCodes & status);
    void AuthWaitForInput(const ECardHandling::AuthProcess & authentication);
    void ReadBegin(ECardHandling::File file);
    void ReadFinished(ECardHandling::File file, const Exceptions::ErrorCodes & status);
    void FileChecked(ECardHandling::File file, const Exceptions::ErrorCodes & status);

    void closeEvent(QCloseEvent* event);

    void on_listWidgetFiles_customContextMenuRequested(const QPoint &pos);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
