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
    ui->bgImage->setFixedWidth(ui->MainLogo->width());
    this->setFixedSize(ui->bgImage->width(), ui->bgImage->height());

    // Set hidden objects
    ui->WarningMsg->setVisible(false);
    ui->LoadingBox->setVisible(false);
    ui->SettingsLabel->setVisible(false);

    // Set Version Number
    ui->VersionLabel->setText("<html><head/><body><p align=""right""><span style="" font-size:14pt;"">" + Settings::version + "</span></p></body></html>");
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
    ui->LoadingBox->setVisible(true);
    std::thread dThread(OnShow_threaded, ui, &(this->settingsMap));
    dThread.join();
}

void inStr(QFile& out, QString& s)
{
    std::string tmpstring;
    size_t l;
    out.read((char*)&l,sizeof(l)); 
    tmpstring.resize(l);
    out.read((char*)tmpstring.data(), l);
    s = s.fromStdString(tmpstring);
}

void OnShow_threaded(Ui::MainWindow *ui, std::unordered_map<QString,QString> *settingsMap){

    // Configure log style
    QString bLText, aLText;
    bLText = "<html><head/><body><p align=""center""><span style="" font-weight:600; color:#ffffff;"">";
    aLText = "</span></p></body></html>";

    // Check language
    ui->LoadingAnimationLabel->setText(bLText + "Checking language..." + aLText);
    auto toLang = QLocale::system().name();
    ui->LoadingAnimationLabel->setText(bLText + "Set language: " + toLang + aLText);

    // Create folder
    ui->LoadingAnimationLabel->setText(bLText + "Creating app folder..." + aLText);
    auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(appDataPath).exists()) if (!QDir(appDataPath).mkpath(appDataPath)) QMessageBox::critical(0, "Critical", "Unable to create app folder!!!");

    // Get Settings
    ui->LoadingAnimationLabel->setText(bLText + "Getting settings..." + aLText);
    auto filePath = appDataPath + QDir::separator() + "settings.cfg";
    QFile settingsCfgFile(filePath);
    if (settingsCfgFile.open(QIODevice::ReadOnly)) {
        size_t l;
        settingsCfgFile.read((char*)&l,sizeof(l));
        for(size_t i = 0; i < l; i++) {
            QString key;
            inStr(settingsCfgFile, key);
            inStr(settingsCfgFile, (*settingsMap)[key]);
        }
    }

    // Load settings config
    if (settingsMap->find("UseOnlineTranslation") == settingsMap->end()) (*settingsMap)["UseOnlineTranslation"] = "True";
    ui->Settings_UseTranslation->setChecked((*settingsMap)["UseOnlineTranslation"] == "True");

    // Connect to server
    bool hasInternet = false;
    if (true){
        ui->LoadingAnimationLabel->setText(bLText + "Connecting server... " + aLText);
        QNetworkAccessManager nam;
        nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
        QNetworkRequest req(QUrl("https://1slimpage.ru/DWAKU2_updater.php?ping="));
        QNetworkReply *reply = nam.get(req);
        QEventLoop loop;
        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
        while (!reply->isFinished()) loop.exec();
        QString serverAnswer = reply->readAll();
        nam.deleteLater();
        hasInternet = serverAnswer == "pong";
        if (hasInternet) ui->LoadingAnimationLabel->setText(bLText + "You are connected to the internet" + aLText);
            else ui->LoadingAnimationLabel->setText(bLText + "You have a network error: " + reply->errorString() + "\n" + req.url().toString() + aLText);
        delete reply;
    }

    // If app doesn't have Internet access -> show offline mode
    if (!hasInternet) {
        ui->WarningMsg->setVisible(true);
        ui->SettingsLabel->setVisible(true);
    }

//    // Getting translation
//    if (hasInternet){
//        ui->LoadingAnimationLabel->setText(bLText + "Getting translation... " + aLText);

//    }

//    // Check updates
//    bool hasUpdatedVersion = false;
//    QString serverVersion = Settings::version;
//    if (hasInternet){
//        ui->LoadingAnimationLabel->setText(bLText + "Checking new version... " + aLText);
//        QNetworkAccessManager nam;
//        nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
//        QNetworkRequest req(QUrl("https://slimpage.ru/DWAKU2_updater.php?get_latest_version="));
//        QNetworkReply *reply = nam.get(req);
//        QEventLoop loop;
//        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
//        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
//        while (!reply->isFinished()) loop.exec();
//        serverVersion = reply->readAll();
//        nam.deleteLater();
//        delete reply;
//        hasUpdatedVersion = Settings::version != serverVersion;
//    }

