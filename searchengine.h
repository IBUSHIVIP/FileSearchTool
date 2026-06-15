#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QString>
#include <atomic>

/**
 * @class       SearchEngine
 * @brief       高性能文件搜索引擎
 */
class SearchEngine : public QObject
{
    Q_OBJECT
public:
    explicit SearchEngine(QObject *parent = nullptr);

public slots:
    void startSearch(const QString & rootDir);
    void stopSearch();
    void setFilter(const QString & filter_suffix, const QString & filter_name);

signals:
    void foundFileBatch(const QStringList &paths);
    void searchFinished();

private:
    bool isMatch(const QString& filePath);
    void searchInDir(const QString & dir);

    QString             m_filter_suffix;
    QString             m_filter_name;
    std::atomic<bool>   m_isStop{false};
};

#endif // SEARCHENGINE_H