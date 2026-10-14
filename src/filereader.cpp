#include "filereader.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QUrl>

#include <QString>

FileReader::FileReader(QObject* parent)
    : QObject(parent)
{
}

/**
 * @brief FileReader::toLocal wrapper over QUrl.toLocalFile() function
 * @param filename
 * @return
 */

QString FileReader::toLocal(const QUrl& filename)
{
    return filename.toLocalFile();
}

/**
 * @brief FileReader::toPath converts a file: or qrc: URL to a path QFile can open
 * @param filename
 * @return empty string for other schemes
 */
QString FileReader::toPath(const QUrl& filename)
{
    if (filename.isLocalFile()) {
        return filename.toLocalFile();
    }
    if (filename.scheme() == "qrc") {
        return ":" + filename.path();
    }
    return QString();
}

QString FileReader::read(const QUrl& filename)
{
    return read_local(toPath(filename));
}

QString FileReader::read_local(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open " << filename << "for reading";
        return QString();
    }

    return QString::fromUtf8(file.readAll());
}

bool FileReader::write(const QUrl& filename, const QString& data)
{
    return write_local(toPath(filename), data);
}

bool FileReader::copy_file(const QUrl& filename, const QUrl& newFilename)
{
    if (!file_exists(filename)) {
        qWarning() << "Cannot copy " << filename << ": file does not exist";
        return false;
    }

    remove_if_exists(newFilename); // remove file with dst name if exist

    return QFile::copy(toPath(filename), toPath(newFilename));
}

void FileReader::remove_if_exists(const QUrl& filename)
{

    if (file_exists(filename)) {
        QFile::remove(toPath(filename));
    }
}

bool FileReader::write_local(const QString& filename, const QString& data)
{
    QSaveFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open " << filename << "for writing";
        return false;
    }

    file.write(data.toUtf8());
    if (!file.commit()) {
        qWarning() << "Cannot write " << filename;
        return false;
    }
    return true;
}

bool FileReader::file_exists(const QUrl& filename)
{
    return file_exists_local(toPath(filename));
}

bool FileReader::file_exists_local(const QString& filename)
{
    return QFileInfo(filename).isFile();
}

bool FileReader::is_local_file(const QUrl& filename)
{
    return filename.isLocalFile();
}

bool FileReader::delete_file(const QUrl& filename)
{
    return delete_file_local(toPath(filename));
}

bool FileReader::delete_file_local(const QString& filename)
{
    return QFile(filename).remove();
}

bool FileReader::is_dir_and_exists_local(const QString& dirname)
{
    return QFileInfo(dirname).isDir();
}

QString FileReader::dirname_local(const QString& filename)
{
    return QFileInfo(filename).absolutePath();
}
