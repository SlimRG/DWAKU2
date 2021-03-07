#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QThread>

#include <iostream>

int main()
{

    std::cout << "Updating..." << std::endl;
    bool hasError = false;

    QCoreApplication::setApplicationName("DWAKU2");
    const QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) +
                                QDir::separator() + "up" +
                                QDir::separator() + "commands.slimrg";
    QCoreApplication::setApplicationName("DWAKU_up");

    QFile in(appDataPath);
    if (in.open(QIODevice::ReadOnly)) {

        QTextStream inTS(&in);
        QString line;

        std::vector<QString> args;
        if (in.isOpen())
            while (inTS.readLineInto(&line))
                args.push_back(line);
        in.close();

        QProcess process;
        process.setProgram("taskkill");
        process.setArguments(QStringList() << "/F" << "/PID" << args[0].toUtf8());
        process.start();
        process.waitForFinished(-1);
        process.close();

        QThread::sleep(3);

        if (QFile::remove(args[2]))
            if (QFile::copy(args[1], args[2]))
                QFile::remove(args[1]);
            else { std::cout << "Error (3)" << std::endl; hasError = true; }
        else { std::cout << "Error (2)" << std::endl; hasError = true; }

        QFileInfo fi(args[2]);
        process.setProgram("cmd.exe");
        const auto wd = fi.absoluteDir().absolutePath().toUtf8();
        const auto title = fi.fileName().toUtf8();
        process.setWorkingDirectory(wd);
        process.setArguments(QStringList() << "/C" <<  "start" << title << title);
        process.startDetached();
    } else { std::cout << "Error (1)" << std::endl; hasError = true; }

    if (hasError) while (true) getchar();

    return 0;
}
