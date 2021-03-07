#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMovie>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QSaveFile>
#include <QProcess>
#include <QThread>
#include <QSystemTrayIcon>
#include <QMutex>
#include <QFileDialog>
#include <QErrorMessage>
#include <QNetworkProxy>
#include <QTimer>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

#include <QtConcurrent>

#include <thread>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <Windows.h>
#include <shellapi.h>
#include <string.h>

#include "settings.hpp"
#include "events.hpp"
#include "appclass.hpp"
#include "timeout.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void OnShow();

    QMap<QString, appInfo> productCards;
    QMap<QString, QString> settings;
    QMap<QString, QMap<QString, QString>> Transations;
    QMutex	loadingMutex;
    size_t loadingCounts = 0;

    // Proxy
    QNetworkProxy proxy;

    // Translation
    const QString Translate(const QString& text);

    // Translation
    QString Version_tr = "Version";
    QString Status_tr  = "Status";
    QString Title_tr   = "Title";
    QString Size_tr    = "Size";
    QString MadeBy_tr  = "Made by";

private slots:
    void SettingsLabelPressed();

    void on_CloseMsgButton_clicked();

    void on_InternetErrorConnection();

    void on_ShowAppMenu();

    void on_AppComboBox_currentIndexChanged(const QString& appName);

    void on_SaveBtn_clicked();

    void on_DownloadBtn_clicked();

    void on_DownloadProgress(qint64 ist, qint64 max);

    void on_UpdateBtn_clicked();

    void on_StartBtn_clicked();

    void on_RemoveAppsBtn_clicked();

    void on_ShowLoading();

    void on_RemoveSettingsBtn_clicked();

    void on_SaveSettings();

    void on_LanguageListCombo_currentIndexChanged(const QString& lang);

    void on_BtnTranslate(QPushButton* btn);

    void on_LabelTranslate(QLabel* label);

    void on_ProxySettingsBtn_clicked();

    void on_ProxyTypeCombo_currentIndexChanged(const QString &arg1);

    void on_UseAuthCB_stateChanged(int state);

    void on_ProxyIP_textChanged(const QString &arg1);

    void on_ProxyPort_textChanged(const QString &arg1);

    void on_ProxyLogin_textChanged(const QString &arg1);

    void on_ProxyPassword_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QThread* startingThread = nullptr;
    void closeEvent(QCloseEvent *event);

signals:
    void sigInternetErrorConnection();
    void sigShowAppMenu();
    void sigShowLoading();
    void sigSaveSettings();
};

void OnShow_threaded(MainWindow* mw, Ui::MainWindow *ui);

#endif // MAINWINDOW_H
