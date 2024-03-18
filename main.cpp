#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QAction>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    /* ---------------------- Style ---------------------- */
    QFile styleSheetFile(":/styles/styles/Adaptic.qss");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleSheetFile.readAll());
    a.setStyleSheet(styleSheet);
    /* ---------------------- Style ---------------------- */

    MainWindow w;
    w.show();
    return a.exec();
}
