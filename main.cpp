/**
 * @file        main.cpp
 * @brief       程序入口文件
 * @details     重定向Qt日志系统、按天分割日志文件、初始化应用与主窗口
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#include "mainwindow.h"
#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QThreadPool>
#include <QRunnable>

/**
 * @brief       全局日志重定向回调函数
 * @param       type        日志等级(调试/信息/警告/错误/致命错误)
 * @param       context     日志上下文(文件、行号等)
 * @param       msg         日志内容
 * @note        1. 自动创建log目录；2. 按日期拆分日志文件；3. 日志格式：[时间] [等级] [内容]
 */
void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 匹配日志等级
    QString level;
    switch (type) {
    case QtDebugMsg:    level = "DEBUG";    break;
    case QtInfoMsg:     level = "INFO";     break;
    case QtWarningMsg:  level = "WARN";     break;
    case QtCriticalMsg: level = "ERROR";    break;
    case QtFatalMsg:    level = "FATAL";    break;
    default:            level = "LOG";     break;
    }

    // 获取当前时间
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    // 拼接完整日志行
    QString log = QString("[%1] [%2] [%3]").arg(time, level, msg);

    // 确保日志目录存在
    QDir dir;
    if(!dir.exists("log")){
        dir.mkpath("log");
    }

    // 按日期命名日志文件，实现日志分割
    QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString path = QString("log/log_%1.log").arg(date);

    // 追加写入日志
    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)){
        QTextStream out(&file);
        out << log << "\n";
        file.close();
    }
}

/**
 * @brief       程序入口函数
 * @note        优先安装日志处理器，再启动Qt应用，保证全局日志生效
 */
int main(int argc, char *argv[])
{
    // 注册自定义日志重定向（必须在QApplication创建之前执行）
    qInstallMessageHandler(myMessageHandler);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    qInfo() << "程序成功启动";
    return QCoreApplication::exec();
}