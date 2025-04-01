#include <QApplication>
#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QFormLayout>
#include <QDialog>
#include <QDebug>
#include <vector>
#include <QPixmap>
#include <QMouseEvent>
#include <QIcon>
#include <QPalette>
#include <QBrush>
#include <QPropertyAnimation>
#include <QTimer>
#include <QSettings>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QMenuBar>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCalendarWidget>
#include <QDateEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QScrollArea>
#include <QStackedWidget>
#include <QActionGroup>
#include "mainwindow.h"

// 定义学生结构体
struct Student {
    int id;
    QString name;
    int age;
    QString major;
};

// 定义待办事项结构体
struct TodoItem {
    QString title;
    QString description;
    QDate dueDate;
    bool completed;
};

// 主窗口类
class StudentManagementSystem : public QMainWindow {
private:
    // 添加成员变量
    QStackedWidget *stackedWidget;
    WelcomeScreen *welcomeScreen;
    QWidget *mainScreen;
    QMenu *themeMenu;
    QLabel *woodfishLabel;
    QLabel *meritLabel;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    int meritCount;
    QPropertyAnimation *woodfishAnimation;

public:
    StudentManagementSystem(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("学生管理与待办事项系统");
        resize(1400, 800);
        
        // 创建堆叠部件
        stackedWidget = new QStackedWidget(this);
        
        // 创建欢迎界面
        welcomeScreen = new WelcomeScreen(this);
        
        // 创建主界面
        mainScreen = new QWidget(this);
        QHBoxLayout *mainLayout = new QHBoxLayout(mainScreen);
        
        // 创建各个面板
        QWidget *welcomePanel = createWelcomePanel();
        QWidget *leftPanel = createStudentPanel();
        QWidget *rightPanel = createTodoPanel();
        
        mainLayout->addWidget(welcomePanel, 2);
        mainLayout->addWidget(leftPanel, 3);
        mainLayout->addWidget(rightPanel, 2);
        
        // 添加到堆叠部件
        stackedWidget->addWidget(welcomeScreen);
        stackedWidget->addWidget(mainScreen);
        setCentralWidget(stackedWidget);
        
        // 添加主题菜单
        setupThemes();
        
        // 连接信号
        connect(welcomeScreen, &WelcomeScreen::enterClicked, this, &StudentManagementSystem::showMainScreen);
        
        // 初始化音频播放器
        player = new QMediaPlayer(this);
        audioOutput = new QAudioOutput(this);
        player->setAudioOutput(audioOutput);
        player->setSource(QUrl("qrc:/sounds/woodfish.mp3"));
        
        // 初始化功德值
        loadMeritCount();
        
        // 加载数据
        loadStudents();
        loadTodoItems();
        
        // 设置窗口样式
        setStyleSheet(R"(
            QMainWindow {
                background-color: #f5f5f5;
            }
            QWidget#leftPanel, QWidget#rightPanel {
                background-color: white;
                border-radius: 10px;
                margin: 5px;
            }
            QPushButton {
                padding: 8px 15px;
                border-radius: 5px;
                font-weight: bold;
                min-width: 100px;
            }
            QPushButton#primary {
                background-color: #2196F3;
                color: white;
            }
            QPushButton#primary:hover {
                background-color: #1976D2;
            }
            QPushButton#secondary {
                background-color: #4CAF50;
                color: white;
            }
            QPushButton#secondary:hover {
                background-color: #388E3C;
            }
            QLineEdit {
                padding: 5px;
                border: 2px solid #ddd;
                border-radius: 5px;
            }
            QLineEdit:focus {
                border-color: #2196F3;
            }
            QTableWidget {
                border: none;
                gridline-color: #ddd;
            }
            QTableWidget::item {
                padding: 5px;
            }
            QListWidget {
                border: none;
                border-radius: 5px;
                background-color: #f8f9fa;
            }
            QListWidget::item {
                padding: 10px;
                margin: 2px 0;
                border-radius: 3px;
            }
            QListWidget::item:hover {
                background-color: #e9ecef;
            }
        )");
    }

