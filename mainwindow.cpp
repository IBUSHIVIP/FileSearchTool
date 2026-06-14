/**
 * @file        mainwindow.cpp
 * @brief       文件搜索工具主窗口实现文件
 * @details     界面初始化、信号槽绑定、多线程管理、结果渲染、拖拽、右键菜单、配置读写
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "searchengine.h"
#include "filterdialog.h"
#include <QLabel>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QThread>
#include <QTimer>
#include <QHeaderView>
#include <QSettings>
#include <QClipboard>
#include <QProcess>
#include <QElapsedTimer>
#include <QtConcurrentRun>
#include <QDesktopServices>
#include <QMetaObject>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QMenu>

/**
 * @brief       主窗口构造函数：初始化界面、控件、信号槽、配置
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/Icon.jpg"));

    // 加载上次保存的配置
    loadConfig();

    // 菜单-关闭程序
    connect(ui->actioncancel,&QAction::triggered,[=](){
        this->close();
    });

    // 初始化三栏状态栏
    m_statusLabel = new QLabel("就绪", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addWidget(m_statusLabel, 1);

    m_timeLabel = new QLabel("时间消耗", this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addWidget(m_timeLabel, 1);

    m_filterLabel = new QLabel("当前过滤：不过滤", this);
    m_filterLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_filterLabel, 1);
    updateFilterLabel();

    // 菜单：打开高级过滤弹窗
    connect(ui->actionfilter, &QAction::triggered, [=]() {
        FilterDialog dlg(this);
        dlg.setCurrentFilter(m_filterName, m_filterSuffix);
        if (dlg.exec() == QDialog::Accepted)
        {
            m_filterSuffix.clear();
            m_filterName.clear();
            if (dlg.isSuffixEnabled())
                m_filterSuffix = dlg.getSuffixText();
            if (dlg.isNameEnabled())
                m_filterName = dlg.getNameText();

            updateFilterLabel();
            startSearch();
        }
    });

    // 菜单：清空所有过滤条件
    connect(ui->actionclear_filter, &QAction::triggered, [=](){
        m_filterSuffix.clear();
        m_filterName.clear();
        updateFilterLabel();
        startSearch();
        QMessageBox::information(this, "提示", "已清空所有过滤条件");
    });

    setWindowTitle("文件搜索工具");
    setAcceptDrops(true);   // 开启窗口拖拽支持

    // 按钮信号槽：选择目录、开始、停止搜索
    connect(ui->btn_selectDir, &QPushButton::clicked,[=](){
        this->selectDir();
    });
    connect(ui->btn_start, &QPushButton::clicked, [=](){
        this->startSearch();
    });
    connect(ui->btn_stop, &QPushButton::clicked, [=](){
        this->stopSearch();
    });

    // 初始化结果表格：两列布局、自适应宽度
    ui->table_result->setColumnCount(2);
    ui->table_result->setHorizontalHeaderLabels({"相对路径", "文件名"});
    ui->table_result->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->table_result->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 表格双击事件：打开文件/所在文件夹
    connect(ui->table_result, &QTableWidget::itemDoubleClicked, this, [=](QTableWidgetItem *item){
        if (m_isSearching)
            stopSearch();

        int row = item->row();
        int col = item->column();
        QString relative = ui->table_result->item(row, 0)->text();
        QString fileName = ui->table_result->item(row, 1)->text();
        QString fullPath = QDir(m_rootPath).filePath(relative + fileName);
        QString folderPath = QFileInfo(fullPath).absolutePath();

        if (col == 1)
        {
            // 选中文件名：打开资源管理器并高亮文件
            QString nativePath = QDir::toNativeSeparators(fullPath);
            QProcess::startDetached("explorer", QStringList() << "/separate" << "/select," << nativePath);
        }
        else
        {
            // 选中路径：直接打开文件夹
            QProcess::startDetached("explorer", QStringList() << "/separate" << folderPath);
        }
    });

    // 表格右键菜单：打开文件、打开目录、复制路径
    ui->table_result->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table_result, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos){
        QModelIndex index = ui->table_result->indexAt(pos);
        if (!index.isValid()) return;

        int row = index.row();
        int col = index.column();
        QString relPath = ui->table_result->item(row, 0)->text();
        QString fileName = ui->table_result->item(row, 1)->text();
        QString fullPath = QDir(m_rootPath).filePath(relPath + fileName);
        QString folderPath = QFileInfo(fullPath).absolutePath();

        QMenu *menu = new QMenu(this);
        QAction *actOpenFile = nullptr;
        if (col == 1)
            actOpenFile = menu->addAction("打开文件");

        QAction *actOpenDir = menu->addAction("打开所在文件夹");
        QAction *actCopyPath = menu->addAction("复制完整路径");

        // 菜单点击逻辑
        connect(menu, &QMenu::triggered, this, [=](QAction *action){
            if (m_isSearching)
                stopSearch();

            // 延迟执行，避免菜单未销毁引发异常
            QTimer::singleShot(0, this, [=]() {
                if (action == actOpenFile)
                {
                    QString nativePath = QDir::toNativeSeparators(fullPath);
                    QProcess::startDetached("explorer", QStringList() << "/separate" << "/select," << nativePath);
                }
                else if (action == actOpenDir)
                {
                    QProcess::startDetached("explorer", QStringList() << "/separate" << folderPath);
                }
                else if (action == actCopyPath)
                {
                    if (col == 1)
                        QApplication::clipboard()->setText(fullPath);
                    else
                        QApplication::clipboard()->setText(relPath);
                }
            });
            menu->deleteLater();
        });

        connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
        menu->popup(ui->table_result->mapToGlobal(pos));
    });
}

/**
 * @brief       拖拽进入校验：仅接受文件/文件夹URL
 */
void MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

/**
 * @brief       拖拽释放：提取路径并填充到输入框
 */
void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (!mimeData->hasUrls()) return;

    QList<QUrl> urls = mimeData->urls();
    if (urls.isEmpty()) return;

    QString path = urls.first().toLocalFile();
    if (path.isEmpty()) return;

    QFileInfo info(path);
    // 文件取所在目录，文件夹直接使用
    QString dirPath = info.isDir() ? path : info.absolutePath();
    ui->edit_path->setText(dirPath);

    saveConfig();
    if (m_autoStartOnDrop)
        startSearch();

    m_statusLabel->setText(QString("已设置搜索目录: %1").arg(dirPath));
    event->acceptProposedAction();
}

/**
 * @brief       更新状态栏过滤规则文本
 */
void MainWindow::updateFilterLabel()
{
    QStringList filters;
    if (!m_filterSuffix.isEmpty())
        filters << "后缀：" + m_filterSuffix;
    if (!m_filterName.isEmpty())
        filters << "名称：" + m_filterName;

    if (filters.isEmpty())
        m_filterLabel->setText("当前无过滤");
    else
        m_filterLabel->setText("当前过滤：" + filters.join(" | "));
}

/**
 * @brief       加载ini配置文件（记录上次搜索目录）
 */
void MainWindow::loadConfig()
{
    QSettings cfg("config.ini", QSettings::IniFormat);
    QString path = cfg.value("Path/LastDir", "").toString();
    ui->edit_path->setText(path);
}

/**
 * @brief       保存当前目录到ini配置文件
 */
void MainWindow::saveConfig()
{
    QSettings cfg("config.ini", QSettings::IniFormat);
    cfg.setValue("Path/LastDir", ui->edit_path->text());
}

/**
 * @brief       窗口关闭事件：安全终止子线程、保存配置
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    // 停止并销毁搜索线程与引擎
    if (m_engine)
    {
        m_engine->stopSearch();
        if (m_searchThread && m_searchThread->isRunning())
        {
            m_searchThread->terminate();
            m_searchThread->wait();
        }
    }
    saveConfig();
    QMainWindow::closeEvent(event);
}

/**
 * @brief       弹出目录选择框，记忆上次选择路径
 */
void MainWindow::selectDir()
{
    QString defaultPath = ui->edit_path->text().trimmed();
    if (defaultPath.isEmpty())
        defaultPath = QDir::homePath();

    QString path = QFileDialog::getExistingDirectory(
        this,
        "选择搜索路径",
        defaultPath,
        QFileDialog::DontUseNativeDialog
        );

    if (!path.isEmpty())
    {
        ui->edit_path->setText(path);
        qInfo() << "用户选择目录：" << path;
        saveConfig();
    }
}

