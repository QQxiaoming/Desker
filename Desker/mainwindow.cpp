/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： mianwindow.cpp
* 文件标识： mainwindow
* 摘    要: 主窗口
******************************************************************************
* 当前版本： 1.0
* 作者    : zhiliao007
* 创建日期： 2018-01-01T15:13:15
******************************************************************************
*/

#include <QString>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QPainter>
#include <QDebug>
#include <QDir>
#include <QDesktopServices>
#include <QStringList>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#if defined(Q_OS_WIN)
#include <QtWin>
#include "windows_api.h"
#endif

#if defined(Q_OS_LINUX)
#include <QProcess>
#endif

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //去掉任务栏和标题栏的最大化显示
    this->setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
    this->showMaximized();

#if defined(Q_OS_WIN)
    ui->label->setPixmap(getIcon("D:\\Program Files (x86)\\Arduino\\arduino.exe", true));

    QDir dir("C:\\Users\\qq_xi\\Desktop");
    dir.setFilter(QDir::Files | QDir::Executable);
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        qDebug() << QString("%1").arg(fileInfo.fileName());
    }
    qDebug() << QString("%1").arg(list.size());

    //提取桌面路径某图标，支持中文名称
    QString Path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    ui->label->setPixmap(getIcon(Path + "//图像.exe", true));
    //ui->label->setPixmap(getIcon("D:\\Program Files (x86)\\Arduino\\arduino.exe",true));
#endif
#if defined(Q_OS_LINUX)
    QString desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    qDebug() << desktop_path;
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label->setPixmap(getIcon(desktop_path + "/Visual Studio Code", true).scaled(QSize(48, 48), Qt::KeepAspectRatio));
    ui->label_2->setText("Visual Studio Code");
    //ui->label_2->setStyleSheet("text-align:center;background-color:#FFFFFF;color:black");
    ui->label_2->setStyleSheet("text-align:center;background:transparent;color:white");
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

QPixmap MainWindow::getIcon(const QString sourceFile, bool sizeFlag)
{
#if defined(Q_OS_WIN)
    // ExtractIconEx 从限定的可执行文件、动态链接库（DLL）、或者图标文件中生成图标句柄数组
    const UINT iconCount = ExtractIconEx((wchar_t *)sourceFile.utf16(), -1, 0, 0, 0);
    if (!iconCount)
    {
        qDebug() << QString("%1 does not appear to contain icons.").arg(sourceFile);
    }

    QScopedArrayPointer<HICON> icons(new HICON[iconCount]);

    if (sizeFlag)
    {
        if (!ExtractIconEx((wchar_t *)sourceFile.utf16(), 0, icons.data(), 0, iconCount))
            ExtractIconEx((wchar_t *)sourceFile.utf16(), 0, 0, icons.data(), iconCount);
    }
    else
        ExtractIconEx((wchar_t *)sourceFile.utf16(), 0, 0, icons.data(), iconCount);

    return QtWin::fromHICON(icons[0]);

#endif

#if defined(Q_OS_LINUX)
    return QPixmap("/snap/vscode/72/usr/share/pixmaps/code.png");
#endif
}

//重写右击事件
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QAction *pNew = new QAction("新建格子", this);
    //QIcon icoNew(":/images/images/new.png");
    //pNew->setIcon(icoNew);
    menu->addAction(pNew);
    //connect(pNew,SIGNAL(triggered()),this,SLOT(close()));

    QAction *pExit = new QAction("退出整理", this);
    QIcon icoExit(":/images/images/exit.png");
    pExit->setIcon(icoExit);
    menu->addAction(pExit);
    connect(pExit, SIGNAL(triggered()), qApp, SLOT(quit())); //直接退出程序

    QAction *pSet = new QAction("设置中心", this);
    //QIcon icoSet(":/images/images/set.png");
    //pSet->setIcon(icoSet);
    menu->addAction(pSet);
    //connect(pSet,SIGNAL(triggered()),this,SLOT(close()));

    QAction *pAbout = new QAction("帮助关于", this);
    QIcon icoAbout(":/images/images/help.png");
    pAbout->setIcon(icoAbout);
    menu->addAction(pAbout);
    //connect(pAbout,SIGNAL(triggered()),this,SLOT(close()));

    menu->move(cursor().pos());
    menu->show();
}

//重写鼠标双击事件
void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    //如果鼠标按下的是左键
    if (event->buttons() == Qt::LeftButton)
    {
        //如果lable为显示，将label设置为隐藏
        if (ui->label->isVisible())
        {
            ui->label->setVisible(false);
            ui->label_2->setVisible(false);
        }
        else
        {
            ui->label->setVisible(true);
            ui->label_2->setVisible(true);
        }
    }
}

//背景重绘
void MainWindow::paintEvent(QPaintEvent *event)
{
    QString paths = NULL;
    QPainter painter(this);

#if defined(Q_OS_WIN)
    //调用windows API获取桌面壁纸路径
    TCHAR chPath[MAX_PATH];
    SystemParametersInfo(SPI_GETDESKWALLPAPER, MAX_PATH, chPath, 0);
    QString path = TCHARToQString(chPath);
    paths = path.replace(QRegExp("\\\\"), "/");
#elif defined(Q_OS_LINUX)
    /* 这里调用dcof获取桌面壁纸路径 */
    QProcess process;

    /* gnome*/
    //process.start("gsettings", QStringList()<<"get"<<"org.gnome.desktop.background"<<"picture-uri");
    /* mate */
    process.start("gsettings", QStringList() << "get"
                                             << "org.mate.background"
                                             << "picture-filename");

    if (process.waitForStarted(-1))
    {
        while (process.waitForReadyRead(-1))
        {
            //qDebug() << "readAllStandardOutput:" << process.readAllStandardOutput();
            QString path = QString::fromLocal8Bit(process.readAllStandardOutput());
            path = path.replace(QRegExp("\'"), "");
            paths = path.replace(QRegExp("\n"), "");
        }
    }
#endif

#ifdef QT_NO_DEBUG

#else
    qDebug() << "paths addr:" << paths;
#endif //QT_NO_DEBUG

    //绘制背景
    painter.drawPixmap(0, 0, width(), height(), QPixmap(paths));
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
