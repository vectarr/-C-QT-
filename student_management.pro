QT       += core gui widgets multimedia multimedia-private

# 添加 JSON 支持
QT += core

# 添加动画支持
CONFIG += animation

CONFIG += c++17

TEMPLATE = app
TARGET = student_management

# 设置源文件
SOURCES += \
    main.cpp

# 设置头文件
HEADERS += \
    mainwindow.h

# 设置资源文件
RESOURCES += \
    resources.qrc

# 设置 Windows 应用程序图标
RC_ICONS = images/app_icon.ico

# Windows 特定配置
win32: {
    QMAKE_LFLAGS += -Wl,-subsystem,windows
}

# 基本配置
CONFIG += windows
DEFINES += QT_DEPRECATED_WARNINGS

# 添加样式支持
QT += widgets

# 启用高DPI支持
DEFINES += QT_ENABLE_HIGH_DPI_SCALING
CONFIG += high_dpi

# 启用内联类的 moc 处理
CONFIG += force_moc