private:
    QTableWidget *tableWidget;
    QLineEdit *searchEdit;
    std::vector<Student> students;
    QListWidget *todoList;
    std::vector<TodoItem> todoItems;
    
    QWidget* createStudentPanel() {
        QWidget *panel = new QWidget;
        panel->setObjectName("leftPanel");
        QVBoxLayout *layout = new QVBoxLayout(panel);
        
        // 创建标题
        QLabel *title = new QLabel("学生管理", panel);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; margin: 10px 0;");
        layout->addWidget(title);
        
        // 创建搜索区域
        QHBoxLayout *searchLayout = new QHBoxLayout;
        searchEdit = new QLineEdit(panel);
        QPushButton *searchButton = new QPushButton("搜索", panel);
        searchButton->setObjectName("primary");
        searchLayout->addWidget(searchEdit);
        searchLayout->addWidget(searchButton);
        layout->addLayout(searchLayout);
        
        // 创建表格
        tableWidget = new QTableWidget(panel);
        tableWidget->setColumnCount(4);
        tableWidget->setHorizontalHeaderLabels({"学号", "姓名", "年龄", "专业"});
        tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(tableWidget);
        
        // 创建按钮区域
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *addButton = new QPushButton("添加学生", panel);
        QPushButton *editButton = new QPushButton("编辑学生", panel);
        QPushButton *deleteButton = new QPushButton("删除学生", panel);
        QPushButton *refreshButton = new QPushButton("刷新列表", panel);
        
        addButton->setObjectName("primary");
        editButton->setObjectName("secondary");
        deleteButton->setObjectName("secondary");
        refreshButton->setObjectName("primary");
        
        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addWidget(refreshButton);
        layout->addLayout(buttonLayout);
        
        // 连接信号
        connect(addButton, &QPushButton::clicked, this, &StudentManagementSystem::addStudent);
        connect(editButton, &QPushButton::clicked, this, &StudentManagementSystem::editStudent);
        connect(deleteButton, &QPushButton::clicked, this, &StudentManagementSystem::deleteStudent);
        connect(refreshButton, &QPushButton::clicked, this, &StudentManagementSystem::refreshStudentList);
        connect(searchButton, &QPushButton::clicked, this, &StudentManagementSystem::searchStudents);
        
        return panel;
    }
    
    QWidget* createTodoPanel() {
        QWidget *panel = new QWidget;
        panel->setObjectName("rightPanel");
        QVBoxLayout *layout = new QVBoxLayout(panel);
        
        // 创建标题
        QLabel *title = new QLabel("待办事项", panel);
        title->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; margin: 10px 0;");
        layout->addWidget(title);
        
        // 创建待办事项列表
        todoList = new QListWidget(panel);
        layout->addWidget(todoList);
        
        // 创建添加待办事项的按钮
        QPushButton *addTodoButton = new QPushButton("添加待办事项", panel);
        addTodoButton->setObjectName("primary");
        layout->addWidget(addTodoButton);
        
        connect(addTodoButton, &QPushButton::clicked, this, &StudentManagementSystem::addTodoItem);
        connect(todoList, &QListWidget::itemDoubleClicked, this, &StudentManagementSystem::toggleTodoItem);
        
        return panel;
    }

    QWidget* createWelcomePanel() {
        QWidget *panel = new QWidget;
        panel->setObjectName("welcomePanel");
        QVBoxLayout *layout = new QVBoxLayout(panel);
        
        // 添加欢迎标题
        QLabel *welcomeTitle = new QLabel("欢迎使用\n学生管理系统", panel);
        welcomeTitle->setAlignment(Qt::AlignCenter);
        welcomeTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #333; margin: 20px 0;");
        layout->addWidget(welcomeTitle);
        
        // 添加木鱼区域
        QVBoxLayout *woodfishLayout = new QVBoxLayout();
        woodfishLabel = new QLabel(panel);
        QPixmap woodfishPixmap(":/images/woodfish.png");
        woodfishLabel->setPixmap(woodfishPixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        woodfishLabel->setAlignment(Qt::AlignCenter);
        woodfishLabel->setCursor(Qt::PointingHandCursor);
        woodfishLabel->setToolTip("点击木鱼获得功德");
        
        meritLabel = new QLabel(QString("功德：%1").arg(meritCount), panel);
        meritLabel->setAlignment(Qt::AlignCenter);
        meritLabel->setStyleSheet("font-size: 18px; color: #666;");
        
        woodfishLayout->addWidget(woodfishLabel);
        woodfishLayout->addWidget(meritLabel);
        layout->addLayout(woodfishLayout);
        
        // 添加装饰图片
        QLabel *decorImage = new QLabel(panel);
        QPixmap moonPixmap(":/images/moon.png");
        decorImage->setPixmap(moonPixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        decorImage->setAlignment(Qt::AlignCenter);
        layout->addWidget(decorImage);
        
        // 安装事件过滤器
        woodfishLabel->installEventFilter(this);
        
        // 初始化动画
        woodfishAnimation = new QPropertyAnimation(woodfishLabel, "geometry", this);
        woodfishAnimation->setDuration(100);
        
        layout->addStretch();
        return panel;
    }

    // 添加事件过滤器
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (obj == woodfishLabel && event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                // 播放点击动画
                QRect geometry = woodfishLabel->geometry();
                QRect smallerGeometry = geometry;
                smallerGeometry.setSize(geometry.size() * 0.9);
                smallerGeometry.moveCenter(geometry.center());
                
                woodfishAnimation->stop();
                woodfishAnimation->setStartValue(geometry);
                woodfishAnimation->setEndValue(smallerGeometry);
                woodfishAnimation->start();
                
                // 设置还原动画
                QTimer::singleShot(50, this, [this, geometry]() {
                    woodfishAnimation->stop();
                    woodfishAnimation->setStartValue(woodfishLabel->geometry());
                    woodfishAnimation->setEndValue(geometry);
                    woodfishAnimation->start();
                });
                
                // 播放音效
                player->setPosition(0);
                player->play();
                
                // 更新功德值
                meritCount++;
                meritLabel->setText(QString("功德：%1").arg(meritCount));
                saveMeritCount();
                
                return true;
            }
        }
        return QMainWindow::eventFilter(obj, event);
    }

    void loadMeritCount() {
        QSettings settings("MyCompany", "StudentManagement");
        meritCount = settings.value("merit", 0).toInt();
        if (meritLabel) {
            meritLabel->setText(QString("功德：%1").arg(meritCount));
        }
    }

    void saveMeritCount() {
        QSettings settings("MyCompany", "StudentManagement");
        settings.setValue("merit", meritCount);
    }

    // 添加学生对话框类
    class StudentDialog : public QDialog {
    public:
        StudentDialog(QWidget *parent = nullptr, const QString &title = "学生信息",
                     int id = 0, const QString &name = "", int age = 0, const QString &major = "") 
            : QDialog(parent) {
            setWindowTitle(title);
            setMinimumWidth(300);
            
            QFormLayout *formLayout = new QFormLayout(this);
            
            idEdit = new QLineEdit(id > 0 ? QString::number(id) : "", this);
            nameEdit = new QLineEdit(name, this);
            ageEdit = new QLineEdit(age > 0 ? QString::number(age) : "", this);
            majorEdit = new QLineEdit(major, this);
            
            formLayout->addRow("学号:", idEdit);
            formLayout->addRow("姓名:", nameEdit);
            formLayout->addRow("年龄:", ageEdit);
            formLayout->addRow("专业:", majorEdit);
            
            QHBoxLayout *buttonLayout = new QHBoxLayout();
            QPushButton *okButton = new QPushButton("确定", this);
            QPushButton *cancelButton = new QPushButton("取消", this);
            buttonLayout->addWidget(okButton);
            buttonLayout->addWidget(cancelButton);
            formLayout->addRow(buttonLayout);
            
            connect(okButton, &QPushButton::clicked, this, &StudentDialog::validateAndAccept);
            connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
        }
        
        int getId() const { return idEdit->text().toInt(); }
        QString getName() const { return nameEdit->text(); }
        int getAge() const { return ageEdit->text().toInt(); }
        QString getMajor() const { return majorEdit->text(); }
        
        void setIdEnabled(bool enabled) { idEdit->setEnabled(enabled); }
        
    private slots:
        void validateAndAccept() {
            if (idEdit->text().isEmpty() || nameEdit->text().isEmpty() || 
                ageEdit->text().isEmpty() || majorEdit->text().isEmpty()) {
                QMessageBox::warning(this, "错误", "所有字段都必须填写！");
                return;
            }
            
            bool idOk, ageOk;
            int id = idEdit->text().toInt(&idOk);
            int age = ageEdit->text().toInt(&ageOk);
            
            if (!idOk || id <= 0) {
                QMessageBox::warning(this, "错误", "学号必须是正整数！");
                return;
            }
            
            if (!ageOk || age <= 0 || age > 100) {
                QMessageBox::warning(this, "错误", "年龄必须是1-100之间的整数！");
                return;
            }
            
            accept();
        }
        
    private:
        QLineEdit *idEdit;
        QLineEdit *nameEdit;
        QLineEdit *ageEdit;
        QLineEdit *majorEdit;
    };
    
    // 学生管理相关函数
    void addStudent() {
        StudentDialog dialog(this, "添加学生");
        if (dialog.exec() == QDialog::Accepted) {
            Student student;
            student.id = dialog.getId();
            student.name = dialog.getName();
            student.age = dialog.getAge();
            student.major = dialog.getMajor();
            
            for (const auto &s : students) {
                if (s.id == student.id) {
                    QMessageBox::warning(this, "错误", "学号已存在！");
                    return;
                }
            }
            
            students.push_back(student);
            saveStudents();
            refreshStudentList();
            QMessageBox::information(this, "成功", "学生添加成功！");
        }
    }
    
    void editStudent() {
        int row = tableWidget->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "警告", "请先选择一个学生！");
            return;
        }
        
        Student &student = students[row];
        StudentDialog dialog(this, "编辑学生", student.id, student.name, student.age, student.major);
        dialog.setIdEnabled(false);
        
        if (dialog.exec() == QDialog::Accepted) {
            student.name = dialog.getName();
            student.age = dialog.getAge();
            student.major = dialog.getMajor();
            saveStudents();
            refreshStudentList();
            QMessageBox::information(this, "成功", "学生信息更新成功！");
        }
    }
    
    void deleteStudent() {
        int row = tableWidget->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "警告", "请先选择一个学生！");
            return;
        }
        
        QMessageBox::StandardButton reply = QMessageBox::question(this, "确认删除", 
            "确定要删除这名学生吗？", QMessageBox::Yes | QMessageBox::No);
            
        if (reply == QMessageBox::Yes) {
            students.erase(students.begin() + row);
            saveStudents();
            refreshStudentList();
            QMessageBox::information(this, "成功", "学生删除成功！");
        }
    }
    
    void searchStudents() {
        QString searchText = searchEdit->text().trimmed().toLower();
        if (searchText.isEmpty()) {
            refreshStudentList();
            return;
        }
        
        std::vector<Student> searchResults;
        for (const auto &student : students) {
            if (QString::number(student.id).contains(searchText) ||
                student.name.toLower().contains(searchText) ||
                student.major.toLower().contains(searchText)) {
                searchResults.push_back(student);
            }
        }
        
        updateTableWithStudents(searchResults);
    }
    
    void loadStudents() {
        QFile file("students.json");
        if (!file.open(QIODevice::ReadOnly)) {
            addDemoData();
            return;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        
        students.clear();
        for (const QJsonValue &value : array) {
            QJsonObject obj = value.toObject();
            Student student;
            student.id = obj["id"].toInt();
            student.name = obj["name"].toString();
            student.age = obj["age"].toInt();
            student.major = obj["major"].toString();
            students.push_back(student);
        }
        
        file.close();
        refreshStudentList();
    }
    
    void saveStudents() {
        QFile file("students.json");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "错误", "无法保存学生数据！");
            return;
        }
        
        QJsonArray array;
        for (const Student &student : students) {
            QJsonObject obj;
            obj["id"] = student.id;
            obj["name"] = student.name;
            obj["age"] = student.age;
            obj["major"] = student.major;
            array.append(obj);
        }
        
        QJsonDocument doc(array);
        file.write(doc.toJson());
        file.close();
    }
    
    void refreshStudentList() {
        searchEdit->clear();
        updateTableWithStudents(students);
    }
    
    void updateTableWithStudents(const std::vector<Student> &students) {
        tableWidget->setRowCount(students.size());
        for (size_t i = 0; i < students.size(); ++i) {
            const Student &student = students[i];
            tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(student.id)));
            tableWidget->setItem(i, 1, new QTableWidgetItem(student.name));
            tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(student.age)));
            tableWidget->setItem(i, 3, new QTableWidgetItem(student.major));
        }
    }

    void addDemoData() {
        students = {
            {1, "张三", 20, "计算机科学与技术"},
            {2, "李四", 21, "软件工程"},
            {3, "王五", 22, "信息安全"}
        };
        saveStudents();
        refreshStudentList();
    }

    void addTodoItem() {
        QDialog dialog(this);
        dialog.setWindowTitle("添加待办事项");
        
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        
        QLineEdit *titleEdit = new QLineEdit(&dialog);
        titleEdit->setPlaceholderText("标题");
        
        QLineEdit *descEdit = new QLineEdit(&dialog);
        descEdit->setPlaceholderText("描述");
        
        QDateEdit *dateEdit = new QDateEdit(QDate::currentDate(), &dialog);
        dateEdit->setCalendarPopup(true);
        
        QPushButton *okButton = new QPushButton("添加", &dialog);
        okButton->setObjectName("primary");
        
        layout->addWidget(new QLabel("标题:"));
        layout->addWidget(titleEdit);
        layout->addWidget(new QLabel("描述:"));
        layout->addWidget(descEdit);
        layout->addWidget(new QLabel("截止日期:"));
        layout->addWidget(dateEdit);
        layout->addWidget(okButton);
        
        connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);
        
        if (dialog.exec() == QDialog::Accepted) {
            TodoItem item{
                titleEdit->text(),
                descEdit->text(),
                dateEdit->date(),
                false
            };
            todoItems.push_back(item);
            updateTodoList();
            saveTodoItems();
        }
    }
    
    void toggleTodoItem(QListWidgetItem *item) {
        int row = todoList->row(item);
        todoItems[row].completed = !todoItems[row].completed;
        updateTodoList();
        saveTodoItems();
    }
    
    void updateTodoList() {
        todoList->clear();
        for (const auto &item : todoItems) {
            QString text = QString("%1\n%2\n截止日期: %3")
                .arg(item.title)
                .arg(item.description)
                .arg(item.dueDate.toString("yyyy-MM-dd"));
            
            QListWidgetItem *listItem = new QListWidgetItem(text);
            listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
            listItem->setCheckState(item.completed ? Qt::Checked : Qt::Unchecked);
            
            if (item.completed) {
                listItem->setBackground(QBrush(QColor("#e8f5e9")));
            } else if (item.dueDate < QDate::currentDate()) {
                listItem->setBackground(QBrush(QColor("#ffebee")));
            }
            
            todoList->addItem(listItem);
        }
    }
    
    void loadTodoItems() {
        QFile file("todo.json");
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }
        
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();
        
        todoItems.clear();
        for (const QJsonValue &value : array) {
            QJsonObject obj = value.toObject();
            TodoItem item{
                obj["title"].toString(),
                obj["description"].toString(),
                QDate::fromString(obj["dueDate"].toString(), Qt::ISODate),
                obj["completed"].toBool()
            };
            todoItems.push_back(item);
        }
        
        file.close();
        updateTodoList();
    }
    
    void saveTodoItems() {
        QFile file("todo.json");
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "错误", "无法保存待办事项！");
            return;
        }
        
        QJsonArray array;
        for (const auto &item : todoItems) {
            QJsonObject obj;
            obj["title"] = item.title;
            obj["description"] = item.description;
            obj["dueDate"] = item.dueDate.toString(Qt::ISODate);
            obj["completed"] = item.completed;
            array.append(obj);
        }
        
        QJsonDocument doc(array);
        file.write(doc.toJson());
        file.close();
    }

    void setupThemes() {
        themeMenu = menuBar()->addMenu("主题");
        QActionGroup *themeGroup = new QActionGroup(this);
        
        auto addTheme = [this, themeGroup](const QString &name, const QString &stylesheet) {
            QAction *action = themeMenu->addAction(name);
            action->setCheckable(true);
            themeGroup->addAction(action);
            connect(action, &QAction::triggered, [this, stylesheet]() {
                setStyleSheet(stylesheet);
            });
        };
        
        addTheme("默认主题", getDefaultTheme());
        addTheme("自然主题", getNatureTheme());
        addTheme("柔和主题", getSoftTheme());
        addTheme("紫色主题", getPurpleTheme());
        
        themeGroup->actions().first()->setChecked(true);
    }
    
    QString getDefaultTheme() const {
        return R"(
            QMainWindow, QDialog {
                background-color: #f5f5f5;
            }
            QWidget#leftPanel, QWidget#rightPanel {
                background-color: white;
                border-radius: 10px;
            }
            QPushButton#primary {
                background-color: #2196F3;
                color: white;
            }
            QPushButton#secondary {
                background-color: #4CAF50;
                color: white;
            }
        )";
    }
    
    QString getNatureTheme() const {
        return R"(
            QMainWindow, QDialog {
                background-color: #E8F5E9;
            }
            QWidget#leftPanel, QWidget#rightPanel {
                background-color: #FFFFFF;
                border-radius: 10px;
                border: 1px solid #81C784;
            }
            QLabel {
                color: #2E7D32;
            }
            QTableWidget {
                background-color: #FFFFFF;
                color: #1B5E20;
                gridline-color: #A5D6A7;
            }
            QHeaderView::section {
                background-color: #81C784;
                color: white;
                padding: 5px;
                border: none;
            }
            QPushButton#primary {
                background-color: #4CAF50;
                color: white;
            }
            QPushButton#primary:hover {
                background-color: #388E3C;
            }
            QPushButton#secondary {
                background-color: #66BB6A;
                color: white;
            }
            QPushButton#secondary:hover {
                background-color: #43A047;
            }
            QLineEdit {
                border: 2px solid #81C784;
                color: #1B5E20;
            }
        )";
    }
    
    QString getSoftTheme() const {
        return R"(
            QMainWindow, QDialog {
                background-color: #e8f5e9;
            }
            QWidget#leftPanel, QWidget#rightPanel {
                background-color: #f1f8e9;
                border-radius: 10px;
            }
            QPushButton#primary {
                background-color: #66bb6a;
                color: white;
            }
            QPushButton#secondary {
                background-color: #81c784;
                color: white;
            }
        )";
    }
    
    QString getPurpleTheme() const {
        return R"(
            QMainWindow, QDialog {
                background-color: #f3e5f5;
            }
            QWidget#leftPanel, QWidget#rightPanel {
                background-color: #fff3e0;
                border-radius: 10px;
            }
            QPushButton#primary {
                background-color: #9c27b0;
                color: white;
            }
            QPushButton#secondary {
                background-color: #7b1fa2;
                color: white;
            }
        )";
    }

private slots:
    void showMainScreen() {
        stackedWidget->setCurrentWidget(mainScreen);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setStyle("Fusion");
    
    QFont font = app.font();
    font.setFamily("Microsoft YaHei");
    app.setFont(font);
    
    app.setWindowIcon(QIcon(":/images/app_icon.ico"));
    
    StudentManagementSystem window;
    window.show();
    
    return app.exec();
}