//    // Getting updates
//    if (hasInternet && hasUpdatedVersion){
//        ui->LoadingAnimationLabel->setText(bLText + "Downloading updates... " + aLText);
//        QNetworkAccessManager nam;
//        nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
//        QNetworkRequest req(QUrl("https://slimpage.ru/DWAKU2_updater.php?download_latest="));
//        QNetworkReply *reply = nam.get(req);
//        QEventLoop loop;
//        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
//        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
//        while (!reply->isFinished()) loop.exec();
//        QSaveFile file(appDataPath + QDir::separator() + "latest_DWAKU2.exe");
//        file.open(QIODevice::WriteOnly);
//        file.write(reply->readAll());
//        file.flush();
//        file.commit();
//        nam.deleteLater();
//        delete reply;
//    }

//    // Checking update
//    bool fileCorrect = false;
//    if (hasInternet && hasUpdatedVersion){
//        ui->LoadingAnimationLabel->setText(bLText + "Checking update files... " + aLText);
//        QNetworkAccessManager nam;
//        nam.setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
//        QNetworkRequest req(QUrl("https://slimpage.ru/DWAKU2_updater.php?check_latest="));
//        QNetworkReply *reply = nam.get(req);
//        QEventLoop loop;
//        QObject::connect(reply, SIGNAL(readyRead()), &loop, SLOT(quit()));
//        QObject::connect(&nam, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
//        while (!reply->isFinished()) loop.exec();
//        QString serverHash = reply->readAll();
//        QFile file(appDataPath + QDir::separator() + "latest_DWAKU2.exe");
//        file.open(QIODevice::ReadOnly);
//        QByteArray clientFile = file.readAll();
//        file.close();
//        QString clientHash = QCryptographicHash::hash(clientFile, QCryptographicHash::Sha256).toHex();
//        fileCorrect = clientHash == serverHash;
//        nam.deleteLater();
//        delete reply;
//    }

//    // Make updater script
//    if (fileCorrect){
//        ui->LoadingAnimationLabel->setText(bLText + "Making updater script... " + aLText);
//        auto programmName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
//        QFile file(appDataPath + QDir::separator() + "updater.cmd");
//        if (file.open(QIODevice::WriteOnly)) {

//            auto pid = "TASKKILL /F /PID " + QString::number(QCoreApplication::applicationPid()) + "\r\n";
//            auto rename = "RENAME \"" + appDataPath + QDir::separator() + "latest_DWAKU2.exe\" \"" + QFileInfo(QCoreApplication::applicationFilePath()).fileName() + "\"\r\n";
//            auto move = "MOVE /Y \"" + appDataPath + QDir::separator() + QFileInfo(QCoreApplication::applicationFilePath()).fileName() + "\" \"" + QCoreApplication::applicationDirPath() + "\"\r\n";
//            auto start = "\"" + QCoreApplication::applicationFilePath() + "\"\r\n";

//            file.write(pid.toUtf8());
//            file.write(rename.toUtf8());
//            file.write(move.toUtf8());
//            file.write(start.toUtf8());
//            file.write("pause\r\n");

//        } else fileCorrect = false;
//        file.close();

//    }

    // Installing updates
//    if (fileCorrect){
//        ui->LoadingAnimationLabel->setText(bLText + "Installing update files... " + aLText);
//        auto command = appDataPath + QDir::separator() + "updater.cmd";
//        system(command.toUtf8());
//        //QProcess updater;
//        //updater.startDetached(appDataPath + QDir::separator() + "updater.cmd");
//    }

    // Close loading animation
    //std::this_thread::sleep_for(std::chrono::seconds(9999));
    //ui->LoadingBox->setVisible(false);
    //ui->SettingsLabel->setVisible(true);
}

void MainWindow::SettingsLabelPressed(){
    ui->bgImage->setFixedSize(ui->bgImage->width() + ui->SettingsFrame->width() + (ui->SettingsFrame->x()-ui->bgImage->width())*2, ui->bgImage->height());
    this->setFixedSize(ui->bgImage->width(), ui->bgImage->height());
}

void MainWindow::on_CloseMsgButton_clicked(){
    ui->WarningMsg->setVisible(false);
}

void outStr(QFile& out, const QString& s)
{
    auto sourceString = s.toStdString();
    size_t l = sourceString.size();

    out.write((char*)&l, sizeof(l));
    out.write((char*)sourceString.c_str(), l);
}

void MainWindow::on_Settings_SaveButton_clicked()
{
    // Save settings
    auto appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    auto filePath = appDataPath + QDir::separator() + "settings.cfg";
    size_t l = settingsMap.size();
    QFile settingsCfgFile(filePath);
    if (settingsCfgFile.open(QIODevice::WriteOnly)) {
        settingsCfgFile.write((char*)&l,sizeof(l));
        for(auto& p: settingsMap)
        {
            outStr(settingsCfgFile, p.first);
            outStr(settingsCfgFile, p.second);
        }
    } else QMessageBox::critical(0, "Critical", "Can't create config file!!!");
}

void MainWindow::on_Settings_UseTranslation_stateChanged(int state)
{
    settingsMap["UseOnlineTranslation"] = (state == Qt::Checked)? "True" : "False";
}
