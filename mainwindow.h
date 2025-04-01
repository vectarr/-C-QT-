#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QDateTime>

class WelcomeScreen : public QWidget {
    Q_OBJECT
public:
    explicit WelcomeScreen(QWidget *parent = nullptr) {
        setMinimumSize(800, 600);
        
        // 创建背景标签
        bgLabel = new QLabel(this);
        bgLabel->setObjectName("bgLabel");
        bgLabel->setScaledContents(true);
        
        // 设置背景图片
        QPixmap bgImage(":/images/moon.png");
        bgLabel->setPixmap(bgImage);
        bgLabel->resize(size());
        
        // 创建内容部件
        contentWidget = new QWidget(this);
        QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
        contentLayout->setAlignment(Qt::AlignCenter);
        
        // 创建时钟标签
        timeLabel = new QLabel(this);
        timeLabel->setObjectName("timeLabel");
        timeLabel->setStyleSheet(R"(
            color: white;
            font-size: 20px;
            background-color: rgba(0, 0, 0, 150);
            padding: 10px;
            border-radius: 10px;
        )");
        
        // 创建标题
        QLabel *titleLabel = new QLabel("欢迎使用\n学生管理系统", this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(R"(
            font-size: 42px;
            font-weight: bold;
            color: white;
            margin: 20px;
            background-color: rgba(0, 0, 0, 150);
            padding: 20px;
            border-radius: 15px;
        )");
        
        // 创建按钮
        QPushButton *enterButton = new QPushButton("开始使用", this);
        enterButton->setFixedWidth(200);
        enterButton->setStyleSheet(R"(
            QPushButton {
                padding: 15px 50px;
                font-size: 18px;
                border-radius: 25px;
                background-color: rgba(33, 150, 243, 180);
                color: white;
                border: none;
            }
            QPushButton:hover {
                background-color: rgba(25, 118, 210, 200);
            }
        )");
        
        contentLayout->addWidget(timeLabel, 0, Qt::AlignRight | Qt::AlignTop);
        contentLayout->addStretch();
        contentLayout->addWidget(titleLabel);
        contentLayout->addWidget(enterButton, 0, Qt::AlignHCenter);
        contentLayout->addStretch();
        
        // 创建定时器
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &WelcomeScreen::updateTime);
        timer->start(1000);
        
        connect(enterButton, &QPushButton::clicked, this, &WelcomeScreen::enterClicked);
        
        // 设置初始大小
        contentWidget->resize(size());
        updateTime();
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QWidget::resizeEvent(event);
        bgLabel->resize(size());
        contentWidget->resize(size());
    }

signals:
    void enterClicked();

private slots:
    void updateTime() {
        timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }

private:
    QLabel *bgLabel;
    QWidget *contentWidget;
    QLabel *timeLabel;
    QTimer *timer;
};
