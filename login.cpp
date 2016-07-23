#include "login.h"
#include "ui_login.h"

#include "exam.h"
#include "passwdedit.h"
#include "register.h"

#include <QMessageBox>
#include <QToolButton>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QDebug>

float opacity1 = 0.0, opacity2 = 1.0;

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    init();

    connect(this,SIGNAL(close()),this,SLOT(close()));
}

Login::~Login()
{
    delete ui;
}

void Login::init()
{
    setWindowTitle(tr("登录"));

    ui->btn_edit_pwd->setStyleSheet("background-color:transparent;");
    ui->btn_regist->setStyleSheet("background-color:transparent;");
    ui->btn_login->setStyleSheet("background-color:transparent;");

    m_Drag = false;

    configWindow();//去边框，最小化，最大化button


    //填充背景图片 start
    QPalette palette;
    palette.setBrush(/*QPalette::Background*/this->backgroundRole(),
                     QBrush(QPixmap(":/images/QQ1.png")));
    this->setPalette(palette);
    //填充背景图片 ends



    timer1 = new QTimer;
    timer1->start(5);
    timer2 = new QTimer;
    connect(timer1, SIGNAL(timeout()), this, SLOT(slot_timer1()));
    connect(timer2, SIGNAL(timeout()), this, SLOT(slot_timer2()));

    //添加键盘ico
    QToolButton *keyBtn = new QToolButton(this);
    keyBtn->setIcon(QIcon(":/images/keyBoard.png"));
    keyBtn->setStyleSheet("background-color:transparent;");

    //设置键盘ico坐标
    int x = ui->lineEdit_passwd->x();
    int y = ui->lineEdit_passwd->y();
    int width = ui->lineEdit_passwd->width();
    keyBtn->setGeometry(x+width-20, y, 20, 20);
    //关联键盘exe
    connect(keyBtn, SIGNAL(clicked()), this, SLOT(slot_getKeyBoard()));

    init_sql();//初始化界面密码，帐号的初值

    //init记住密码
    ui->checkBox_rPasswd->setChecked(true);
    ui->lineEdit_passwd->setEchoMode(QLineEdit::Password);
}

void Login::get_user_info()
{
    user_info_stu.userName.clear();
    user_info_stu.userName = ui->cBox_account->currentText();
    user_info_stu.passwd.clear();
    user_info_stu.passwd = ui->lineEdit_passwd->text();
}

void Login::configWindow()
{
    //去掉窗口边框
    setWindowFlags(Qt::FramelessWindowHint);
    //获取界面的宽度
    int width = this->width();
    //构建最小化、最大化、关闭按钮
    QToolButton *minBtn = new QToolButton(this);
    QToolButton *closeBbtn= new QToolButton(this);
//    //获取最小化、关闭按钮图标
//    QPixmap minPix  = style()->standardPixmap(QStyle::SP_TitleBarMinButton);
//    QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);
//    //设置最小化、关闭按钮图标
//    minBtn->setIcon(minPix);
//    closeBbtn->setIcon(closePix);
    //设置最小化、关闭按钮在界面的位置
    minBtn->setGeometry(width-55,5,20,20);
    closeBbtn->setGeometry(width-25,5,20,20);
    //设置鼠标移至按钮上的提示信息
    minBtn->setToolTip(tr("最小化"));
    closeBbtn->setToolTip(tr("关闭"));
    //设置最小化、关闭按钮的样式
    minBtn->setStyleSheet("background-color:transparent;");
    closeBbtn->setStyleSheet("background-color:transparent;");
    //关联最小化、关闭按钮的槽函数
    connect(minBtn, SIGNAL(clicked()), this, SLOT(slot_minWindow()));
    connect(closeBbtn, SIGNAL(clicked()), this, SLOT(slot_closeWindow()));
}

