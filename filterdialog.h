/**
 * @file        filterdialog.h
 * @brief       高级过滤弹窗头文件
 * @details     提供文件名关键词、文件后缀双条件过滤配置界面，支持参数回填与读取
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QString>
#include <QLineEdit>
#include <QCheckBox>

/**
 * @class       FilterDialog
 * @brief       文件高级过滤对话框
 * @note        支持两种过滤规则：文件名模糊匹配、文件后缀精准匹配
 *              可回填历史过滤条件，操作完成后返回配置参数
 */
class FilterDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief       构造函数
     * @param       parent  父窗口指针
     */
    explicit FilterDialog(QWidget *parent = nullptr);

    /**
     * @brief       获取输入的文件名关键词
     * @return      去除首尾空格后的字符串
     */
    QString getNameText() const;

    /**
     * @brief       获取输入的文件后缀名
     * @return      去除首尾空格后的字符串
     */
    QString getSuffixText() const;

    /**
     * @brief       判断是否启用文件名过滤
     * @return      勾选返回 true，否则 false
     */
    bool isNameEnabled() const;

    /**
     * @brief       判断是否启用后缀过滤
     * @return      勾选返回 true，否则 false
     */
    bool isSuffixEnabled() const;

    /**
     * @brief       回填当前过滤条件到输入框
     * @param       name    文件名关键词
     * @param       suffix  文件后缀
     */
    void setCurrentFilter(const QString& name, const QString& suffix);

signals:

private:
    QLineEdit   *edit_name;     ///< 文件名关键词输入框
    QLineEdit   *edit_suffix;   ///< 文件后缀输入框
    QCheckBox   *chk_name;      ///< 启用文件名过滤复选框
    QCheckBox   *chk_suffix;    ///< 启用后缀过滤复选框
};

#endif // FILTERDIALOG_H