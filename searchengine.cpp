/**
 * @file        searchengine.cpp
 * @brief       文件搜索引擎实现文件
 * @details     非递归目录遍历、文件过滤、批量结果上报、线程安全控制
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#include "searchengine.h"
#include <QStack>
#include <QSet>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QString>

/**
 * @brief       全局常量：单文件最大限制（预留扩展，当前未使用）
 */
static const qint64 MAX_CONTENT_FILE_SIZE = 10 * 1024 * 1024;

/**
 * @brief       构造函数
 */
SearchEngine::SearchEngine(QObject *parent)
    : QObject{parent}
{}

/**
 * @brief       启动搜索任务
 * @param       rootDir  检索根目录
 * @note        重置停止标记，调用目录遍历逻辑，完成后发送结束信号
 */
void SearchEngine::startSearch(const QString &rootDir)
{
    m_isStop = false;
    searchInDir(rootDir);
    emit searchFinished();
}

/**
 * @brief       非递归目录遍历核心函数
 * @param       rootDir  起始目录
 * @note        1. 使用QStack手动栈，避免递归深度过大导致程序栈溢出
 *              2. 跳过符号链接目录，防止循环遍历死循环
 *              3. 批量收集结果，20条为一组上报，降低UI刷新压力
 *              4. 实时检测停止标记，响应停止指令
 */
void SearchEngine::searchInDir(const QString &rootDir)
{
    QStack<QString> dirStack;
    dirStack.push(rootDir);

    // 循环遍历栈中所有目录
    while (!dirStack.isEmpty() && !m_isStop)
    {
        QString currentDir = dirStack.pop();
        QDir qDir(currentDir);

        // 获取当前目录下所有文件/目录，排除 . 和 ..
        QFileInfoList list = qDir.entryInfoList(
            QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot
            );

        QStringList batch;  // 批量结果缓存

        for (const QFileInfo& info : list)
        {
            // 检测停止标记，收到停止指令立即退出循环
            if (m_isStop) break;

            if (info.isDir())
            {
                // 跳过符号链接目录，避免循环遍历
                if (info.isSymLink())
                {
                    continue;
                }
                // 子目录压入栈，继续遍历
                dirStack.push(info.absoluteFilePath());
            }
            else if (info.isFile())
            {
                // 校验文件是否符合过滤规则
                if (isMatch(info.absoluteFilePath()))
                {
                    batch << info.absoluteFilePath();
                    // 凑够20条，批量发送结果
                    if (batch.size() >= 20)
                    {
                        emit foundFileBatch(batch);
                        batch.clear();
                    }
                }
            }
        }

        // 遍历结束，发送剩余不足20条的结果
        if (!batch.isEmpty())
        {
            emit foundFileBatch(batch);
        }
    }
}

/**
 * @brief       线程安全停止搜索
 * @note        仅修改原子标记，由遍历循环实时检测并退出
 */
void SearchEngine::stopSearch()
{
    m_isStop = true;
}

/**
 * @brief       设置过滤规则
 * @param       filter_suffix  后缀条件
 * @param       filter_name    文件名关键词条件
 */
void SearchEngine::setFilter(const QString & filter_suffix, const QString & filter_name)
{
    m_filter_suffix = filter_suffix;
    m_filter_name = filter_name;
}

/**
 * @brief       文件规则匹配判断
 * @param       filePath  文件绝对路径
 * @return      匹配返回true
 * @note        不区分大小写；支持单独后缀、单独关键词、双条件组合过滤
 */
bool SearchEngine::isMatch(const QString &filePath)
{
    QFileInfo info(filePath);
    QString fileName = info.fileName().toLower();

    // 1. 后缀过滤（非空则必须匹配）
    if (!m_filter_suffix.isEmpty())
    {
        if (!fileName.endsWith(m_filter_suffix.toLower()))
        {
            return false;
        }
    }

    // 2. 文件名关键词过滤（非空则必须包含）
    if (!m_filter_name.isEmpty())
    {
        if (!fileName.contains(m_filter_name.toLower()))
        {
            return false;
        }
    }

    // 无过滤/全部匹配
    return true;
}