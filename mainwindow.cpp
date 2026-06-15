/**
 * @file        mainwindow.cpp
 * @brief       文件搜索工具主窗口实现文件
 * @version     V1.1 修复状态标记、线程、刷新、路径问题
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
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QDir>
#include <QMenu>
#include <QRect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/res/Icon.jpg"));
    loadConfig();

    // 关闭表格默认编辑
    ui->table_result->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 菜单-关闭程序
    connect(ui->actioncancel,&QAction::triggered,[=](){
        this->close();
    });

    // 初始化状态栏
    m_statusLabel = new QLabel("就绪", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addWidget(m_statusLabel, 1);

    m_timeLabel = new QLabel("时间消耗", this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addWidget(m_timeLabel, 1);

    m_filterLabel = new QLabel("当前无过滤", this);
    m_filterLabel->setAlignment(Qt::AlignCenter);
    statusBar()->addPermanentWidget(m_filterLabel, 1);
    updateFilterLabel();

    // 高级过滤弹窗
    connect(ui->actionfilter, &QAction::triggered, [=]() {
        FilterDialog dlg(this);
        dlg.setCurrentFilter(m_filterName, m_filterSuffix);
        if (dlg.exec() == QDialog::Accepted)
        {
            QString newSuffix = dlg.isSuffixEnabled() ? dlg.getSuffixText() : "";
            QString newName = dlg.isNameEnabled() ? dlg.getNameText() : "";
            if (newSuffix != m_filterSuffix || newName != m_filterName)
            {
                m_filterSuffix = newSuffix;
                m_filterName = newName;
                updateFilterLabel();
                startSearch();
            }
        }
    });

    // 清空过滤
    connect(ui->actionclear_filter, &QAction::triggered, [=](){
        if (m_filterSuffix.isEmpty() && m_filterName.isEmpty())
        {
            QMessageBox::information(this, "提示", "当前无过滤条件");
            return;
        }
        m_filterSuffix.clear();
        m_filterName.clear();
        updateFilterLabel();
        QMessageBox::information(this, "提示", "已清空所有过滤条件");
    });

    setWindowTitle("文件搜索工具");
    setAcceptDrops(true);

    // 按钮绑定
    connect(ui->btn_selectDir, &QPushButton::clicked, this, &MainWindow::selectDir);
    connect(ui->btn_start, &QPushButton::clicked, this, &MainWindow::startSearch);
    connect(ui->btn_stop, &QPushButton::clicked, this, &MainWindow::stopSearch);

    // 表格列设置
    ui->table_result->setColumnCount(2);
    ui->table_result->setHorizontalHeaderLabels({"相对路径", "文件名"});
    ui->table_result->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->table_result->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 表格双击
    connect(ui->table_result, &QTableWidget::itemDoubleClicked, this, [=](QTableWidgetItem *item){
        if (m_isSearching) stopSearch();
        int row = item->row();
        QString relPath = ui->table_result->item(row, 0)->text();
        QString fileName = ui->table_result->item(row, 1)->text();
        QString fullPath = QDir(m_rootPath).filePath(relPath + fileName);
        QString folderPath = QFileInfo(fullPath).absolutePath();
        int col = item->column();
        if (col == 1)
        {
            QString nativePath = QDir::toNativeSeparators(fullPath);
            QProcess::startDetached("explorer", QStringList() << "/separate" << "/select," << nativePath);
        }
        else
        {
            QProcess::startDetached("explorer", QStringList() << "/separate" << folderPath);
        }
    });

    // 表格右键菜单
    ui->table_result->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->table_result, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos){
        QModelIndex index = ui->table_result->indexAt(pos);
        if (!index.isValid()) return;
        int row = index.row();
        QString relPath = ui->table_result->item(row, 0)->text();
        QString fileName = ui->table_result->item(row, 1)->text();
        QString fullPath = QDir(m_rootPath).filePath(relPath + fileName);
        QString folderPath = QFileInfo(fullPath).absolutePath();

        QMenu *menu = new QMenu(this);
        QAction *actOpenFile = menu->addAction("打开文件");
        QAction *actOpenDir = menu->addAction("打开所在文件夹");
        QAction *actCopyPath = menu->addAction("复制完整路径");

        connect(menu, &QMenu::triggered, this, [=](QAction *action){
            if (m_isSearching) stopSearch();
            QTimer::singleShot(0, this, [=](){
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
                    QApplication::clipboard()->setText(fullPath);
                }
                menu->deleteLater();
            });
        });
        connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
        menu->popup(ui->table_result->mapToGlobal(pos));
    });
}

// 拖拽
void MainWindow::dragEnterEvent(QDragEnterEvent * event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (!mimeData->hasUrls()) return;
    QList<QUrl> urls = mimeData->urls();
    if (urls.isEmpty()) return;
    QString path = urls.first().toLocalFile();
    if (path.isEmpty()) return;
    QFileInfo info(path);
    QString dirPath = info.isDir() ? path : info.absolutePath();
    ui->edit_path->setText(dirPath);
    saveConfig();
    m_statusLabel->setText(QString("已设置搜索目录: %1").arg(dirPath));
    event->acceptProposedAction();
}

void MainWindow::updateFilterLabel()
{
    QStringList filters;
    if (!m_filterSuffix.isEmpty()) filters << "后缀：" + m_filterSuffix;
    if (!m_filterName.isEmpty()) filters << "名称：" + m_filterName;
    m_filterLabel->setText(filters.isEmpty() ? "当前无过滤" : "当前过滤：" + filters.join(" | "));
}

void MainWindow::loadConfig()
{
    QSettings cfg("config.ini", QSettings::IniFormat);
    ui->edit_path->setText(cfg.value("Path/LastDir", "").toString());
}

void MainWindow::saveConfig()
{
    QSettings cfg("config.ini", QSettings::IniFormat);
    cfg.setValue("Path/LastDir", ui->edit_path->text());
}

// 窗口关闭：优雅停止，不再 terminate
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_engine)
        m_engine->stopSearch();
    if (m_searchThread && m_searchThread->isRunning())
        m_searchThread->wait(1000);

    saveConfig();
    QMainWindow::closeEvent(event);
}

void MainWindow::selectDir()
{
    QString defaultPath = ui->edit_path->text().trimmed();
    if (defaultPath.isEmpty()) defaultPath = QDir::homePath();
    QString path = QFileDialog::getExistingDirectory(
        this, "选择搜索路径", defaultPath, QFileDialog::DontUseNativeDialog);
    if (!path.isEmpty())
    {
        ui->edit_path->setText(path);
        saveConfig();
        qInfo() << "用户选择目录：" << path;
    }
}

// 核心：启动搜索（修复状态标记、线程、信号槽）
void MainWindow::startSearch()
{
    // 状态判断：现在 m_isSearching 初始为 false，首次点击正常进入
    if(m_isSearching)
    {
        QMessageBox::information(this, "提示", "搜索正在进行中，请先停止");
        return;
    }

    QString path = ui->edit_path->text().trimmed();
    if(path.isEmpty())
    {
        QMessageBox::warning(this,"提示","请先选择搜索路径");
        return;
    }

    m_time.start();
    m_timeLabel->setText("搜索中...");
    m_rootPath = path;

    // 清空表格 + 缓存
    ui->table_result->setRowCount(0);
    m_fileCount = 0;
    m_pendingFiles.clear();
    m_isUpdating = false;

    // 按钮状态
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);
    m_statusLabel->setText("正在搜索中，请稍后......");

    // 优雅清理上一轮资源，彻底删除 terminate
    if (m_engine)
        m_engine->stopSearch();
    if (m_searchThread && m_searchThread->isRunning())
        m_searchThread->wait(1000);

    // 新建引擎 & 线程
    SearchEngine * engine = new SearchEngine;
    QThread * thread = new QThread;
    m_engine = engine;
    m_searchThread = thread;

    engine->moveToThread(m_searchThread);
    engine->setFilter(m_filterSuffix, m_filterName);

    // 修复信号槽签名不匹配：使用 Lambda 中转
    connect(thread, &QThread::started, this, [this, engine](){
        engine->startSearch(m_rootPath);
    });

    connect(engine, &SearchEngine::foundFileBatch, this, &MainWindow::onFileFoundBatch);
    connect(engine, &SearchEngine::searchFinished, this, &MainWindow::onSearchFinished);
    connect(engine, &SearchEngine::searchFinished, thread, &QThread::quit);

    // 线程结束自动释放
    connect(thread, &QThread::finished, engine, &SearchEngine::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::finished, this, [this](){
        m_engine = nullptr;
        m_searchThread = nullptr;
    });

    m_searchThread->start();
    m_isSearching = true; // 标记为搜索中
}

void MainWindow::stopSearch()
{
    if (m_engine)
        m_engine->stopSearch();

    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
    m_statusLabel->setText("正在停止搜索...");

    // 清空渲染缓存
    m_pendingFiles.clear();
    m_isUpdating = false;
}

// 表格结果接收 + 节流缓存
void MainWindow::onFileFoundBatch(const QStringList &paths)
{
    m_pendingFiles.append(paths);
    if (!m_isUpdating)
    {
        m_isUpdating = true;
        QTimer::singleShot(50, this, &MainWindow::renderPendingFiles);
    }
}

// 延时渲染表格（修复无 updateRow 问题）
void MainWindow::renderPendingFiles()
{
    if (m_pendingFiles.isEmpty())
    {
        m_isUpdating = false;
        return;
    }

    QStringList paths = m_pendingFiles;
    m_pendingFiles.clear();

    QDir rootDir(m_rootPath);
    int startRow = ui->table_result->rowCount();
    int total = paths.size();
    ui->table_result->setRowCount(startRow + total);

    for (int i = 0; i < total; ++i)
    {
        const QString &path = paths[i];
        QFileInfo fileInfo(path);
        QString relDir = rootDir.relativeFilePath(fileInfo.path());
        QString fileName = fileInfo.fileName();

        QTableWidgetItem *itemDir = new QTableWidgetItem(relDir + "/");
        QTableWidgetItem *itemName = new QTableWidgetItem(fileName);
        itemDir->setFlags(itemDir->flags() & ~Qt::ItemIsEditable);
        itemName->setFlags(itemName->flags() & ~Qt::ItemIsEditable);

        int curRow = startRow + i;
        ui->table_result->setItem(curRow, 0, itemDir);
        ui->table_result->setItem(curRow, 1, itemName);

        // 局部区域刷新，兼容所有 Qt 版本
        QRect rect = ui->table_result->visualRect(ui->table_result->model()->index(curRow, 0));
        ui->table_result->viewport()->update(rect);

        m_fileCount++;
    }

    m_statusLabel->setText(QString("已经找到 %1 个文件").arg(m_fileCount));
    m_isUpdating = false;
}

// 搜索完成：复位状态标记（关键）
void MainWindow::onSearchFinished()
{
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);

    qint64 ms = m_time.elapsed();
    double sec = ms / 1000.0;
    m_timeLabel->setText(QString("耗时：%1 s").arg(sec, 0, 'f', 2));
    m_statusLabel->setText(QString("搜索完成，共找到 %1 个文件").arg(m_fileCount));

    // 必须复位状态，下次才能正常点击
    m_isSearching = false;
}

MainWindow::~MainWindow()
{
    // 析构前优雅停止
    if (m_engine)
        m_engine->stopSearch();
    if (m_searchThread && m_searchThread->isRunning())
        m_searchThread->wait(1000);

    delete ui;
}