void Login::init_sql()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
    if (!db.open()){
        qDebug() << "database open fail!";
    }
    else
    {
        qDebug() << "database open success!";
        QSqlQuery q;

        //创建一个名为userInfo的表 顺序为: 用户名 密码 email
         QString sql_create_table = "CREATE TABLE userInfo (name VARCHAR PRIMARY KEY,passwd VARCHAR, email VARCHAR)";
         q.prepare(sql_create_table);
       // q.exec("CREATE TABLE userInfo (name VARCHAR PRIMARY KEY,passwd VARCHAR, email VARCHAR)");
        if(!q.exec())
        {
            qDebug()<<"creater table error";
        }
/*
        //选择数据库里面，table名字为userInfo的表
        bool tableFlag=false;
        QString sql_select_table = "select tbl_name userInfo from sqlite_master where type = 'table'";
        q.prepare(sql_select_table);
        if(!q.exec())
        {
            qDebug()<<"select table error";
        }
        else
        {
            QString tableName;
            while(q.next())
            {
                tableName = q.value(0).toString();
                qDebug()<<tableName;
                if(tableName.compare("userInfo"))//查找表名是否和user相匹配
                {
                    tableFlag=false;
                    qDebug()<<"table is not exist";
                }
                else
                {
                    tableFlag=true;
                    qDebug()<<"table is exist";
                }
            }
        }
*/

        q.exec("insert into userInfo values ('operator','operator','help@demo.com')");
        q.exec("select * from userInfo");

        while (q.next())
        {
            QString userName = q.value(0).toString();
            ui->cBox_account->addItem(userName);
            QString passwd = q.value(1).toString();
            userPasswd.append(passwd);
            qDebug() << "userName:::"<< userName << "passwd:::" << passwd;
        }
        ui->cBox_account->setCurrentIndex(0);
        ui->lineEdit_passwd->setText(userPasswd.at(0));
    }
    db.close();
    qDebug()<<"database closed!";
}

void Login::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_Drag = true;
        m_point = e->globalPos() - this->pos();
        e->accept();
      //  qDebug()<<"leo";
    }
}

void Login::mouseMoveEvent(QMouseEvent *e)
{
    if (m_Drag && (e->buttons() && Qt::LeftButton)) {
        move(e->globalPos() - m_point);
        e->accept();
       // qDebug()<<"leomove";
    }
}

void Login::mouseReleaseEvent(QMouseEvent *e)
{
    m_Drag = false;
}

void Login::on_btn_login_clicked()
{
    if(ui->cBox_account->currentText().isEmpty() ||
            ui->lineEdit_passwd->text().isEmpty()){
        QMessageBox::warning(this,tr("警告"),tr("请输入用户名和密码！"));
    }
    else
    {
        int is_use_exist_flag = 0;       //判断用户是否存在
        int is_use_nampwd_check_flag = 0;       //判断用户名和密码是否匹配
        get_user_info();

        if(!db.open())
        {
            qDebug() << "database open fail login!";
        }
        else
        {
            QSqlQuery query;
            qDebug() << "database open success login!";
            query.exec("select * from userInfo");
            while (query.next())
            {
                QString userName = query.value(0).toString();
                QString passwd = query.value(1).toString();
                qDebug() << "login userName:::"<< userName << "passwd:::" << passwd;

                if(userName == user_info_stu.userName){
                    is_use_exist_flag = true;              //用户存在
                    if(passwd == user_info_stu.passwd){
                        is_use_nampwd_check_flag = true;          //用户名和密码匹配
                        Exam *e = new Exam;
                        e->show();
                        emit close();
                    }
                }
            }

            if(is_use_exist_flag == false)
            {
                QMessageBox::information(this,tr("提示"),tr("用户不存在！"));
            }
            else
            {
                if(is_use_nampwd_check_flag == false)
                {
                    QMessageBox::warning(this,tr("警告"),tr("用户密码错误！"));
                }
            }
        }

        db.close();
    }
}


//注册button
void Login::on_btn_regist_clicked()
{
    Register r(this);
    this->hide();
    r.show();
   //transmitdb(database);
    r.exec();
    this->show();
}

