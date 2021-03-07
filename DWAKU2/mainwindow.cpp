#include "mainwindow.hpp"
#include "mainwindow.hpp"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow){
    ui->setupUi(this);

    // Custom Frame
    this->setWindowFlags(Qt::FramelessWindowHint);

    // Set app name
    this->setWindowTitle("DWAKU2");

    // Connect SettingsLabel
    connect(ui->SettingsLabel, SIGNAL(Mouse_Pressed()), this, SLOT(SettingsLabelPressed()));

    // Set main frame
    ui->bgImage->setFixedWidth(ui->line->geometry().topLeft().x());
    ui->bgImage->setFixedHeight(ui->line_2->geometry().topLeft().y());
    this->setFixedSize(ui->bgImage->width(), ui->bgImage->height());

    // Set Version Number and Made by
    ui->VersionLabel->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt;\"> "+ Translate("Version") + ": " + Settings::version + "</span></p></body></html>");
    ui->MadeBy->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt;\"> "+ Translate("Made by") + "<br/>Alrott SlimRG</span></p></body></html>");

    // Set hidden objects
    ui->WarningMsg->setVisible(false);
    ui->LoadingBox->setVisible(false);
    ui->AppSelectionBox->setVisible(false);
    ui->SettingsLabel->setVisible(false);
    ui->ProgressBar->setVisible(false);

    // Set objects' posotions
    ui->LoadingBox->setGeometry(60, 100, 250, 250);
    ui->AppSelectionBox->setGeometry(40, 100, 290, 290);

    // Set LineEdit for IP and Port
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    ui->ProxyIP->setValidator(new QRegExpValidator(ipRegex, this));
    ui->ProxyPort->setValidator(new QIntValidator(0, 999999, this));

    // Set proxy
    proxy.setCapabilities(QNetworkProxy::TunnelingCapability);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::OnShow(){
    // Create loading animation
    QMovie *movie = new QMovie(":/dynamic/loading/main.gif");
    ui->LoadingLabel->setMovie(movie);
    movie->start();

    // Start OnShow actions
    this->startingThread = QThread::create([&](){
        OnShow_threaded(this, ui);
    });

    QObject::connect(this, &MainWindow::sigInternetErrorConnection, this, &MainWindow::on_InternetErrorConnection);
    QObject::connect(this, &MainWindow::sigShowAppMenu, this, &MainWindow::on_ShowAppMenu);
    QObject::connect(this, &MainWindow::sigShowLoading, this, &MainWindow::on_ShowLoading);
    QObject::connect(this, &MainWindow::sigSaveSettings, this, &MainWindow::on_SaveSettings);

    loadingMutex.lock();
    loadingCounts++;
    loadingMutex.unlock();

    startingThread->start();
}

