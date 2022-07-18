#include "mainwindow.h"
#include "ui_mainwindow.h"

// This class makes string concatenation with spaces and prefixes.
class StrCon
{
    QChar* nullchar() { static QChar val; return &val; }
    const QChar *fstr, *cstr;
    StrCon(const QChar *str, const StrCon &csv) : fstr(str), cstr(csv.cstr) { }

    static QString con(const QChar *fstr, const QString &lstr) {
        return !fstr->isNull() ? QString(fstr).append(lstr[0]!=',' ? " " : "").append(lstr) : lstr;
    }
public:
    StrCon() : fstr(nullchar()), cstr(nullchar()) { }
    StrCon(const QString &bounder) : fstr(nullchar()), cstr(bounder.constData()) { }

    friend QString operator +(const StrCon &csv, const QString &str) {
        if(str.isEmpty()) return QString(csv.fstr);
        return con(csv.fstr, con(csv.cstr, str));
    }

    friend StrCon operator +(const QString &str, const StrCon &csv) {
        return StrCon(str.constData(), csv);
    }
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    pr(0),
    eventHandler(0),
    DeviceIsConnected(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->installEventFilter(this);

    setFixedSize(width(), height());

    try {
        pr = new Pr22::DocumentReaderDevice();
    }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
        exit(1);
    }

    eventHandler = new EventHandler();

    qRegisterMetaType<Imaging::Light>("Imaging::Light");

    connect(eventHandler, SIGNAL(Connection(QStringList)), this, SLOT(GetDeviceModel(QStringList)));
    connect(eventHandler, SIGNAL(ImageScanned(int, Imaging::Light)), this, SLOT(DrawImage(int, Imaging::Light)));
    connect(eventHandler, SIGNAL(ScanFinished()), this, SLOT(Analyze()));
    connect(eventHandler, SIGNAL(ScanFinished()), this, SLOT(CloseScan()));
    connect(eventHandler, SIGNAL(DocFrameFound(int)), this, SLOT(RunDocFrameFound(int)));
    connect(eventHandler, SIGNAL(PresenceStateChanged()), this, SLOT(on_pushButtonStart_clicked()));

    eventHandler->RegisterEvents(pr);

    QStringList strList;
    strList << "MRZ" << "VIZ" << "BCR";
    foreach(const QString &field, strList) {
        QListWidgetItem* item = new QListWidgetItem(field + " fields", ui->listWidgetOCR);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    modelGrid = new QStandardItemModel();
    modelGrid->setHorizontalHeaderItem(0, new QStandardItem("Field ID"));
    modelGrid->setHorizontalHeaderItem(1, new QStandardItem("Value"));
    modelGrid->setHorizontalHeaderItem(2, new QStandardItem("Status"));
    modelGrid->setHorizontalHeaderItem(3, new QStandardItem("ID"));

    ui->tableViewGrid->setModel(modelGrid);

    // The following lines have to be placed after tableView's 'setModel'
    ui->tableViewGrid->setColumnWidth(0, 160);
#if QT_VERSION > 0x050000
    ui->tableViewGrid->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
#endif
    ui->tableViewGrid->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableViewGrid->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableViewGrid->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableViewGrid->setColumnHidden(3, true);
}

void EventHandler::RegisterEvents(Pr22::DocumentReaderDevice* PR)
{
    PR->AddEventHandler(Events::Event::Connection, this);
    PR->AddEventHandler(Events::Event::PresenceDetection, this);
    PR->AddEventHandler(Events::Event::ImageScanned, this);
    PR->AddEventHandler(Events::Event::ScanFinished, this);
    PR->AddEventHandler(Events::Event::DocFrameFound, this);
}

void MainWindow::closeEvent(QCloseEvent*)
{
    if(DeviceIsConnected) {
        CloseScan();
        QApplication::processEvents();
        pr->Close();
    }
}

MainWindow::~MainWindow()
{
    delete eventHandler;
    delete pr;
    delete ui;
}

REGION("Connection") {
//------------------------------------------------------------------------------

// This raises only when no device is used or when the currently used
// device is disconnected.
void EventHandler::OnConnection(int)
{
    std::vector<QString> list = Pr22::DocumentReaderDevice::GetDeviceList();
    QStringList stringList;
    for(unsigned int ix = 0; ix < list.size(); ix++)
        stringList << list[ix];

    emit Connection(stringList);
}

void MainWindow::GetDeviceModel(QStringList stringList)
{
    QStringListModel* listModel = new QStringListModel(stringList, NULL);
    ui->listViewDevices->setModel(listModel);
}

void MainWindow::on_pushButtonConnect_clicked()
{
    QModelIndex index = ui->listViewDevices->currentIndex();
    QString item = index.data(Qt::DisplayRole).toString();

    if(item.length() <= 0) return;

    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonConnect->repaint();
    setCursor(Qt::WaitCursor);
    try {
        pr->UseDevice(item);

        DeviceIsConnected = true;
        pr->GetScanner().StartTask(Task::FreerunTask::Detection());
        ui->pushButtonDisconnect->setEnabled(true);
        std::vector<Imaging::Light> Lights = pr->GetScanner().GetInfo().GetLights();
        for(unsigned int ix = 0; ix < Lights.size(); ix++)
        {
            QString lightName = (ToQString)Lights[ix].ToString();
            QListWidgetItem* item = new QListWidgetItem(lightName, ui->listWidgetLights);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }
        ui->pushButtonStart->setEnabled(true);
    }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
        on_pushButtonDisconnect_clicked();
    }
    setCursor(Qt::ArrowCursor);
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    if(DeviceIsConnected) {
        CloseScan();
        QApplication::processEvents();
        pr->Close();
        DeviceIsConnected = false;
    }
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonStart->setEnabled(false);
    ui->listWidgetLights->clear();
    RemoveTabs();
    RemoveGridRows();
    ClearOCRData();
    ClearDataPage();
}

ENDREGION }

