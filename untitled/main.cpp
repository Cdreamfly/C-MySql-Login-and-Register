#include<iostream>
#include <mysql/mysql.h>
#include<string>
using namespace std;
#include <cstdio>

#define MAX_RETRY_TIMES 4
#define DB_NAME "USR"
#define DB_HOST "127.0.0.1"
#define DB_PORT 3306
#define DB_USER "root"
#define DB_USER_PASSWD "cmf.199991"

bool IsdigitAll(string str) //判断是否全是数字
{
    if(str.length()>11)
    {
        cout<<"帐号超限！"<<endl;
        return false;
    }
    if(str.length()<<11&&str.length()!=11)
    {
        cout<<"帐号过小！"<<endl;
        return false;
    }
    for (int i = 0; i<str.size(); i++)
    {
        if (!isdigit(str[i]))
        {
            cout<<"帐号必须是数字！"<<endl;
            return false;
        }
    }
    return true;
}

typedef struct _userinfo{
    int id; /* 用户 id */
    string username; /* 用户名 */
    string passwd; /* 密码 */
}userinfo;

bool connect_db(MYSQL& mysql)
{
    /* 1. 初始化数据库句柄 */
    mysql_init(&mysql);

    /* 2. 设置字符编码 */
    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf8");

    /* 3. 连接数据库 */
    if (mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_USER_PASSWD, DB_NAME, DB_PORT, nullptr, 0) == NULL)
    {
        printf("数据库连接出错, 错误原因: %s \n", mysql_error(&mysql));
        return false;
    }

    return true;
}
/* 读取用户信息 */

bool fetch_user_info(userinfo& user)
{
    MYSQL mysql;
    MYSQL_RES* res; /* 查询结果集 */
    MYSQL_ROW row; /* 记录结构体 */
    char sql[256];
    bool ret = false;


    /* 1. 连接到数据库 */
    if (connect_db(mysql) == false)
    {
        return false;
    }

    /* 2. 根据用户名和密码获取用户信息(id, leve_id) */
    snprintf(sql, 256,
             "select * from USR.usr where name='%s' and pwd = '%s';",
             user.username.c_str(), user.passwd.c_str());

    ret = mysql_query(&mysql, sql); /* 成功返回0 */

    if (ret)
    {
        printf("数据库查询出错, %s 错误原因: %s\n", sql, mysql_error(&mysql));
        mysql_close(&mysql);
        return false;
    }


    /* 3. 获取结果 */
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == nullptr) /* 没有查找到记录 */
    {
        cout<<"没有查找到记录"<<endl;
        mysql_free_result(res);
        mysql_close(&mysql);
        return false;
    }

    user.username = row[1];
    printf("username: %s \n", user.username.c_str()); /* 打印 ID */

    /* 4. 返回结果 */

    /* 释放结果集 */
    mysql_free_result(res);

    /* 关闭数据库 */
    mysql_close(&mysql);

    return true;
}
bool insert_user_info(userinfo& user)
{
    MYSQL mysql;
    bool ret = false;

    /* 1. 连接到数据库 */
    if (connect_db(mysql) == false)
    {
        return false;
    }

    /* 2. 根据用户名和密码获取用户信息(id, leve_id) */
    string sql = "insert into USR.usr values('0','" +user.username+ "','" +user.passwd+ "');";
    ret = mysql_query(&mysql, sql.data()); /* 成功返回0 */

    if (ret)
    {
        printf("数据库查询出错, %s 错误原因: %s\n", sql.data(), mysql_error(&mysql));
        mysql_close(&mysql);
        return false;
    }

    printf("username: %s \n", user.username.c_str()); /* 打印 ID */

    /* 关闭数据库 */
    mysql_close(&mysql);

    return true;
}

bool ifrepeat(userinfo& user)
{
    MYSQL mysql;
    MYSQL_RES* res; /* 查询结果集 */
    MYSQL_ROW row; /* 记录结构体 */
    char sql[256];
    bool ret = false;

    /* 1. 连接到数据库 */
    if (connect_db(mysql) == false)
    {
        return false;
    }

    /* 2. 根据用户名和密码获取用户信息(id, leve_id) */
    snprintf(sql, 256,"select * from USR.usr where name='%s';",
             user.username.c_str());

    ret = mysql_query(&mysql, sql); /* 成功返回0 */

    if (ret)
    {
        printf("数据库查询出错, %s 错误原因: %s\n", sql, mysql_error(&mysql));
        mysql_close(&mysql);
        return false;
    }


    /* 3. 获取结果 */
    res = mysql_store_result(&mysql);
    row = mysql_fetch_row(res);

    if (row == nullptr) /* 没有查找到记录 */
    {
        mysql_free_result(res);
        mysql_close(&mysql);
        return true;
    }

    user.username = row[1];
    printf("username: %s \n", user.username.c_str()); /* 打印 ID */

    /* 释放结果集 */
    mysql_free_result(res);

    /* 关闭数据库 */
    mysql_close(&mysql);

    return false;
}
bool Register(userinfo& user)
{
    bool ret = false;

    do {
        cout << "请输入用户名: ";
        cin >> user.username;
        if (IsdigitAll(user.username))
        {
            ret = ifrepeat(user);
            if(!ret)
            {
                cout<<"帐号已存在！请重新输入"<<endl;
                continue;
            }
            cout << "请输入密码: ";
            cin >> user.passwd;

            /* 返回 bool, 成功返回 true, 失败返回 false */
            ret = insert_user_info(user); /* 读取用户信息 */
            if (ret == false)
            {
                cout << "注册失败, 请重新输入!" << endl;
            }
        }

    } while (!ret);

    return ret;
}
bool login(userinfo& user)
{
    int times = 0;
    bool ret = false;

    do {
        cout << "请输入用户名: ";
        cin >> user.username;
        if(IsdigitAll(user.username))
        {
            cout << "请输入密码: ";
            cin >> user.passwd;
            /* 返回 bool, 成功返回 true, 失败返回 false */
            ret = fetch_user_info(user); /* 读取用户信息 */
            times++;
            if (times >= MAX_RETRY_TIMES)
            {
                break;
            }

            if (ret == false)
            {
                cout << ", 请重新输入!" << endl;
            }
        }

    } while (!ret);

    return ret;
}
int main()
{

    /* 用户身份验证 */

    while (true) {
        userinfo user;
        cout<<"     (1)Register注册\n     (2)Enter登录\n     (3)Exit退出\n\nChoice:";
        int choice;//选择界面
        cin>>choice;
        switch (choice) {
            case 1://注册
                if (Register(user) == false) {
                    cout << "注册失败, 请重新注册!!!" << endl;
                    exit(-1);
                }
                break;
            case 2://登录
                if (login(user) == false) {
                    cout << "登录失败, 请重新登录!!!" << endl;
                    exit(-1);
                } else {
                    cout << "登录成功, 请开始你的表演!!!" << endl;
                }
                break;
            case 3://退出
                exit(EXIT_FAILURE);
        }
    }

    return 0;
}