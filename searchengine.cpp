#include "searchengine.h"
#include <QStack>
#include <QDir>
#include <QFileInfo>
#include <QStringList>

static const qint64 MAX_CONTENT_FILE_SIZE = 10 * 1024 * 1024;

SearchEngine::SearchEngine(QObject *parent)
    : QObject{parent}
{
    m_isStop = false;
}

void SearchEngine::startSearch(const QString &rootDir)
{
    m_isStop = false;
    searchInDir(rootDir);
    emit searchFinished();
}

void SearchEngine::searchInDir(const QString &rootDir)
{
    QStack<QString> dirStack;
    dirStack.push(rootDir);

    while (!dirStack.isEmpty() && !m_isStop)
    {
        QString currentDir = dirStack.pop();
        QDir qDir(currentDir);
        QFileInfoList list = qDir.entryInfoList(
            QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot
            );

        QStringList batch;
        for (const QFileInfo& info : list)
        {
            if (m_isStop) break;
            if (info.isSymLink()) continue;

            if (info.isDir())
            {
                dirStack.push(info.absoluteFilePath());
            }
            else if (info.isFile())
            {
                if (isMatch(info.absoluteFilePath()))
                {
                    batch << info.absoluteFilePath();
                    // 改为5条一批，提升实时性
                    if (batch.size() >= 5)
                    {
                        emit foundFileBatch(batch);
                        batch.clear();
                    }
                }
            }
        }
        if (!batch.isEmpty())
        {
            emit foundFileBatch(batch);
        }
    }
}

void SearchEngine::stopSearch()
{
    m_isStop = true;
}

void SearchEngine::setFilter(const QString & filter_suffix, const QString & filter_name)
{
    m_filter_suffix = filter_suffix.trimmed().toLower();
    m_filter_name = filter_name.trimmed().toLower();
}

// 修复后缀精准匹配
bool SearchEngine::isMatch(const QString &filePath)
{
    QFileInfo info(filePath);
    QString fileName = info.fileName().toLower();
    QString fileSuffix = info.suffix().toLower();

    if (!m_filter_suffix.isEmpty())
    {
        if (fileSuffix != m_filter_suffix)
            return false;
    }
    if (!m_filter_name.isEmpty())
    {
        if (!fileName.contains(m_filter_name))
            return false;
    }
    return true;
}