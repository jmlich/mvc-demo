#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include "filereader.h"

#include <QTextStream>
#include <QTextCodec>
#include <QString>

FileReader::FileReader(QObject *parent) :
    QObject(parent)
{
}

/**
 * @brief FileReader::toLocal wrapper over QUrl.toLocalFile() function
 * @param filename
 * @return
 */

QString FileReader::toLocal(const QUrl &filename) {
    return filename.toLocalFile();
}


QByteArray FileReader::read(const QUrl &filename) {
    if (filename.isLocalFile()) {
        return read_local(filename.toLocalFile());
    }
    if (filename.scheme() == "qrc") {
        QString resource_fn = ":" + filename.toString(QUrl::RemoveScheme);
        return read_local(resource_fn);
    }
    qWarning() << "Cannot open " << filename << "for reading";

    return QByteArray();
}


QByteArray FileReader::read_local(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open " << filename << "for reading";
        return QByteArray();
    }

    return file.readAll();
}

void FileReader::write(const QUrl &filename, QByteArray data) {
    write_local(filename.toLocalFile(), data);
}

void FileReader::copy_file(const QUrl &filename, const QUrl &newFilename) {

    remove_if_exists(newFilename); // remove file with dst name if exist

    QFile::copy(filename.toLocalFile(), newFilename.toLocalFile());
}

void FileReader::remove_if_exists(const QUrl &filename) {

    if (file_exists(filename)) {
        QFile::remove(filename.toLocalFile());
    }
}

void FileReader::writeUTF8(const QUrl &filename, QByteArray data) {

    QFile file (filename.toLocalFile());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }

    QTextStream streamFileOut(&file);
    streamFileOut.setCodec(QTextCodec::codecForName("UTF-8"));
    streamFileOut << QString::fromUtf8(data);
    streamFileOut.flush();

    file.close();
}

void FileReader::write_local(const QString &filename, QByteArray data) {
    QFile file (filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Cannot open " << filename << "for writing";
        return;
    }

    file.write(data);
}

bool FileReader::file_exists(const QUrl &filename) {
    if (filename.isLocalFile()) {
        return file_exists_local(filename.toLocalFile());
    }
    if (filename.scheme() == "qrc") {
        QString resource_fn = ":" + filename.toString(QUrl::RemoveScheme);
        return file_exists_local(resource_fn);
    }
    return false;
}

bool FileReader::file_exists_local(const QString &filename) {
    return QFileInfo(filename).isFile() && QFile(filename).exists();
}

bool FileReader::is_local_file(const QUrl &filename) {
    return filename.isLocalFile();
}

bool FileReader::delete_file(const QUrl &filename) {
    return delete_file_local(filename.toLocalFile());
}

bool FileReader::delete_file_local(const QString &filename) {
    return QFile(filename).remove();
}

bool FileReader::is_dir_and_exists_local(const QString &dirname) {
    QFileInfo info(dirname);
//    qDebug() << "is dir and exists " <<dirname << " exists: " <<info.exists() << " isDir: " << info.isDir();
    return info.exists() && info.isDir();
}

QString FileReader::dirname_local(const QString &filename) {
    QFileInfo info(filename);
    QUrl u = info.dir().path();
    return u.toLocalFile();
 }
