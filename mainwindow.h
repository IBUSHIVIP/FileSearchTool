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

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void selectDir();
    void startSearch();
    void stopSearch();
    void onFileFoundBatch(const QStringList &paths);
    void onSearchFinished();
    void renderPendingFiles();  // 新增：表格延时渲染槽

private:
    void loadConfig();
    void saveConfig();
    void closeEvent(QCloseEvent *event) override;
    void updateFilterLabel();
    void dragEnterEvent(QDragEnterEvent * event) override;
    void dropEvent(QDropEvent *event) override;

private:
    Ui::MainWindow *ui;
    // 全部成员强制初始化，解决随机脏值
    bool            m_isSearching{false};
    SearchEngine*   m_engine{nullptr};
    QThread*        m_searchThread{nullptr};
    QString         m_rootPath;
    int             m_fileCount{0};
    QLabel*         m_statusLabel{nullptr};
    QLabel*         m_timeLabel{nullptr};
    QElapsedTimer   m_time;
    QLabel*         m_filterLabel{nullptr};
    QString         m_filterSuffix;
    QString         m_filterName;
    bool            m_autoStartOnDrop{false};

    // 表格刷新节流（解决之前表格不实时刷新问题）
    bool            m_isUpdating{false};
    QStringList     m_pendingFiles;
};
#endif // MAINWINDOW_H