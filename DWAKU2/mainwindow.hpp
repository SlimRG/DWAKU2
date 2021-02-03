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

#include <thread>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "settings.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

void OnShow_threaded(Ui::MainWindow *ui, std::unordered_map<QString,QString> *settingsMap);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void OnShow();

private slots:
    void SettingsLabelPressed();

    void on_CloseMsgButton_clicked();

    void on_Settings_SaveButton_clicked();

    void on_Settings_UseTranslation_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    std::unordered_map<QString,QString> settingsMap;
};

#endif // MAINWINDOW_H
