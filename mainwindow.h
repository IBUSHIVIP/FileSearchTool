/**
 * @file        mainwindow.h
 * @brief       文件搜索工具主窗口头文件
 * @details     主界面布局、信号槽、多线程管理、拖拽、配置持久化、右键菜单
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QString>
#include "searchengine.h"
#include <QThread>
#include <QPoint>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class       MainWindow
 * @brief       文件搜索工具主窗口
 * @note        功能清单：
 *              1. 目录选择、多线程文件检索、启停控制
 *              2. 文件名/后缀高级过滤、过滤状态展示
 *              3. 表格展示结果、双击/右键打开文件/文件夹、复制路径
 *              4. 文件夹拖拽导入、本地配置持久化
 *              5. 检索耗时统计、状态栏状态提示
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /**
     * @brief       构造函数
     * @param       parent  父窗口指针
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief       析构函数，释放资源
     */
    ~MainWindow() override;

    // 槽函数声明
private slots:
    /**
     * @brief       弹出目录选择对话框
     */
    void selectDir();

    /**
     * @brief       启动文件搜索（创建子线程+检索引擎）
     */
    void startSearch();

    /**
     * @brief       停止当前搜索任务
     */
    void stopSearch();

    /**
     * @brief       接收引擎批量搜索结果，刷新表格
     * @param       paths  批量文件路径列表
     */
    void onFileFoundBatch(const QStringList &paths);

    /**
     * @brief       搜索任务完成收尾工作
     */
    void onSearchFinished();

private:
    /**
     * @brief       加载本地配置（上次搜索目录）
     */
    void loadConfig();

    /**
     * @brief       保存当前配置到本地
     */
    void saveConfig();

    /**
     * @brief       窗口关闭事件，安全终止子线程、保存配置
     * @param       event  关闭事件对象
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * @brief       更新状态栏过滤信息文本
     */
    void updateFilterLabel();

    /**
     * @brief       拖拽进入事件（判断是否为文件/文件夹）
     * @param       event  拖拽事件对象
     */
    void dragEnterEvent(QDragEnterEvent * event);

    /**
     * @brief       拖拽释放事件（读取路径并设置）
     * @param       event  拖拽事件对象
     */
    void dropEvent(QDropEvent *event);

private:
    Ui::MainWindow *ui;

    bool            m_isSearching;      ///< 全局搜索状态标记
    SearchEngine*   m_engine;           ///< 搜索引擎对象
    QThread*        m_searchThread;     ///< 检索子线程

    QString         m_rootPath;         ///< 搜索根目录（用于计算相对路径）
    int             m_fileCount;        ///< 已检索到的文件总数

    QLabel*         m_statusLabel;      ///< 左侧状态栏：运行状态
    QLabel*         m_timeLabel;        ///< 中间状态栏：检索耗时
    QElapsedTimer   m_time;             ///< 耗时计时器
    QLabel*         m_filterLabel;      ///< 右侧状态栏：当前过滤规则

    QString         m_filterSuffix;     ///< 当前生效的文件后缀过滤
    QString         m_filterName;       ///< 当前生效的文件名关键词过滤

    bool            m_autoStartOnDrop;  ///< 拖拽目录后是否自动开始搜索
};

#endif // MAINWINDOW_H