REGION("Scanning") {
//------------------------------------------------------------------------------

// To raise this event FreerunTask.Detection() has to be started.
void EventHandler::OnPresenceStateChanged(const Util::PresenceState & state)
{
    if(state == Util::PresenceState::Present)
        emit PresenceStateChanged();
}

void MainWindow::on_pushButtonStart_clicked()
{
    RemoveTabs();
    RemoveGridRows();
    ClearOCRData();
    ClearDataPage();

    Task::DocScannerTask ScanTask;

    int count = 0;
    for(int ix = 0; ix < ui->listWidgetLights->count(); ix++)
    {
        QListWidgetItem *item = ui->listWidgetLights->item(ix);
        if(item->checkState() == Qt::Checked)
        {
            AddTabPage(item->text());
            ScanTask.Add(string2enum<Imaging::Light>(item->text()));
            count++;
        }
    }
    if(count == 0) {
        QMessageBox::information(this, "Info", "No light selected to scan!");
        return;
    }

    ui->pushButtonStart->setEnabled(false);
    try {
        ScanCtrl = pr->GetScanner().StartScanning(ScanTask, Imaging::PagePosition::First);
    }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }
}

void EventHandler::OnImageScanned(int page, const Imaging::Light& light)
{
    emit ImageScanned(page, light);
}

// To rotate the document to upside down direction the Analyze() should
// be called.
void EventHandler::OnDocFrameFound(int page, const Exceptions::ErrorCodes &)
{
    emit DocFrameFound(page);
}

void MainWindow::RunDocFrameFound(int page)
{
    if(!ui->checkBoxDocView->isChecked()) return;

    for(int ix = 0; ix < ui->tabWidgetRight->count(); ++ix) {
        QLabel *label = dynamic_cast<QLabel*>(ui->tabWidgetRight->widget(ix));
        if(label && label->pixmap()) {
            Imaging::Light light = string2enum<Imaging::Light>(ui->tabWidgetRight->tabText(ix));
            DrawImage(page, light);
        }
    }
}

void MainWindow::DrawImage(int page, Imaging::Light light)
{
    Imaging::DocImage docImage = pr->GetScanner().GetPage(page).Select(light);
    QString lightName = (ToQString)light.ToString();

    for(int ix = 0; ix < ui->tabWidgetRight->count(); ix++)
    {
        if(ui->tabWidgetRight->tabText(ix) == lightName)
        {
            QPixmap pm;
            if(ui->checkBoxDocView->isChecked())
            {
                try { pm = docImage.DocView().GetImage().ToQImage<QPixmap>(); }
                catch(const Exceptions::General &) { }
            }
            if(pm.isNull()) pm = docImage.GetImage().ToQImage<QPixmap>();

            DrawImage((QLabel*)ui->tabWidgetRight->widget(ix), pm);
        }
    }
}

void EventHandler::OnScanFinished(int, const Exceptions::ErrorCodes&)
{
    emit ScanFinished();
}

void MainWindow::CloseScan()
{
    try { ScanCtrl.Wait(); }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }

    ui->pushButtonStart->setEnabled(true);
}

