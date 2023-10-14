#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QtCore>

#include "filereader.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QTranslator translator;

    QString i18nFilename = QLatin1String("mvc-demo_") + QLocale::system().name();

    if (translator.load(i18nFilename, "./")) {
        qDebug() << "loading translations from " << i18nFilename;
        app.installTranslator(&translator);
    } else {
        qWarning() << "failed to load translations from " << i18nFilename;
    }

    qmlRegisterType<FileReader>("cz.mlich", 1, 0, "FileReader");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
