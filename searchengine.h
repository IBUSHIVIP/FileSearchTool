#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QString>
#include <atomic> /**
 * @file        searchengine.h
 * @brief       文件搜索引擎头文件
 * @details     独立子线程运行的核心检索模块，非递归目录遍历、双条件过滤、线程安全启停
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <QString>
#include <atomic>

/**
 * @class       SearchEngine
 * @brief       高性能文件搜索引擎
 * @note        核心特性：
 *              1. 非递归栈遍历目录，规避深层目录栈溢出风险
 *              2. std::atomic 原子变量实现线程安全启停
 *              3. 批量上报结果，减少UI刷新次数、优化渲染性能
 *              4. 支持文件名关键词 + 文件后缀双条件过滤
 *              5. 自动跳过符号链接，防止循环遍历死锁
 */
class SearchEngine : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief       构造函数
     * @param       parent  父对象指针
     */
    explicit SearchEngine(QObject *parent = nullptr);

public slots:
    /**
     * @brief       开始执行文件搜索
     * @param       rootDir  检索根目录
     */
    void startSearch(const QString & rootDir);

    /**
     * @brief       停止当前搜索任务（线程安全）
     */
    void stopSearch();

    /**
     * @brief       设置过滤规则
     * @param       filter_suffix  文件后缀过滤条件
     * @param       filter_name    文件名关键词过滤条件
     */
    void setFilter(const QString & filter_suffix, const QString & filter_name);

signals:
    /**
     * @brief       批量检索结果信号
     * @param       paths  一批文件绝对路径列表
     * @note        每收集20条结果触发一次，优化UI大批量渲染卡顿
     */
    void foundFileBatch(const QStringList &paths);

    /**
     * @brief       整个检索任务完成信号
     */
    void searchFinished();

private:
    /**
     * @brief       单个文件匹配判断函数
     * @param       filePath  文件绝对路径
     * @return      满足过滤条件返回 true，否则 false
     */
    bool isMatch(const QString& filePath);

    /**
     * @brief       递归遍历目录核心逻辑（栈实现非递归）
     * @param       dir  当前遍历目录
     */
    void searchInDir(const QString & dir);

    QString             m_filter_suffix;    ///< 文件后缀过滤条件
    QString             m_filter_name;      ///< 文件名关键词过滤条件
    std::atomic<bool>   m_isStop;           ///< 原子停止标记，多线程读写安全
};

#endif // SEARCHENGINE_H  // 添加

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

    QString m_filter_suffix;
    QString m_filter_name;
    std::atomic<bool> m_isStop{false};   // 原子布尔，默认 false
};

#endif // SEARCHENGINE_H