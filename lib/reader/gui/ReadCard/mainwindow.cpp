#include "mainwindow.h"
#include "ui_mainwindow.h"

QString Black("black");
QString Red("red");
QString Green("green");
QString Yellow("yellow");

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        pr(0),
        eventHandler(0),
        DeviceIsConnected(false),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    try {
        pr = new Pr22::DocumentReaderDevice();
    }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
        exit(1);
    }

    eventHandler = new EventHandler();

    qRegisterMetaType<ECardHandling::AuthProcess>("ECardHandling::AuthProcess");
    qRegisterMetaType<ECardHandling::File>("ECardHandling::File");
    qRegisterMetaType<Exceptions::ErrorCodes>("Exceptions::ErrorCodes");

    connect(eventHandler, SIGNAL(Connection(QStringList)), this, SLOT(GetDeviceModel(QStringList)));
    connect(eventHandler, SIGNAL(AuthBegin(const ECardHandling::AuthProcess &)),
                                               this, SLOT(AuthBegin(const ECardHandling::AuthProcess &)));

    connect(eventHandler, SIGNAL(AuthFinished(const ECardHandling::AuthProcess &, const Exceptions::ErrorCodes &)),
                                              this, SLOT(AuthFinished(const ECardHandling::AuthProcess &,
                                              const Exceptions::ErrorCodes &)));

    connect(eventHandler, SIGNAL(AuthWaitForInput(const ECardHandling::AuthProcess &)), this,
                                                       SLOT(AuthWaitForInput(const ECardHandling::AuthProcess &)));

    connect(eventHandler, SIGNAL(ReadBegin(ECardHandling::File)), this, SLOT(ReadBegin(ECardHandling::File)));

    connect(eventHandler, SIGNAL(ReadFinished(ECardHandling::File, const Exceptions::ErrorCodes &)), this,
                                        SLOT(ReadFinished(ECardHandling::File,const Exceptions::ErrorCodes &)));
    connect(eventHandler, SIGNAL(FileChecked(ECardHandling::File, const Exceptions::ErrorCodes &)), this,
                                        SLOT(FileChecked(ECardHandling::File, const Exceptions::ErrorCodes &)));

    eventHandler->RegisterEvents(pr);

    for(int ix = 0; ix < Util::EnumInfo<ECardHandling::FileId>::Count(); ix++)
    {
        QString str = (ToQString)Util::EnumInfo<ECardHandling::FileId>::GetName(ix);
        QListWidgetItem* item = new QListWidgetItem(str, ui->listWidgetFiles);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }

    for(int ix = 0; ix < Util::EnumInfo<ECardHandling::AuthLevel>::Count(); ix++)
    {
        QString str = (ToQString)Util::EnumInfo<ECardHandling::AuthLevel>::GetName(ix);
        ui->comboBoxAuthSel->addItem(str);
    }

    ui->comboBoxAuthSel->setCurrentIndex(1);
    card = Pr22::ECard();

    QSettings settings("ReadCard.config", QSettings::IniFormat);
    LoadCertificates(settings.value("CertDir", "").toString());

    ui->listWidgetFiles->setContextMenuPolicy(Qt::CustomContextMenu);
}

void EventHandler::RegisterEvents(Pr22::DocumentReaderDevice* PR)
{
    PR->AddEventHandler(Events::Event::Connection, this);
    PR->AddEventHandler(Events::Event::AuthBegin, this);
    PR->AddEventHandler(Events::Event::AuthFinished, this);
    PR->AddEventHandler(Events::Event::AuthWaitForInput, this);
    PR->AddEventHandler(Events::Event::ReadBegin, this);
    PR->AddEventHandler(Events::Event::ReadFinished, this);
    PR->AddEventHandler(Events::Event::FileChecked, this);
}