ENDREGION }

REGION("Analyzing") {
//------------------------------------------------------------------------------

void MainWindow::Analyze()
{
    Task::EngineTask OcrTask;

    if(ui->listWidgetOCR->item(0)->checkState() == Qt::Checked)
        OcrTask.Add(FieldSource::Mrz , FieldId::All);
    if(ui->listWidgetOCR->item(1)->checkState() == Qt::Checked)
        OcrTask.Add(FieldSource::Viz , FieldId::All);
    if(ui->listWidgetOCR->item(2)->checkState() == Qt::Checked)
        OcrTask.Add(FieldSource::Barcode , FieldId::All);

    Page page;
    try { page = pr->GetScanner().GetPage(0); }
    catch(const Exceptions::General &) { return; }
    try { AnalyzeResult = pr->GetEngine().Analyze(page, OcrTask); }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
        return;
    }

    FillOcrDataGrid();
    FillDataPage();
}

void MainWindow::FillOcrDataGrid()
{
    std::vector<FieldReference> Fields = AnalyzeResult.GetFields();
    for(int ix = 0; ix < (int)Fields.size(); ix++)
    {
        try
        {
            Field field = AnalyzeResult.GetField(Fields[ix]);

            modelGrid->setItem(ix, 3, new QStandardItem(QString::number(ix)));
            QString FieldID = Fields[ix].ToString(" ") + StrCon() + GetAmid(field);
            modelGrid->setItem(ix, 0, new QStandardItem(FieldID));

            QString Value;
            try { Value = QChar(0x200e) + field.GetBestStringValue(); }
            catch(const Exceptions::InvalidParameter &) {
                Value = PrintBinary(field.GetBinaryValue(), 0, 16);
            }
            catch(const Exceptions::General &) { }
            modelGrid->setItem(ix, 1, new QStandardItem(Value));

            modelGrid->setItem(ix, 2, new QStandardItem((ToQString)field.GetStatus().ToString()));
        }
        catch(const Exceptions::General &) { }
    }
}

void MainWindow::UpdateFieldDisplay()
{
    QItemSelectionModel *select =  ui->tableViewGrid->selectionModel();
    if(!select->hasSelection()) return;

    ClearOCRData();

    int ix = select->selection().indexes()[0].row();
    ix = modelGrid->data(modelGrid->index(ix, 3)).toInt();

    FieldReference SelectedField = AnalyzeResult.GetFields()[ix];
    Field field = AnalyzeResult.GetField(SelectedField);
    try { ui->labelRAWValue->setText(QChar(0x200e) + field.GetRawStringValue()); }
    catch(const Exceptions::General &) { }
    try { ui->labelFormattedValue->setText(field.GetFormattedStringValue()); }
    catch(const Exceptions::General &) { }
    try { ui->labelStandardizedValue->setText(field.GetStandardizedStringValue()); }
    catch(const Exceptions::General &) { }
    try {
        QPixmap pm = field.GetImage().ToQImage<QPixmap>();
        DrawImage(ui->labelFieldImagePicture, pm);
    }
    catch(const Exceptions::General &) { }
}

void MainWindow::FillDataPage()
{
    QString name = GetFieldValue(FieldId::Surname);
    ui->labelTabNameValue->setText(name);
    if(!name.isEmpty())
    {
        ui->labelTabNameValue->setText(name + " " + GetFieldValue(FieldId::Surname2));
        ui->labelTabNameValue2->setText(GetFieldValue(FieldId::Givenname) + StrCon()
                                        + GetFieldValue(FieldId::MiddleName));
    }
    else ui->labelTabNameValue->setText(GetFieldValue(FieldId::Name));

    ui->labelTabBirthValue->setText(StrCon("on") + GetFieldValue(FieldId::BirthDate)
                                    + StrCon("in") + GetFieldValue(FieldId::BirthPlace));

    ui->labelTabNationalityValue->setText(GetFieldValue(FieldId::Nationality));

    ui->labelTabSexValue->setText(GetFieldValue(FieldId::Sex));

    ui->labelTabIssuerValue->setText(GetFieldValue(FieldId::IssueCountry) + StrCon()
                                     + GetFieldValue(FieldId::IssueState));

    ui->labelTabTypeValue->setText(GetFieldValue(FieldId::DocType) + StrCon()
                                   + GetFieldValue(FieldId::DocTypeDisc));
    if(ui->labelTabTypeValue->text().isEmpty())
        ui->labelTabTypeValue->setText(GetFieldValue(FieldId::Type));

    ui->labelTabPageValue->setText(GetFieldValue(FieldId::DocPage));

    ui->labelTabNumberValue->setText(GetFieldValue(FieldId::DocumentNumber));

    ui->labelTabValidValue->setText(StrCon("from") + GetFieldValue(FieldId::IssueDate)
                                    + StrCon("to") + GetFieldValue(FieldId::ExpiryDate));

    try {
        QPixmap pm = AnalyzeResult.GetField(FieldSource::Viz,
                        FieldId::Face).GetImage().ToQImage<QPixmap>();
        DrawImage(ui->labelTabFacePhotoPic, pm);
    }
    catch(const Exceptions::General &) { }

    try {
        QPixmap pm = AnalyzeResult.GetField(FieldSource::Viz,
                        FieldId::Signature).GetImage().ToQImage<QPixmap>();
        DrawImage(ui->labelTabSignaturePic, pm);
    }
    catch(const Exceptions::General &) { }
}

