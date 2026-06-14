/**
 * @file        filterdialog.cpp
 * @brief       高级过滤弹窗实现文件
 * @details     界面布局、控件初始化、参数回填、参数读取逻辑
 * @author      我没有会员
 * @date        2026-06-14
 * @version     V1.0
 */
#include "filterdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

/**
 * @brief       构造函数：初始化界面布局与控件
 * @note        垂直布局为主，分为过滤选项区、功能按钮区
 */
FilterDialog::FilterDialog(QWidget *parent)
    : QDialog{parent}
{
    setWindowTitle("高级过滤界面");

    // 主垂直布局
    QVBoxLayout * mainLayout = new QVBoxLayout(this);

    // 1. 文件名过滤组件
    chk_name = new QCheckBox("按文件名关键词过滤");
    edit_name = new QLineEdit();
    edit_name->setPlaceholderText("例如：test、report、2025");

    // 2. 文件后缀过滤组件
    chk_suffix = new QCheckBox("按文件后缀过滤");
    edit_suffix = new QLineEdit();
    edit_suffix->setPlaceholderText("例如：cpp");

    // 添加过滤控件到主布局
    mainLayout->addWidget(chk_name);
    mainLayout->addWidget(edit_name);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(chk_suffix);
    mainLayout->addWidget(edit_suffix);

    // 3. 底部按钮水平布局
    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *btn_ok = new QPushButton("确定");
    QPushButton *btn_cancel = new QPushButton("取消");
    btnLayout->addStretch();
    btnLayout->addWidget(btn_ok);
    btnLayout->addWidget(btn_cancel);
    mainLayout->addLayout(btnLayout);

    // 绑定按钮点击信号
    connect(btn_ok, &QPushButton::clicked, this, &QDialog::accept);
    connect(btn_cancel, &QPushButton::clicked, this, &QDialog::reject);
}

/**
 * @brief       回填历史过滤条件
 * @param       name    文件名关键词
 * @param       suffix  文件后缀
 * @note        非空则勾选对应选项并填充内容，空则清空并取消勾选
 */
void FilterDialog::setCurrentFilter(const QString &name, const QString &suffix)
{
    // 处理文件名过滤项
    if (!name.isEmpty()) {
        chk_name->setChecked(true);
        edit_name->setText(name);
    } else {
        chk_name->setChecked(false);
        edit_name->clear();
    }

    // 处理后缀过滤项
    if (!suffix.isEmpty()) {
        chk_suffix->setChecked(true);
        edit_suffix->setText(suffix);
    } else {
        chk_suffix->setChecked(false);
        edit_suffix->clear();
    }
}

/**
 * @brief       获取文件名关键词（去除首尾空格）
 */
QString FilterDialog::getNameText() const {
    return edit_name->text().trimmed();
}

/**
 * @brief       获取文件后缀（去除首尾空格）
 */
QString FilterDialog::getSuffixText() const {
    return edit_suffix->text().trimmed();
}

/**
 * @brief       文件名过滤是否启用
 */
bool FilterDialog::isNameEnabled() const {
    return chk_name->isChecked();
}

/**
 * @brief       后缀过滤是否启用
 */
bool FilterDialog::isSuffixEnabled() const {
    return chk_suffix->isChecked();
}