#pragma once

#include <QLoggingCategory>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QCoreApplication>

// Get the default Qt message handler.
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);

static const QLoggingCategory first_category("first_category");
static const QLoggingCategory second_category("second_category");
static const QLoggingCategory third_category("third_category");
static const QLoggingCategory fourth_category("fourth_category");
static const QLoggingCategory fifth_category("fifth_category");
static const QLoggingCategory db_category("db_category");

class LogManager
{
public:
    static void init(const int quantityFiles,
                     const int maxFileSizeMB,
                     const QString& loggingType,
                     const QString& dirToSave,
                     const QStringList& loggingCategories
                     )
    {
        Q_UNUSED(loggingType) // TODO: write me
        _quantityFiles = quantityFiles;
        _maxFileSizeByte = maxFileSizeMB * 1024 * 1024;
        _dirToSave = dirToSave;
        QDir().mkdir(_dirToSave.isEmpty() ? "Log" : _dirToSave);
        QString categories = "default=true\n"
                + QString("%1=false\n").arg(first_category.categoryName())
                + QString("%1=false\n").arg(second_category.categoryName())
                + QString("%1=false\n").arg(third_category.categoryName())
                + QString("%1=false\n").arg(fourth_category.categoryName())
                + QString("%1=false\n").arg(fifth_category.categoryName())
                + QString("%1=false\n").arg(db_category.categoryName())
                ;
        for (const QString& c : loggingCategories) {
            categories.replace(c + "=false", c + "=true");
        }
        QLoggingCategory::setFilterRules(categories);
        qSetMessagePattern("[%{time} %{type}](%{file}|%{function}|%{line})(threadid:%{threadid}):\n%{message}");
        qInstallMessageHandler(LogManager::myCustomMessageHandler);
        qCDebug(fifth_category) << "Init app..." << Qt::endl;
    }

private:
    static void myCustomMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        saveToFile(qFormatLogMessage(type, context, msg));
#ifdef QT_DEBUG
        if (QString(context.category) == QString(first_category.categoryName())
//                || QString(context.category) == QString(second_category.categoryName())
                ) {
//            return;
        }
#endif
        // Call the default handler.
        (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
    }
    static void saveToFile(const QString &msg)
    {
        static int currentQuantityFiles = gitFileInfoList(_dirToSave).size();
        static QFile outFile(getStartLogFile());
        QTextStream ts(&outFile);
        if (outFile.isOpen()){
            ts << msg << Qt::endl;
        }
        else{
            if (outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered)) {
                ts << msg << Qt::endl;
            }
        }
        if (outFile.size() >= _maxFileSizeByte) {
            outFile.close();
            outFile.setFileName(getNewPathFile());
            currentQuantityFiles = gitFileInfoList(_dirToSave).size();
        }
        if (currentQuantityFiles > _quantityFiles) {
            const QFileInfoList fileInfoList =  gitFileInfoList(_dirToSave);
            const bool res = QFile::remove(fileInfoList.first().absoluteFilePath());
            if (!res) {
                qCCritical(fifth_category) << "Can not remove log file" << Qt::endl;
            }
            currentQuantityFiles = gitFileInfoList(_dirToSave).size();
        }

    }
    static QString getNewPathFile()
    {
        return QDir(_dirToSave).absolutePath() + "/"
                + QCoreApplication::applicationName()
                + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss-zzz") + ".log";
    }
    static QFileInfoList gitFileInfoList(const QString& dir)
    {
        return QDir(dir).entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::SortFlag::Name);
    }
    static QString getStartLogFile()
    {
        const auto list = gitFileInfoList(_dirToSave);
        if (list.size() > 0) {
            return list.last().absoluteFilePath();
        }
        return getNewPathFile();
    }

private:
    inline static int _quantityFiles = 0;
    inline static qint64 _maxFileSizeByte = 0;
    inline static QString _dirToSave;
};