/*
//注册button
void Login::on_btn_regist_clicked()
{
    get_user_info();
    if(user_info_stu.userName.isEmpty() || user_info_stu.passwd.isEmpty()){
        QMessageBox::information(this,tr("提示"),tr("请输入用户名和密码！"));
    }
    else
    {
        bool exitFlag = false;       //判断用户是否存在

        if(!db.open())
        {
            qDebug() << "database open fail regist!";
        }
        else
        {
            QSqlQuery query;
            qDebug() << "database open success regist!";
            query.exec("select * from userInfo");
            while (query.next())
            {
                QString userName = query.value(0).toString();
                QString passwd = query.value(1).toString();
                qDebug() << "regist userName:::"<< userName << "passwd:::" << passwd;

                if(userName == user_info_stu.userName){
                    exitFlag = true;              //用户存在
                }
            }

            if(exitFlag == false){
                query.exec(tr("insert into userInfo values ('%1','%2')")
                           .arg(user_info_stu.userName).arg(user_info_stu.passwd));
                qDebug()<<"regist:::"<<query.lastQuery();

                ui->cBox_account->addItem(user_info_stu.userName);
                userPasswd.append(user_info_stu.passwd);
                QMessageBox::information(this,tr("提示"),tr("注册成功！"));
            }else{
                QMessageBox::warning(this,tr("警告"),tr("用户已存在！"));
            }
        }
        db.close();
    }
}
*/

//修改密码button
void Login::on_btn_edit_pwd_clicked()
{
    if(ui->cBox_account->currentText().isEmpty() ||
            ui->lineEdit_passwd->text().isEmpty()){
        QMessageBox::information(this,tr("提示"),tr("请输入用户名和密码！"));
    }
    else
    {
        bool is_use_exist_flag = false;       //判断用户是否存在
        bool is_use_nampwd_check_flag = false;       //判断用户名和密码是否匹配
        get_user_info();

        if(!db.open())
        {
            qDebug() << "database open fail login!";
        }
        else
        {
            QSqlQuery query;
            qDebug() << "database open success login!";
            query.exec("select * from userInfo");
            while (query.next())
            {
                QString userName = query.value(0).toString();
                QString passwd = query.value(1).toString();
                qDebug() << "edit userName:::"<< userName << "passwd:::" << passwd;

                if(userName == user_info_stu.userName)
                {
                    is_use_exist_flag = true;              //用户存在
                    if(passwd == user_info_stu.passwd)
                    {
                        is_use_nampwd_check_flag = true;          //用户名和密码匹配
                        passwdEdit passwd;
                        passwd.setLogin(this);
                        //this->hide();
                        passwd.exec();
                    }
                }
            }

            if(is_use_exist_flag == false)
            {
                QMessageBox::information(this,tr("提示"),tr("用户不存在！"));
            }
            else
            {
                if(is_use_nampwd_check_flag == 0){
                    QMessageBox::warning(this,tr("警告"),tr("用户密码错误！"));
                }
            }
        }
        db.close();
    }
}

void Login::slot_minWindow()
{
    this->showMinimized();
}

void Login::slot_closeWindow()
{
    timer2->start(5);
}

void Login::slot_getKeyBoard()
{
    qDebug() << "key!";

//    QString m_path("D:/login/images/osk.exe");
//    int ret = curPath.compare(m_path);
//    qDebug() << "curPath:" << curPath << "ret:" << ret;
//    QString path;
//    if (ret != 0){
//        QStringList pathList = curPath.split("/");
//        path = pathList.at(0);
//        path.append("/login/images/osk.exe");
//    }else{
//        path = m_path;
//    }
    QString curPath = QApplication::applicationDirPath();
    curPath.append("/osk.exe");
    qDebug() << "curPath:" << curPath;

    QDesktopServices::openUrl(QUrl(curPath, QUrl::TolerantMode));
}

void Login::slot_timer1()
{
    if (opacity1 >= 1.0) {
        timer1->stop();
    }else{
        opacity1 += 0.01;
    }
    setWindowOpacity(opacity1);//设置窗口透明度
}

void Login::slot_timer2()
{
    if (opacity2 <= 0.0) {
        timer2->stop();

        this->close();
    }else{
        opacity2 -= 0.01;
    }
    setWindowOpacity(opacity2);//设置窗口透明度
}

void Login::on_cBox_account_activated(int index)
{
    ui->lineEdit_passwd->setText(userPasswd.at(index));
    qDebug() << "change cBox:" << ui->cBox_account->currentText()
             << userPasswd.at(index);
}



