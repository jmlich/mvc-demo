#include <QTemporaryDir>
#include <QtTest>

#include "filereader.h"

class TestFileReader : public QObject {
    Q_OBJECT

private:
    QTemporaryDir tmp;
    FileReader reader;

    QString path(const QString& name)
    {
        return tmp.filePath(name);
    }

private slots:
    void initTestCase()
    {
        QVERIFY(tmp.isValid());
    }

    void writeAndReadLocal()
    {
        QVERIFY(reader.write_local(path("a.txt"), "hello"));
        QVERIFY(reader.file_exists_local(path("a.txt")));
        QCOMPARE(reader.read_local(path("a.txt")), QString("hello"));
    }

    void writeAndReadUtf8()
    {
        QVERIFY(reader.write_local(path("utf8.txt"), QString::fromUtf8("Česko")));
        QCOMPARE(reader.read_local(path("utf8.txt")), QString::fromUtf8("Česko"));
    }

    void writeLocalTruncates()
    {
        reader.write_local(path("b.txt"), "long content");
        reader.write_local(path("b.txt"), "short");
        QCOMPARE(reader.read_local(path("b.txt")), QString("short"));
    }

    void writeLocalToMissingDir()
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .* for writing"));
        QVERIFY(!reader.write_local(path("missing/c.txt"), "x"));
        QVERIFY(!reader.file_exists_local(path("missing/c.txt")));
    }

    void readLocalMissingFile()
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .* for reading"));
        QCOMPARE(reader.read_local(path("nothing.txt")), QString());
    }

    void writeAndReadUrl()
    {
        QUrl url = QUrl::fromLocalFile(path("d.txt"));
        QVERIFY(reader.write(url, "via url"));
        QVERIFY(reader.file_exists(url));
        QCOMPARE(reader.read(url), QString("via url"));
    }

    void readQrc()
    {
        QUrl url("qrc:/main.qml");
        QVERIFY(reader.file_exists(url));
        QVERIFY(reader.read(url).contains("ApplicationWindow"));
    }

    void readUnsupportedScheme()
    {
        QUrl url("https://example.com/salary.json");
        QVERIFY(!reader.file_exists(url));
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .* for reading"));
        QCOMPARE(reader.read(url), QString());
    }

    void writeQrcFails()
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot open .* for writing"));
        QVERIFY(!reader.write(QUrl("qrc:/main.qml"), "x"));
        QVERIFY(!reader.delete_file(QUrl("qrc:/main.qml")));
        QVERIFY(reader.file_exists(QUrl("qrc:/main.qml")));
    }

    void fileExistsLocalIsFalseForDir()
    {
        QVERIFY(!reader.file_exists_local(tmp.path()));
    }

    void isLocalFile()
    {
        QVERIFY(reader.is_local_file(QUrl::fromLocalFile(path("x"))));
        QVERIFY(!reader.is_local_file(QUrl("qrc:/main.qml")));
    }

    void toLocal()
    {
        QCOMPARE(reader.toLocal(QUrl::fromLocalFile(path("x"))), path("x"));
    }

    void copyFile()
    {
        QUrl src = QUrl::fromLocalFile(path("src.txt"));
        QUrl dst = QUrl::fromLocalFile(path("dst.txt"));
        reader.write(src, "new");
        reader.write(dst, "old");
        QVERIFY(reader.copy_file(src, dst));
        QCOMPARE(reader.read(dst), QString("new"));
    }

    void copyFileFromQrc()
    {
        QUrl dst = QUrl::fromLocalFile(path("copied.qml"));
        QVERIFY(reader.copy_file(QUrl("qrc:/main.qml"), dst));
        QVERIFY(reader.read(dst).contains("ApplicationWindow"));
    }

    void copyFileKeepsDestinationWhenSourceIsMissing()
    {
        QUrl dst = QUrl::fromLocalFile(path("keep.txt"));
        reader.write(dst, "keep");
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot copy .* file does not exist"));
        QVERIFY(!reader.copy_file(QUrl::fromLocalFile(path("no-source.txt")), dst));
        QCOMPARE(reader.read(dst), QString("keep"));
    }

    void deleteFile()
    {
        QUrl url = QUrl::fromLocalFile(path("e.txt"));
        reader.write(url, "x");
        QVERIFY(reader.delete_file(url));
        QVERIFY(!reader.file_exists(url));
        QVERIFY(!reader.delete_file(url));
    }

    void deleteFileLocal()
    {
        reader.write_local(path("f.txt"), "x");
        QVERIFY(reader.delete_file_local(path("f.txt")));
        QVERIFY(!reader.file_exists_local(path("f.txt")));
    }

    void removeIfExists()
    {
        QUrl url = QUrl::fromLocalFile(path("g.txt"));
        reader.write(url, "x");
        reader.remove_if_exists(url);
        QVERIFY(!reader.file_exists(url));
        reader.remove_if_exists(url);
    }

    void isDirAndExistsLocal()
    {
        QVERIFY(reader.is_dir_and_exists_local(tmp.path()));
        QVERIFY(!reader.is_dir_and_exists_local(path("missing")));
        reader.write_local(path("h.txt"), "x");
        QVERIFY(!reader.is_dir_and_exists_local(path("h.txt")));
    }

    void dirnameLocal()
    {
        QCOMPARE(reader.dirname_local(path("i.txt")), tmp.path());
    }
};

QTEST_GUILESS_MAIN(TestFileReader)
#include "tst_filereader.moc"