void MainWindow::closeEvent(QCloseEvent*)
{
    if(readCtrl.IsValid()) {
        readCtrl.Stop().Wait();
        QApplication::processEvents();
    }
    if(DeviceIsConnected) pr->Close();
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
        stringList.append(list[ix]);

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
        ui->pushButtonDisconnect->setEnabled(true);
        std::vector<Pr22::ECardReader> Readers = pr->GetReaders();
        for(unsigned int ix = 0; ix < Readers.size(); ix++)
        {
            QString hwt = (ToQString)Readers[ix].GetInfo().GetHwType().ToString();
            QListWidgetItem* item = new QListWidgetItem(hwt, ui->listWidgetReaders);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Unchecked);
        }
        ui->pushButtonRead->setEnabled(true);
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
        if(readCtrl.IsValid()) {
            readCtrl.Stop().Wait();
            QApplication::processEvents();
        }
        if(!card.GetSerial().empty()) {
            card.Disconnect();
            card = Pr22::ECard();
        }
        pr->Close();
        DeviceIsConnected = false;
    }
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonRead->setEnabled(false);
    ui->listWidgetReaders->clear();
    ui->textEditMsgs->setText("");
}

ENDREGION }

REGION("Reading") {
//------------------------------------------------------------------------------

void MainWindow::on_pushButtonRead_clicked()
{
    ui->textEditMsgs->setText("");
    ClearControls();

    if(readCtrl.IsValid()) readCtrl.Wait();
    if(!card.GetSerial().empty()) {
        try { card.Disconnect(); }
        catch(const Exceptions::General &) { }
        card = Pr22::ECard();
    }

    Pr22::ECardReader *cardReader = NULL;
    for(int cnt = 0; cnt < ui->listWidgetReaders->count(); cnt++) {

        QListWidgetItem *item = ui->listWidgetReaders->item(cnt);
        if(item->checkState() == Qt::Checked)
        {
            Pr22::ECardReader &reader = pr->GetReaders()[cnt];

            try {
                const std::vector<QString> Cards = reader.GetCards();
                if(!Cards.empty())
                {
                    card = reader.ConnectCard(0);
                    cardReader = &reader;
                    break;
                }
            }
            catch(const Exceptions::General &ex) {
                QMessageBox::critical(this, "Error", (ToQString)ex.what());
            }
        }
    }

    if(cardReader != NULL && !card.GetSerial().empty()) {
        StartRead(cardReader);
    }
}

void MainWindow::StartRead(Pr22::ECardReader *cardReader)
{
    ClearControls();
    ui->pushButtonRead->setEnabled(false);

    ui->textEditMsgs->append("Scanning");
    ui->textEditMsgs->repaint();
    Task::DocScannerTask scanTask;
    scanTask.Add(Imaging::Light::Infra).Add(Imaging::Light::White);
    Page page;

    try {
        page = pr->GetScanner().Scan(scanTask, Imaging::PagePosition::First);
    }
    catch(const Exceptions::General &) {}

    ui->textEditMsgs->append("Analyzing");
    ui->textEditMsgs->repaint();
    Task::EngineTask engineTask;
    engineTask.Add(FieldSource::Mrz, FieldId::All);
    engineTask.Add(FieldSource::Viz, FieldId::CAN);

    FieldReference faceFieldId(FieldSource::Viz, FieldId::Face);
    engineTask.Add(faceFieldId);
    FieldReference signatureFieldId(FieldSource::Viz, FieldId::Signature);
    engineTask.Add(signatureFieldId);

    try {
        vizResult = pr->GetEngine().Analyze(page, engineTask);
    }
    catch(const Exceptions::General &) { }
    faceDoc = Document();

    try { DrawImage(ui->labelFacesPic2, vizResult.GetField(faceFieldId).GetImage().ToQImage<QPixmap>()); }
    catch(const Exceptions::General &) { }
    try { DrawImage(ui->labelSignPic2, vizResult.GetField(signatureFieldId).GetImage().ToQImage<QPixmap>()); }
    catch(const Exceptions::General &) { }

    Task::ECardTask task;
    task.SetAuthLevel( string2enum<ECardHandling::AuthLevel>(ui->comboBoxAuthSel->currentText()) );

    for(int cnt = 0; cnt < ui->listWidgetFiles->count(); cnt++) {
        if(ui->listWidgetFiles->item(cnt)->checkState() == Qt::Checked)
            task.Add(string2enum<ECardHandling::FileId>(ui->listWidgetFiles->item(cnt)->text()));

    }

    try {
        readCtrl = cardReader->StartRead(card, task);
    } catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }
}

void EventHandler::OnAuthBegin(const ECardHandling::AuthProcess & authentication)
{
     emit AuthBegin(authentication);
}