/**
 * @brief       启动搜索：创建独立子线程，分离UI与检索逻辑
 * @note        核心优化：耗时检索全部在子线程执行，杜绝UI卡顿
 */
void MainWindow::startSearch()
{
    m_time.start();
    m_timeLabel->setText("搜索中...");

    // 禁止重复启动
    if(m_isSearching)
    {
        QMessageBox::information(this, "提示", "搜索正在进行中，请稍后或者停止搜索");
        return;
    }

    QString path = ui->edit_path->text();
    if(path.isEmpty())
    {
        QMessageBox::warning(this,"提示","请先选择正确路径");
        return;
    }

    m_rootPath = path;
    ui->table_result->setRowCount(0);
    m_fileCount = 0;

    // 更新按钮与状态栏状态
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);
    m_statusLabel->setText("正在搜索中，请稍后......");

    // 清理上一轮残留的引擎与线程
    if (m_engine)
    {
        m_engine->stopSearch();
        if (m_searchThread && m_searchThread->isRunning())
        {
            disconnect(m_engine, nullptr, this, nullptr);
            m_engine->deleteLater();
            m_searchThread->terminate();
            m_searchThread->wait();
            m_searchThread->deleteLater();
        }
        m_engine = nullptr;
        m_searchThread = nullptr;
    }

    // 新建引擎与子线程
    SearchEngine * engine = new SearchEngine;
    QThread * thread = new QThread;
    m_engine = engine;
    m_searchThread = thread;

    // 将引擎移入子线程（核心：对象依附线程执行）
    engine->moveToThread(m_searchThread);
    // 传递当前过滤规则
    engine->setFilter(m_filterSuffix, m_filterName);

    // 绑定信号槽
    connect(thread, &QThread::started, engine, [=]() {
        engine->startSearch(m_rootPath);
    });
    connect(engine, &SearchEngine::foundFileBatch, this, &MainWindow::onFileFoundBatch);
    connect(engine, &SearchEngine::searchFinished, this, &MainWindow::onSearchFinished);
    connect(engine, &SearchEngine::searchFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, engine, &SearchEngine::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
    m_isSearching = true;
}

/**
 * @brief       停止搜索：设置原子停止标记，不阻塞UI
 */
void MainWindow::stopSearch()
{
    if (m_engine)
        m_engine->stopSearch();

    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
    m_statusLabel->setText("正在停止搜索...");
}

/**
 * @brief       批量接收搜索结果，写入表格
 * @note        临时关闭表格重绘，大批量数据优化渲染速度
 */
void MainWindow::onFileFoundBatch(const QStringList &paths)
{
    if (paths.isEmpty()) return;

    QDir rootDir(m_rootPath);
    int startRow = ui->table_result->rowCount();
    int total = paths.size();

    // 关闭自动重绘，多次添加后统一刷新，提升性能
    ui->table_result->setUpdatesEnabled(false);
    ui->table_result->setRowCount(startRow + total);

    for (int i = 0; i < total; ++i)
    {
        const QString &path = paths[i];
        QString relative = rootDir.relativeFilePath(path);
        QString fileName = QFileInfo(path).fileName();

        ui->table_result->setItem(startRow + i, 0, new QTableWidgetItem(relative.remove(fileName)));
        ui->table_result->setItem(startRow + i, 1, new QTableWidgetItem(fileName));
        m_fileCount++;
    }

    // 恢复重绘并刷新视图
    ui->table_result->setUpdatesEnabled(true);
    ui->table_result->viewport()->update();
    m_statusLabel->setText(QString("已经找到 %1 个文件").arg(m_fileCount));
}

/**
 * @brief       搜索完成收尾：统计耗时、恢复按钮状态
 */
void MainWindow::onSearchFinished()
{
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);

    // 计算并展示检索耗时
    qint64 ms = m_time.elapsed();
    double sec = ms / 1000.0;
    m_timeLabel->setText(QString("耗时：%1 s").arg(sec, 0, 'f', 2));

    m_statusLabel->setText(
        QString("搜索完成，共找到 %1 个文件").arg(m_fileCount));

    m_engine = nullptr;
    m_isSearching = false;
}

/**
 * @brief       析构函数
 */
MainWindow::~MainWindow()
{
    delete ui;
}