ENDREGION }

REGION("General tools") {
//------------------------------------------------------------------------------

QString MainWindow::GetAmid(const Field &field)
{
    try {
        return field.ToVariant().GetChild(Util::VariantId::AMID, 0).ToString();
    }
    catch(const Exceptions::General &) { return ""; }
}

QString MainWindow::GetFieldValue(const Processing::FieldId &Id)
{
     FieldReference filter(FieldSource::All, Id);
     std::vector<FieldReference> Fields = AnalyzeResult.GetFields(filter);
     for(unsigned int ix = 0; ix < Fields.size(); ix++)
     {
         QString value = AnalyzeResult.GetField(Fields[ix]).GetBestStringValue();
         if(value != "") return value;
     }
     return "";
}

QString MainWindow::PrintBinary(const std::vector<gxu8> &arr, int pos, int sz)
{
    int p0;
    QString str = "", str2 = "";
    for(p0 = pos; p0 < (int)arr.size() && p0 < pos + sz; p0++)
    {
        str += QString().sprintf("%02X ", arr[p0]);
        str2 += arr[p0] < 0x21 || arr[p0] > 0x7e ? '.' : (QChar)arr[p0];
    }
    for(; p0 < pos + sz; p0++) { str += "   "; str2 += " "; }
    return str + str2;
}

void MainWindow::AddTabPage(const QString &lightName)
{
    QLabel* lb = new QLabel();
    lb->setObjectName(lightName);
    lb->setFixedWidth(ui->tabWidgetRight->widget(0)->width());
    lb->setFixedHeight(ui->tabWidgetRight->widget(0)->height());
    lb->setStyleSheet("QLabel {background-color: black;}");
    ui->tabWidgetRight->addTab(lb, lightName);
}

void MainWindow::DrawImage(QLabel* lbl, QPixmap pm)
{
    lbl->setPixmap(pm.scaled(lbl->width(), lbl->height(), Qt::KeepAspectRatio));
}

void MainWindow::ClearOCRData()
{
    ui->labelFieldImagePicture->clear();
    ui->labelRAWValue->setText("");
    ui->labelFormattedValue->setText("");
    ui->labelStandardizedValue->setText("");
}

void MainWindow::ClearDataPage()
{
    ui->labelTabNameValue->setText("");
    ui->labelTabNameValue2->setText("");
    ui->labelTabBirthValue->setText("");
    ui->labelTabNationalityValue->setText("");
    ui->labelTabSexValue->setText("");
    ui->labelTabIssuerValue->setText("");
    ui->labelTabTypeValue->setText("");
    ui->labelTabPageValue->setText("");
    ui->labelTabNumberValue->setText("");
    ui->labelTabValidValue->setText("");
    ui->labelTabSignaturePic->clear();
    ui->labelTabFacePhotoPic->clear();
}

void MainWindow::RemoveTabs()
{
    for(int ix = ui->tabWidgetRight->count()-1; ix >= 2; ix--)
        ui->tabWidgetRight->removeTab(ix);
}

void MainWindow::RemoveGridRows()
{
     ui->tableViewGrid->model()->removeRows(0, ui->tableViewGrid->model()->rowCount());
}

void MainWindow::on_tableViewGrid_clicked(const QModelIndex&)
{
    UpdateFieldDisplay();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if(obj == ui->tableViewGrid && (ev->type() == QEvent::KeyRelease))
        UpdateFieldDisplay();

    return QObject::eventFilter(obj, ev);
}

ENDREGION }