void MainWindow::AuthBegin(const ECardHandling::AuthProcess& authentication)
{
    ui->textEditMsgs->append("Auth Begin: " + (ToQString)authentication.ToString());
    ColorAuthLabel(authentication, Yellow);
}

void EventHandler::OnAuthFinished(const ECardHandling::AuthProcess & authentication, const Exceptions::ErrorCodes & status)
{
    emit AuthFinished(authentication, status);
}

void MainWindow::AuthFinished(const ECardHandling::AuthProcess & authentication, const Exceptions::ErrorCodes & status)
{
    QString errstr = (ToQString)status.ToString();
    if(errstr.isEmpty()) errstr = QString().sprintf("%04X", (int)status);

    ui->textEditMsgs->append("Auth Done: " + (ToQString)authentication.ToString() + " status: " + errstr);
    bool ok = status == Exceptions::ErrorCodes::NoErr;
    ColorAuthLabel(authentication, ok ? Green : Red);
}

void EventHandler::OnAuthWaitForInput(const ECardHandling::AuthProcess & authentication)
{
    emit AuthWaitForInput(authentication);
}

void MainWindow::AuthWaitForInput(const ECardHandling::AuthProcess& authentication)
{
    try
    {
        ui->textEditMsgs->append("Auth Wait For Input: " + (ToQString)authentication.ToString());
        ColorAuthLabel(authentication, Yellow);

        BinData authData;
        int selector = 0;

        switch(authentication) {
            case ECardHandling::AuthProcess::BAC:
            case ECardHandling::AuthProcess::BAP:
            case ECardHandling::AuthProcess::PACE:
               {
                FieldReference fr(FieldSource::Mrz, FieldId::All);
                std::vector<FieldReference> authFields = vizResult.GetFields(fr);
                selector = 1;
                if(authFields.empty()) {
                    fr = FieldReference(FieldSource::Viz, FieldId::CAN);
                    authFields = vizResult.GetFields(fr);
                    selector = 2;
                }
                if(authFields.empty()) break;

                authData.SetString(vizResult.GetField(fr).GetBestStringValue());
               }
                break;

            default:
                break;
        }

        card.Authenticate(authentication, authData, selector);
    }
    catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }
}

void EventHandler::OnReadBegin(const ECardHandling::File & fileId)
{
    emit ReadBegin(fileId);
}

void MainWindow::ReadBegin(ECardHandling::File file)
{
    ui->textEditMsgs->append("Read Begin: " + file.ToString());
}

void EventHandler::OnReadFinished(const ECardHandling::File & fileId, const Exceptions::ErrorCodes & status)
{
    emit ReadFinished(fileId, status);
}

void MainWindow::ReadFinished(ECardHandling::File file, const Exceptions::ErrorCodes & status)
{
    QString errstr = (ToQString)status.ToString();
    if(errstr.isEmpty()) errstr = QString().sprintf("%04X", (int)status);

    ui->textEditMsgs->append("Read End: " + file.ToString() + " result: " + errstr);

    if(file.Id == ECardHandling::FileId::All) {
        ProcessAfterAllRead();
        ui->pushButtonRead->setEnabled(true);
    } else if (status != Exceptions::ErrorCodes::NoErr) {
        ColorFileName(file, Red);
    } else {
        ColorFileName(file, "blue");
        ProcessAfterFileRead(file);
    }
}

void MainWindow::ProcessAfterAllRead()
{
    try {
        QString mrz = vizResult.GetField(FieldSource::Mrz, FieldId::All).GetRawStringValue();
        QString dg1 = ui->textEditMrz->toPlainText();
        if(dg1.length() > 40) {
            ColorLabel(ui->labelMrz, (mrz == dg1 ? Green : Red));
        }
    } catch(const Exceptions::General &) { }
    try {
        Document facecmp = vizResult + faceDoc;
        std::vector<FieldCompare> fcl = facecmp.GetFieldCompareList();
        for(unsigned int ix = 0; ix < fcl.size(); ix++) {
            if(fcl[ix].Field1.GetId() == FieldId::Face && fcl[ix].Field2.GetId() == FieldId::Face) {
                QString col = Yellow;
                if(fcl[ix].Confidence < 300) col = Red;
                else if(fcl[ix].Confidence > 600) col = Green;
                ColorLabel(ui->labelFace, col);
            }
        }
    } catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }
}