void OnShow_threaded(MainWindow* mw, Ui::MainWindow *ui){

    // Clear old translations
    ui->LoadingAnimationLabel->setText(mw->Translate(mw->Translate("Cleaning translations...")));
    mw->Transations.clear();

    // Show loading
    emit mw->sigShowLoading();

    // Create folder
    ui->LoadingAnimationLabel->setText(mw->Translate("Creating app folder..."));
    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(appDataPath).exists()) if (!QDir(appDataPath).mkpath(appDataPath)) QMessageBox::critical(0, "Critical", mw->Translate("Unable to create app's folder!!!"));
    const auto appDataSubPath = appDataPath + QDir::separator() + "apps";
    if (!QDir(appDataSubPath).exists()) if (!QDir(appDataSubPath).mkpath(appDataSubPath)) QMessageBox::critical(0, "Critical", mw->Translate("Unable to create apps folder!!!"));
    const auto langPath = appDataPath + QDir::separator() + "lang";
    if (!QDir(langPath).exists()) if (!QDir(langPath).mkpath(langPath)) QMessageBox::critical(0, "Critical", mw->Translate("Unable to create language folder!!!"));
    const auto settingsPath = appDataPath + QDir::separator() + "settings";
    if (!QDir(settingsPath).exists()) if (!QDir(settingsPath).mkpath(settingsPath)) QMessageBox::critical(0, "Critical", mw->Translate("Unable to create settings folder!!!"));

    // Remove updater's folder
    ui->LoadingAnimationLabel->setText(mw->Translate("Removing updater's folder..."));
    const auto upPath = appDataPath + QDir::separator() + "up";
    if (QDir(upPath).exists()) QDir(upPath).removeRecursively();

    // Read settings
    ui->LoadingAnimationLabel->setText(mw->Translate("Reading settings..."));
    const auto settingsList = settingsPath + QDir::separator() + "settings.json";
    if (QFile(settingsList).exists()) {
        QFile file(settingsList);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString valjson = file.readAll();
        file.close();
        // Synchronize
        QJsonObject clientjson = (QJsonDocument::fromJson(valjson.toUtf8())).object();
        foreach (auto key, clientjson.keys()) {
            mw->settings[key] = clientjson[key].toString();
        }
    }

    // Read local translation
    ui->LoadingAnimationLabel->setText(mw->Translate("Reading local translations..."));
    QDir dir(langPath);
    QStringList jsons = dir.entryList(QStringList("*.json"));
    foreach (QString file, jsons){
        QFileInfo fileInfo(langPath + QDir::separator() + file);
        const auto langName = fileInfo.baseName();
        const auto lName = langName.simplified();
        const auto nLN = QLocale(lName).nativeLanguageName();
        mw->Transations[nLN]["$BaseName"] = lName;
    }

    // Set language for first time
    ui->LoadingAnimationLabel->setText(mw->Translate("Setting language for first time..."));
    QString firstTimeolang;
    if (!mw->settings.contains("DefaultLang")){
        const auto sysLang = QLocale::system().nativeLanguageName();
        if (mw->Transations.contains(sysLang)){
            firstTimeolang = sysLang;
        } else {
            firstTimeolang = "";
        }
    } else firstTimeolang = mw->settings["DefaultLang"];

    // Generate translation
    ui->LoadingAnimationLabel->setText(mw->Translate("Preparing language list..."));
    ui->LanguageListCombo->clear();
    foreach (auto item, mw->Transations.keys()) {
        ui->LanguageListCombo->addItem(item);
    }
    ui->LanguageListCombo->setCurrentText(firstTimeolang);

    // Set proxy settings
    ui->LoadingAnimationLabel->setText(mw->Translate("Setting proxy..."));
    if (mw->settings.contains("ProxyType")) {
        ui->ProxyTypeCombo->setCurrentText(mw->settings["ProxyType"]);
        if (mw->settings["ProxyType"] == "Socks5Proxy") mw->proxy.setType(QNetworkProxy::Socks5Proxy);
            else if (mw->settings["ProxyType"] == "HttpProxy") mw->proxy.setType(QNetworkProxy::HttpProxy);
                else mw->proxy.setType(QNetworkProxy::DefaultProxy);
    }
    if (mw->settings.contains("ProxyPassword")) {
        ui->ProxyPassword->setText(mw->settings["ProxyPassword"]);
        mw->proxy.setPassword(mw->settings["ProxyPassword"]);
    }
    if (mw->settings.contains("ProxyLogin")) {
        ui->ProxyLogin->setText(mw->settings["ProxyLogin"]);
        mw->proxy.setUser(mw->settings["ProxyLogin"]);
    }
    if (mw->settings.contains("ProxyPort")) {
        ui->ProxyPort->setText(mw->settings["ProxyPort"]);
        mw->proxy.setPort(mw->settings["ProxyPort"].toUInt());
    }
    if (mw->settings.contains("ProxyIP")) {
        ui->ProxyIP->setText(mw->settings["ProxyIP"]);
        mw->proxy.setHostName(mw->settings["ProxyIP"]);
    }
    if ((mw->settings.contains("UseProxyAuth")) && (mw->settings["UseProxyAuth"] == "Checked")) {
        ui->UseAuthCB->setChecked(true);
    }

    // Set network connection
    ui->LoadingAnimationLabel->setText(mw->Translate("Setting internet libs..."));
    QNetworkAccessManager nam;
    if (!mw->proxy.hostName().isEmpty())
        nam.setProxy(mw->proxy);
    nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkRequest req;
    QNetworkReply *reply = nullptr;
    QEventLoop loop;
    QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    // Connect to server
    bool hasInternet = false;
    if (true){
        ui->LoadingAnimationLabel->setText(mw->Translate("Connecting server..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?ping="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        ReplyTimeout::set(reply, 5000);
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        QString serverAnswer = reply->readAll();
        hasInternet = serverAnswer == "pong";
        if (hasInternet) ui->LoadingAnimationLabel->setText(mw->Translate("You are connected to the internet"));
            else ui->LoadingAnimationLabel->setText(mw->Translate("You have a network error: ") + reply->errorString() + "\n" + req.url().toString());
        delete reply;
    }

    // If app doesn't have Internet access -> show offline mode
    if (!hasInternet){
        emit mw->sigInternetErrorConnection();
        while (!ui->WarningMsg->isVisible()) QThread::sleep(1);
        while (ui->WarningMsg->isVisible()) QThread::sleep(5);
    }

    // Getting translation
    if (hasInternet){
        ui->LoadingAnimationLabel->setText(mw->Translate("Getting translation..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?get_langList="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        ReplyTimeout::set(reply, 5000);
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        while (reply->canReadLine()){
            const auto lName = reply->readLine().simplified();
            const auto nLN = QLocale(lName).nativeLanguageName();
            mw->Transations[nLN]["$BaseName"] = lName;
        }
        delete reply;
    }

    // Check updates
    bool hasUpdatedVersion = false;
    QString serverVersion = Settings::version;
    if (hasInternet){
        ui->LoadingAnimationLabel->setText(mw->Translate("Checking new version..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?get_latest_version="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        ReplyTimeout::set(reply, 5000);
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        serverVersion = reply->readAll();
        delete reply;
        hasUpdatedVersion = Settings::version != serverVersion;
    }

    // Getting updates
    if (hasInternet && hasUpdatedVersion){
        ui->LoadingAnimationLabel->setText(mw->Translate("Downloading updates..."));
        if (!QDir(upPath).exists()) if (!QDir(upPath).mkpath(upPath)) QMessageBox::critical(0, "Critical", mw->Translate("Unable to create updater folder!!!"));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?download_latest="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        QFile file(upPath + QDir::separator() + QFileInfo(QCoreApplication::applicationFilePath()).fileName());
        file.open(QIODevice::WriteOnly);
        while (!reply->isFinished() || reply->isRunning()) {
            file.write(reply->readAll());
            file.flush();
            loop.exec();
        }
        file.write(reply->readAll());
        file.close();
        delete reply;
    }

    // Checking update
    bool fileCorrect = false;
    if (hasInternet && hasUpdatedVersion){
        ui->LoadingAnimationLabel->setText(mw->Translate("Checking update files..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?check_latest="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        QString serverHash = reply->readAll();
        delete reply;
        QFile file(upPath + QDir::separator() + QFileInfo(QCoreApplication::applicationFilePath()).fileName());
        file.open(QIODevice::ReadOnly);
        QByteArray clientFile = file.readAll();
        file.close();
        QString clientHash = QCryptographicHash::hash(clientFile, QCryptographicHash::Sha256).toHex();
        fileCorrect = clientHash == serverHash;    
    }

    // Downloading updater
    if (fileCorrect){
        ui->LoadingAnimationLabel->setText(mw->Translate("Downloading updater..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?download_updater="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        QFile file(upPath + QDir::separator() + "DWAKU2_up.exe");
        file.open(QIODevice::WriteOnly);
        while (!reply->isFinished() || reply->isRunning()) {
            file.write(reply->readAll());
            file.flush();
            loop.exec();
        }
        file.write(reply->readAll());
        file.close();
        delete reply;
    }

    // Checking updater
    if (fileCorrect){
        ui->LoadingAnimationLabel->setText(mw->Translate("Checking updater..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?check_updater="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        QString serverHash = reply->readAll();
        QFile file(upPath + QDir::separator() + "DWAKU2_up.exe");
        file.open(QIODevice::ReadOnly);
        QByteArray clientFile = file.readAll();
        file.close();
        QString clientHash = QCryptographicHash::hash(clientFile, QCryptographicHash::Sha256).toHex();
        fileCorrect = clientHash == serverHash;
        delete reply;
    }

    // Installing updates
    if (fileCorrect){
        ui->LoadingAnimationLabel->setText(mw->Translate("Installing updates..."));

        QString args;
        args += QString::number(QCoreApplication::applicationPid()) + "\n";
        args += upPath + QDir::separator() + QFileInfo(QCoreApplication::applicationFilePath()).fileName() + "\n";
        args += QCoreApplication::applicationFilePath();

        QFile commands(upPath + QDir::separator() + "commands.slimrg");
        commands.open(QIODevice::WriteOnly);
        commands.write(args.toUtf8());
        commands.close();

        QProcess process;
        process.setProgram("cmd.exe");
        process.setWorkingDirectory(upPath);
        process.setArguments(QStringList() << "/C" <<  "start" << "DWAKU2_up.exe" << "DWAKU2_up.exe");
        process.startDetached();
        QThread::sleep(-1);
    }

    // Get online programm list
    QString jsonList = "";
    if (hasInternet){
        ui->LoadingAnimationLabel->setText(mw->Translate("Downloading app list..."));
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?get_list="));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        ReplyTimeout::set(reply, 5000);
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        jsonList = reply->readAll();
        delete reply;
    }

    // Clear old product cards
    mw->productCards.clear();

    // Read server's json
    ui->LoadingAnimationLabel->setText(mw->Translate("Reading server's app list..."));
    if (jsonList.size()) {
        QJsonArray serverjson = (QJsonDocument::fromJson(jsonList.toUtf8())).array();
        foreach (const QJsonValue& servval, serverjson){
            appInfo ai;
            ai.Title = servval["Title"].toString();
            ai.ServerVersion = servval["Version"].toString();
            ai.ServerSize = servval["Size"].toString();
            ai.ServerHash = servval["ServerHash"].toString();
            ai.Link = servval["Link"].toString();
            ai.AlterLink = servval["AlterLink"].toString();

            mw->productCards[ai.Title] = ai;
        }
    }

    // Read client's json
    ui->LoadingAnimationLabel->setText(mw->Translate("Reading client's app list..."));
    const auto appList = appDataSubPath + QDir::separator() + "list.json";
    if (QFile(appList).exists()) {
        QFile file;
        file.setFileName(appList);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString valjson = file.readAll();
        file.close();
        // Synchronize
        QJsonArray clientjson = (QJsonDocument::fromJson(valjson.toUtf8())).array();
        foreach (const QJsonValue& clval, clientjson){
            if (!clval["Size"].toString().isEmpty()) {
                const auto appName = clval["Title"].toString();

                // Checking files
                file.setFileName(appDataSubPath + QDir::separator() + clval["Title"].toString() + ".exe");
                file.open(QIODevice::ReadOnly);
                QString hash = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha256).toHex();
                file.close();

                if (clval["Hash"].toString() == hash){
                    mw->productCards[appName].Title = appName;
                    mw->productCards[appName].LocalSize = clval["Size"].toString();
                    mw->productCards[appName].LocalVersion = clval["Version"].toString();
                    mw->productCards[appName].LocalHash = clval["Hash"].toString();
                }
            }
        }
    }

    // Generate app list
    ui->LoadingAnimationLabel->setText(mw->Translate("Prepare app list..."));
    ui->AppComboBox->clear();
    foreach (auto app,  mw->productCards) {
        ui->AppComboBox->addItem(app.Title);
    }
    if (!mw->settings.contains("DefaultApp")) mw->settings["DefaultApp"] = mw->productCards.first().Title;
        else if ((!mw->productCards.contains(mw->settings["DefaultApp"])) && (!mw->productCards.isEmpty())) mw->settings["DefaultApp"] = mw->productCards.first().Title;
    ui->AppComboBox->setCurrentText(mw->settings["DefaultApp"]);

    // Set default language to settings
    if ((!mw->settings.contains("DefaultLang")) || (mw->settings["DefaultLang"].isEmpty())){
        const auto sysLang = QLocale::system().nativeLanguageName();
        if (mw->Transations.contains(sysLang)){
            mw->settings["DefaultLang"] = sysLang;
        } else {
            mw->settings["DefaultLang"] = "";
        }
    }

    // Generate translation
    ui->LoadingAnimationLabel->setText(mw->Translate("Prepare language list..."));
    ui->LanguageListCombo->clear();
    foreach (auto item, mw->Transations.keys()) {
        ui->LanguageListCombo->addItem(item);
    }
    ui->LanguageListCombo->setCurrentText(mw->settings["DefaultLang"]);

    // Save settings
    ui->LoadingAnimationLabel->setText(mw->Translate("Saving settings..."));
    emit mw->sigSaveSettings();

    // Cleaning
    nam.deleteLater();

    // Show app menu
    mw->loadingMutex.lock();
    mw->loadingCounts--;
    if (!mw->loadingCounts)  emit mw->sigShowAppMenu();
    mw->loadingMutex.unlock();

}

void MainWindow::SettingsLabelPressed() {
    if (ui->bgImage->width() == ui->line->geometry().topLeft().x())
        ui->bgImage->setFixedSize(ui->line_3->geometry().topLeft().x(), ui->bgImage->height());
    else
        ui->bgImage->setFixedSize(ui->line->geometry().topLeft().x(), ui->bgImage->height());
    this->setFixedSize(ui->bgImage->width(), ui->bgImage->height());
}

void MainWindow::on_CloseMsgButton_clicked(){
    ui->WarningMsg->setVisible(false);
}

void MainWindow::on_InternetErrorConnection()
{
    ui->WarningMsg->setVisible(true);
    ui->SettingsLabel->setVisible(true);
}

void MainWindow::on_ShowAppMenu()
{
    ui->LoadingBox->setVisible(false);
    ui->AppSelectionBox->setVisible(true);
    ui->SettingsLabel->setVisible(true);
    on_AppComboBox_currentIndexChanged(ui->AppComboBox->currentText());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (event != nullptr){
        this->startingThread->terminate();
        QApplication::quit();
    }
}

const QString MainWindow::Translate(const QString &text)
{
    const auto lang = settings["DefaultLang"];
    if (lang.isEmpty()) return text;

    if (Transations[lang].contains(text)) return Transations[lang][text];
        else return text;
}

void MainWindow::on_AppComboBox_currentIndexChanged(const QString& appName)
{

    const auto tt = productCards[appName].Title;
    const auto sv = productCards[appName].ServerVersion;
    const auto lv = productCards[appName].LocalVersion;
    const auto ss = productCards[appName].ServerSize;
    const auto ls = productCards[appName].LocalSize;

    // Title
    ui->TitleLabel->setText(Title_tr + ": " + tt);
    ui->TitleLabel->setToolTip(tt);

    // Version
    if ((sv == lv) || (lv.isEmpty()) || (sv.isEmpty())) {
        ui->VerLabel->setText(Version_tr + ": " + ((lv.isEmpty())? sv : lv));
        ui->VerLabel->setToolTip(((lv.isEmpty())? sv : lv));
    } else {
        ui->VerLabel->setText(Version_tr + ": " + lv +  " (" + sv + "⍐)");
        ui->VerLabel->setToolTip(lv + " (" + sv + "⍐)");
    }

    // Size
    if ((ss == ls) || (ls.isEmpty()) || (ss.isEmpty())) {
        ui->SizeLabel->setText(Size_tr + ": " + ((ls.isEmpty())? ss : ls));
        ui->SizeLabel->setToolTip((ls.isEmpty())? ss : ls);
    } else {
        ui->SizeLabel->setText(Size_tr + ": " + ls +  " (" + ss + "⍐)");
        ui->SizeLabel->setToolTip(ls +  " (" + ss + "⍐)");
    }

    // Status
    if ((ls == ss) && (ls.isEmpty())) {
        productCards.remove(appName);
        ui->AppComboBox->removeItem(ui->AppComboBox->currentIndex());
    } else if ((ls != ss) && (ls.isEmpty())) {
        ui->StatLabel->setText(Status_tr + ":\t"+ Translate("Not Downloaded"));
        ui->DownloadBtn->setVisible(true);
    } else if ((ls != ss) && (ss.isEmpty())) {
        ui->StatLabel->setText(Status_tr + ":\t"+ Translate("Local app"));
        ui->DownloadBtn->setVisible(false);
        ui->StartBtn->setVisible(true);
        ui->UpdateBtn->setVisible(false);
        ui->SaveBtn->setVisible(true);
    } else if ((ls != ss) && (!ss.isEmpty()) && (!ls.isEmpty())) {
        ui->StatLabel->setText(Status_tr + ": " + Translate("Has Update"));
        ui->DownloadBtn->setVisible(false);
        ui->StartBtn->setVisible(true);
        ui->UpdateBtn->setVisible(true);
        ui->SaveBtn->setVisible(true);
    } else if ((ls == ss) && (!ls.isEmpty())) {
        if (lv == sv) {
            ui->StatLabel->setText(Status_tr + ": " + Translate("Already Updated"));
            ui->DownloadBtn->setVisible(false);
            ui->StartBtn->setVisible(true);
            ui->UpdateBtn->setVisible(false);
            ui->SaveBtn->setVisible(true);
        } else {
            ui->StatLabel->setText(Status_tr + ": " + Translate("Has Update"));
            ui->DownloadBtn->setVisible(false);
            ui->StartBtn->setVisible(true);
            ui->UpdateBtn->setVisible(true);
            ui->SaveBtn->setVisible(true);
        }
    }

    // If you have no apps
    if (appName.isEmpty()) {
        ui->StatLabel->setText(Status_tr + ": " + Translate("No apps"));
        ui->DownloadBtn->setVisible(false);
        ui->StartBtn->setVisible(false);
        ui->UpdateBtn->setVisible(false);
        ui->SaveBtn->setVisible(false);
    }

    // Add default app to settings
    settings["DefaultApp"] = appName;
    emit sigSaveSettings();
}

void MainWindow::on_SaveBtn_clicked()
{
    auto title = ui->AppComboBox->currentText();
    QString filename = QFileDialog::getSaveFileName(nullptr, "Save file", title, "*.exe");
    if (filename.isEmpty()) return;

    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto appDataSubPath = appDataPath + QDir::separator() + "apps";
    const auto from = appDataSubPath + QDir::separator() + title + ".exe";

    if(!QFile::copy(from, filename)) {
        auto msg = QErrorMessage(this);
        msg.showMessage("Error while coping file");
    }
}

void MainWindow::on_DownloadProgress(qint64 ist, qint64 max)
{
    ui->ProgressBar->setRange(0,max);
    ui->ProgressBar->setValue(ist);
}

void MainWindow::on_DownloadBtn_clicked()
{
    ui->ProgressBar->setVisible(true);
    ui->ProgressBar->setValue(0);

    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto appDataSubPath = appDataPath + QDir::separator() + "apps";

    // Get app title
    auto title = ui->AppComboBox->currentText();

    // Download app
    QNetworkAccessManager nam;
    if (!proxy.hostName().isEmpty()) nam.setProxy(proxy);
    nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkRequest req(QUrl(this->productCards[title].Link));
    QNetworkReply *reply = nam.get(req);
    QEventLoop loop;
    QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(on_DownloadProgress(qint64, qint64)));
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QByteArray clientFile;
    while (!reply->isFinished() || reply->isRunning()){
        clientFile += reply->readAll();
        loop.exec();
    }
    nam.deleteLater();
    delete reply;

    // Check file
    QString clientHash = QCryptographicHash::hash(clientFile, QCryptographicHash::Sha256).toHex();
    QString serverHash = this->productCards[title].ServerHash;

    // Download from alterlink
    if (clientHash != serverHash) {
        QNetworkRequest req(QUrl(this->productCards[title].AlterLink));
        QNetworkReply *reply = nam.get(req);
        QObject::connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(on_DownloadProgress(qint64, qint64)));
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
        clientFile.clear();
        while (!reply->isFinished() || reply->isRunning()){
            clientFile += reply->readAll();
            loop.exec();
        }
        nam.deleteLater();
        delete reply;

        // Check file
        clientHash = QCryptographicHash::hash(clientFile, QCryptographicHash::Sha256).toHex();
        if (clientHash != serverHash) {
           ui->ProgressBar->setVisible(false);
           return;
        }
    }

    // Save file
    QFile file(appDataSubPath + QDir::separator() + title + ".exe");
    file.open(QIODevice::WriteOnly);
    file.write(clientFile);
    file.close();

    // Save file info
    this->productCards[title].LocalSize = this->productCards[title].ServerSize;
    this->productCards[title].LocalVersion = this->productCards[title].ServerVersion;
    this->productCards[title].LocalHash = clientHash;

    // Save json
    QJsonArray json;
    foreach (auto jsonObj, this->productCards){
        QJsonObject val;
        val["Title"] = jsonObj.Title;
        val["Version"] = jsonObj.LocalVersion;
        val["Size"] = jsonObj.LocalSize;
        val["Hash"] = jsonObj.LocalHash;
        json.push_back(val);
    }

    file.setFileName(appDataSubPath + QDir::separator() + "list.json");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream << QJsonDocument(json).toJson(QJsonDocument::Indented);
    file.close();

    ui->ProgressBar->setVisible(false);
    ui->DownloadBtn->setVisible(false);
    on_AppComboBox_currentIndexChanged(ui->AppComboBox->currentText());
}

void MainWindow::on_UpdateBtn_clicked()
{
    on_DownloadBtn_clicked();
    on_AppComboBox_currentIndexChanged(ui->AppComboBox->currentText());
}

void MainWindow::on_StartBtn_clicked()
{
    const auto title = ui->AppComboBox->currentText();
    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto appDataSubPath = appDataPath + QDir::separator() + "apps";

    QProcess process;
    process.setProgram("cmd.exe");
    process.setWorkingDirectory(appDataSubPath);
    process.setArguments(QStringList() << "/C" <<  "start" << title + ".exe" << title + ".exe");
    process.startDetached();
    this->showMinimized();
}


void MainWindow::on_RemoveAppsBtn_clicked()
{
    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto appDataSubPath = appDataPath + QDir::separator() + "apps";

    QDir dir(appDataSubPath);
    dir.removeRecursively();

    // Start OnShow actions
    ui->LoadingBox->setVisible(true);
    this->startingThread = QThread::create([&](){
        OnShow_threaded(this, ui);
    });

    loadingMutex.lock();
    loadingCounts++;
    loadingMutex.unlock();

    startingThread->start();
}

void MainWindow::on_ShowLoading()
{
    ui->LoadingBox->setVisible(true);
    ui->AppSelectionBox->setVisible(false);
}

void MainWindow::on_RemoveSettingsBtn_clicked()
{
    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto appDataSubPath = appDataPath + QDir::separator() + "apps";

    QDir dir(appDataSubPath);
    dir.removeRecursively();

    // Start OnShow actions
    ui->LoadingBox->setVisible(true);
    this->startingThread = QThread::create([&](){
        OnShow_threaded(this, ui);
    });

    loadingMutex.lock();
    loadingCounts++;
    loadingMutex.unlock();

    startingThread->start();
}

void MainWindow::on_SaveSettings()
{

    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto settingsPath = appDataPath + QDir::separator() + "settings";

    QJsonObject jsonObj;
    QMapIterator<QString, QString> i(this->settings);
    while (i.hasNext()) {
        i.next();
        jsonObj.insert(i.key(), i.value());
    }

    QFile file(settingsPath + QDir::separator() + "settings.json");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << QJsonDocument(jsonObj).toJson(QJsonDocument::Indented);
    file.close();
}



void MainWindow::on_LanguageListCombo_currentIndexChanged(const QString &lang)
{

    const auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const auto langPath = appDataPath + QDir::separator() + "lang";
    const auto baseName = Transations[lang]["$BaseName"];

    // Add default app to settings
    settings["DefaultLang"] = lang;
    emit sigSaveSettings();

    // Set file path
    const auto filePath = langPath + QDir::separator() + baseName+".json";

    // Check local language
    QFile localJson(filePath);
    QString localHash;
    if (localJson.exists()){
        localJson.open(QFile::ReadOnly);
        localHash = QCryptographicHash::hash(localJson.readAll(), QCryptographicHash::Sha256).toHex();
        localJson.close();
    }

    // Check server file
    QNetworkAccessManager nam;
    if (!proxy.hostName().isEmpty()) nam.setProxy(proxy);
    nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkRequest req;
    QNetworkReply *reply = nullptr;
    QEventLoop loop;
    req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?get_langHash="+baseName));
    reply = nam.get(req);
    QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
    QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    ReplyTimeout::set(reply, 5000);
    while (!reply->isFinished() || reply->isRunning()) loop.exec();
    QString serverlHash = reply->readAll();
    delete reply;

    // Update translations
    QByteArray data;
    if ((localHash != serverlHash) && (!serverlHash.isEmpty())){
        req.setUrl(QUrl("https://slimpage.ru/DWAKU2_updater.php?get_lang="+baseName));
        reply = nam.get(req);
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        QFile file(filePath);
        file.open(QIODevice::WriteOnly);
        while (!reply->isFinished() || reply->isRunning()) loop.exec();
        data = reply->readAll();
        file.write(data);
        file.close();
    }

    // Check language
    if (data.isEmpty()){
        QFile localJson(filePath);
        if (localJson.exists()){
            localJson.open(QFile::ReadOnly);
            data = localJson.readAll();
            localJson.close();
        }
    }

    //Update language
    Transations[lang].clear();
    Transations[lang]["$BaseName"] = baseName;
    QJsonArray serverjson = (QJsonDocument::fromJson(data)).array();
    foreach (const QJsonValue& servval, serverjson){
        Transations[lang][servval["ParamID"].toString()] = servval["Translation"].toString();
    }

    // Rename btns
    QList<QPushButton *> btns = findChildren<QPushButton *>();
    for (auto btn : btns) on_BtnTranslate(btn);

    // Lazy rename labels
    QList<QLabel *> labels = findChildren<QLabel *>();
    for (auto label : labels) on_LabelTranslate(label);

    // Additional translation
    Version_tr = Translate("Version");
    Status_tr  = Translate("Status");
    Title_tr   = Translate("Title");
    Size_tr    = Translate("Size");
    on_AppComboBox_currentIndexChanged(ui->AppComboBox->currentText());
    ui->UseAuthCB->setText(Translate("Use authentication:"));
    // Set Version Number and Made by
    ui->VersionLabel->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt;\"> "+ Translate("Version") + ": " + Settings::version + "</span></p></body></html>");
    ui->MadeBy->setText("<html><head/><body><p align=\"center\"><span style=\" font-size:16pt;\"> "+ Translate("Made by") + "<br/>Alrott SlimRG</span></p></body></html>");
}

void MainWindow::on_BtnTranslate(QPushButton *btn)
{
    const auto translation = Translate(btn->objectName());
    if (translation != btn->objectName()) btn->setText(translation);
}

void MainWindow::on_LabelTranslate(QLabel *label)
{
    const auto translation = Translate(label->objectName());
    if (translation != label->objectName()) label->setText(translation);
}

void MainWindow::on_ProxySettingsBtn_clicked()
{
    if (ui->bgImage->width() == ui->line_3->geometry().topLeft().x())
        ui->bgImage->setFixedSize(ui->line_4->geometry().topLeft().x(), ui->bgImage->height());
    else
        ui->bgImage->setFixedSize(ui->line_3->geometry().topLeft().x(), ui->bgImage->height());
    this->setFixedSize(ui->bgImage->width(), ui->bgImage->height());
}

void MainWindow::on_ProxyTypeCombo_currentIndexChanged(const QString &proxyName)
{
    if (proxyName == "Socks5Proxy") proxy.setType(QNetworkProxy::Socks5Proxy);
        else if (proxyName == "HttpProxy") proxy.setType(QNetworkProxy::HttpProxy);
            else proxy.setType(QNetworkProxy::DefaultProxy);

    settings["ProxyType"] = proxyName;
    on_SaveSettings();
}

void MainWindow::on_UseAuthCB_stateChanged(int state)
{
    ui->ProxyLogin->setEnabled(state == Qt::Checked);
    ui->ProxyPassword->setEnabled(state == Qt::Checked);

    if  (state != Qt::Checked) {
        ui->ProxyLogin->clear();
        ui->ProxyPassword->clear();
    }

    settings["UseProxyAuth"] = (state == Qt::Checked)? "Checked" : "Unchecked";
    on_SaveSettings();
}

void MainWindow::on_ProxyIP_textChanged(const QString &proxyIP)
{
    settings["ProxyIP"] = proxyIP;
    proxy.setHostName(proxyIP);
    on_SaveSettings();
}

void MainWindow::on_ProxyPort_textChanged(const QString &proxyPort)
{
    settings["ProxyPort"] = proxyPort;
    proxy.setPort(proxyPort.toUInt());
    on_SaveSettings();
}

void MainWindow::on_ProxyLogin_textChanged(const QString &login)
{
    settings["ProxyLogin"] = login;
    proxy.setUser(login);
    on_SaveSettings();
}

void MainWindow::on_ProxyPassword_textChanged(const QString &password)
{
    settings["ProxyPassword"] = password;
    proxy.setPassword(password);
    on_SaveSettings();
}
