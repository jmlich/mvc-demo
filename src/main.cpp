#include <QDebug>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QTranslator>

#include "filereader.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    QTranslator translator;
    QQmlApplicationEngine engine;

    QString i18nFilename = QLatin1String("mvc-demo_") + QLocale::system().name();

    if (translator.load(i18nFilename, QCoreApplication::applicationDirPath())) {
        qDebug() << "loading translations from " << i18nFilename;
        app.installTranslator(&translator);
    } else {
        qWarning() << "failed to load translations from " << i18nFilename;
    }

    qmlRegisterType<FileReader>("cz.mlich", 1, 0, "FileReader");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