void MainWindow::ProcessAfterFileRead(const ECardHandling::File& file)
{
    try
    {
        BinData rawFileContent = card.GetFile(file);
        Document fileDoc = pr->GetEngine().Analyze(rawFileContent);

        FieldReference faceFieldId(FieldSource::ECard, FieldId::Face);
        FieldReference mrzFieldId(FieldSource::ECard, FieldId::CompositeMrz);
        FieldReference signatureFieldId(FieldSource::ECard, FieldId::Signature);
        FieldReference fingerFieldId(FieldSource::ECard, FieldId::Fingerprint);

        if(!fileDoc.GetFields(faceFieldId).empty()) {
            faceDoc = fileDoc;
            DrawImage(ui->labelFacesPic1, fileDoc.GetField(faceFieldId).GetImage().ToQImage<QPixmap>());
        }
        if(!fileDoc.GetFields(mrzFieldId).empty()) {
            QString mrz = fileDoc.GetField(mrzFieldId).GetRawStringValue();
            if(mrz.length() == 90) mrz.insert(60, '\n').insert(30, '\n');
            else if(mrz.length() > 50) mrz.insert(mrz.length()/2, '\n');
            ui->textEditMrz->setAlignment(Qt::AlignCenter);
            ui->textEditMrz->append(mrz);
        }
        if(!fileDoc.GetFields(signatureFieldId).empty()) {
            try {
                DrawImage(ui->labelSignPic1, fileDoc.GetField(signatureFieldId).GetImage().ToQImage<QPixmap>());
            } catch(const Exceptions::General &) { }
        }
        if(!fileDoc.GetFields(fingerFieldId).empty()) {
            try {
                DrawImage(ui->labelFingerprintsPic1, fileDoc.GetField(FieldSource::ECard,
                                                FieldId::Fingerprint, 0).GetImage().ToQImage<QPixmap>());
                DrawImage(ui->labelFingerprintsPic2, fileDoc.GetField(FieldSource::ECard,
                                                FieldId::Fingerprint, 1).GetImage().ToQImage<QPixmap>());
            }
            catch(const Exceptions::General &) { }
        }
    } catch(const Exceptions::General &ex) {
        QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }
}

void EventHandler::OnFileChecked(const ECardHandling::File & fileId, const Exceptions::ErrorCodes & status)
{
    emit FileChecked(fileId, status);
}

void MainWindow::FileChecked(ECardHandling::File file, const Exceptions::ErrorCodes & status)
{
    ui->textEditMsgs->append("File Checked: " + file.ToString());
    bool ok = status == Exceptions::ErrorCodes::NoErr;
    ColorFileName(file, ok ? Green : Yellow);
}

ENDREGION }

REGION("General tools") {
//------------------------------------------------------------------------------

void MainWindow::ColorFileName(ECardHandling::File file, const QString &color)
{
    QList<QListWidgetItem*> list = ui->listWidgetFiles->findItems(file.ToString(), Qt::MatchFixedString);
    if(list.isEmpty())
    {
        try { file = card.ConvertFileId(file); }
        catch(const Exceptions::General &) { }
        list = ui->listWidgetFiles->findItems(file.ToString(), Qt::MatchFixedString);
    }
    if(!list.isEmpty()) list[0]->setTextColor(QColor(color));
}

void MainWindow::on_listWidgetFiles_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem* lwi = ui->listWidgetFiles->itemAt(pos);
    lwi->setSelected(true);

    if(lwi->textColor() != QColor(Black) && lwi->textColor() != QColor(Red) && lwi->textColor().isValid())
    {
        QMenu menu;
        menu.addAction("Save");
        QAction* action = menu.exec(QCursor::pos());
        if(action)
            SaveFile(lwi->text());
    }
}

void MainWindow::SaveFile(QString text)
{
    if(card.GetSerial().empty()) return;

    ECardHandling::FileId file = string2enum<ECardHandling::FileId>(text);

    try
    {
        QString FileName = QFileDialog::getSaveFileName(this, "Save", text, "XML files (*.xml);;Binary files (*.bin)");
        if(FileName.length() > 0)
        {
            BinData fileData = card.GetFile(file);
            if(FileName.endsWith(".bin", Qt::CaseInsensitive))
                    fileData.Save(FileName);
            else if(FileName.endsWith(".xml", Qt::CaseInsensitive))
                    pr->GetEngine().Analyze(fileData).Save(Document::FileFormat::Xml).Save(FileName);
        }
    } catch(const Exceptions::General &ex) {
            QMessageBox::critical(this, "Error", (ToQString)ex.what());
    }

}

QStringList MainWindow::FileList(const QString &dirname, const QStringList &masks)
{
    QStringList ResultList;
    try {
        QDir Dir(dirname);

        QFileInfoList DirList = Dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs);
        foreach(const QFileInfo &fi, DirList) ResultList << FileList(fi.absoluteFilePath(), masks);

        QFileInfoList List = Dir.entryInfoList(masks, QDir::Files);
        foreach(const QFileInfo &fi, List) ResultList << fi.absoluteFilePath();

    } catch(...) { }
    return ResultList;
}

void MainWindow::LoadCertificates(const QString &dir)
{
    QStringList exts;
    exts << "*.cer" << "*.crt" << "*.der" << "*.pem" << "*.crl" << "*.cvcert" << "*.ldif" << "*.ml";

    int cnt = 0;

    QStringList list = FileList(dir, exts);
    foreach(const QString &file, list)
    {
        try {
            BinData privateKey;
            if(file.endsWith(".cvcert", Qt::CaseInsensitive)) {
                //Searching for private key
                QString pk = file.mid(0, file.lastIndexOf('.') + 1).append("pkcs8");
                if(QFile(pk).exists()) privateKey.Load(pk);
            }
            pr->GetGlobalCertificateManager().Load(BinData().Load(file), privateKey);
            ++cnt;
        } catch(const Exceptions::General &ex) {
            ui->textEditMsgs->append("Error at loading " + file + " : " + (ToQString)ex.what());
        }
    }
    ui->textEditMsgs->append((cnt ? QString::number(cnt) : "No") + " certificate"
                             + (cnt==1 ? "" : "s") + " loaded from: '" + dir + "'");
}

ENDREGION }

REGION("Display") {
//------------------------------------------------------------------------------

void MainWindow::ColorAuthLabel(ECardHandling::AuthProcess auth, const QString &color)
{
    QLabel *label;
    using namespace ECardHandling;

    switch(auth) {
        case AuthProcess::BAC:
        case AuthProcess::BAP:      label = ui->labelBac; break;
        case AuthProcess::PACE:     label = ui->labelPace; break;
        case AuthProcess::Chip:     label = ui->labelCa; break;
        case AuthProcess::Terminal: label = ui->labelTa; break;
        case AuthProcess::Passive:  label = ui->labelPa; break;
        case AuthProcess::Active:   label = ui->labelTa; break;
        default: return;
    }

    ColorLabel(label, color);
}

void MainWindow::ColorLabel(QLabel *label, const QString &color)
{
    label->setStyleSheet("QLabel {color: " + color + ";}");
}

void MainWindow::DrawImage(QLabel* lbl, QPixmap pm)
{
    lbl->setPixmap(pm.scaled(lbl->width(), lbl->height(), Qt::KeepAspectRatio));
}

void MainWindow::ClearControls()
{
    ui->textEditMrz->setText("");
    ui->labelFacesPic1->clear();
    ui->labelFacesPic2->clear();
    ui->labelSignPic1->clear();
    ui->labelSignPic2->clear();
    ui->labelFingerprintsPic1->clear();
    ui->labelFingerprintsPic2->clear();

    ColorLabel(ui->labelBac, Black);
    ColorLabel(ui->labelPace, Black);
    ColorLabel(ui->labelCa, Black);
    ColorLabel(ui->labelTa, Black);
    ColorLabel(ui->labelPa, Black);
    ColorLabel(ui->labelAa, Black);
    ColorLabel(ui->labelMrz, Black);
    ColorLabel(ui->labelFace, Black);

    for(int ix = 0; ix < ui->listWidgetFiles->count(); ix++)
        ui->listWidgetFiles->item(ix)->setTextColor(Qt::black);

    ui->listWidgetFiles->repaint();
}

ENDREGION }
