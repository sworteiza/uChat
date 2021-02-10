#include "server.h"

t_db data;
int connfd = 0;
char *send_msg = NULL;
static int inUsr = 0;
t_del delete;
t_msg msg;
int listenfd = 0;
char *concat_msg(char *str1, char *str2)
{
    if (str1 == NULL)
        return str2;
    if (str2 == NULL)
        return str1;
    char *result = malloc(strlen(str1) + strlen(str2) + 2);
    strcpy(result, str1);
    strcat(result, " ");
    strcat(result, str2);
    return result;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

char *mx_strjoin(const char *s1, const char *s2)
{
    if (s1 == NULL && s2 == NULL)
        return NULL;
    else if (s1 == NULL)
        return strdup(s2);
    else if (s2 == NULL)
        return strdup(s1);
    else
    {
        int ns1 = strlen(s1), ns2 = strlen(s2);
        char *new = mx_strnew(ns1 + ns2);

        new = strcpy(new, s1);
        new = strcat(new, s2);
        return new;
    }
}

int key[4]={1,6,3,9};
char* locked(char *temp){
    int len=strlen(temp);
    char *result=NULL;
    int n=len+1;
    result=malloc(n*sizeof(char));
    int j=0;
    for(int i = 0;i<len;i++){
        result[i]=temp[i]+key[j];
        if(j<3)j++;
        else j=0;
    }
    result[len]='\0';
    return result;
}
char* unlocked(char *temp){
    int len=strlen(temp);
    char *result=NULL;
    int n=len+1;
    result=malloc(n*sizeof(char));
    int j=0;
    for(int i = 0;i<len;i++){
        result[i]=temp[i]-key[j];
        if(j<3)j++;
        else j=0;
    }
    result[len]='\0';
    return result;
}
static int c_log(void *NotUsed, int argc, char **argv, char **azColName)
{
    
    if (strcmp(argv[0], data.login) == 0)
    {
        if (strcmp(unlocked(argv[1]), data.pass) == 0)
            data.check = 1;
    }
    return 0;
}

void LogIn(char *Login, char *Pass)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    data.login = Login;
    data.pass = Pass;
    data.check = 0;
    rc = sqlite3_open("database.db", &db);
    sql = "SELECT * FROM USER";
    rc = sqlite3_exec(db, sql, c_log, 0, &zErrMsg);
   sqlite3_close(db);
}

static int search_log(void *NotUsed, int argc, char **argv, char **azColName)
{
    data.check = 1;
    return 0;
}

void Search(char *Login)
{
    sqlite3 *db;
    char *zErrMsg = 0;
   int rc;
    char *sql;
    data.login = Login;
    data.check = 0;
   rc = sqlite3_open("database.db", &db);
    char *log = NULL;
    log = mx_strjoin(log, "'");
    log = mx_strjoin(log, Login);
    log = mx_strjoin(log, "'");
    sql = "SELECT LOGIN FROM USER WHERE LOGIN = ";
    sql = mx_strjoin(sql, log);
   rc = sqlite3_exec(db, sql, search_log, 0, &zErrMsg);
    sqlite3_close(db);
}

char *mx_null(int n){
    char *tmp=0;
    for(int i=0;i<n;i++){
        tmp[i]='\0';
    }
    return tmp;
}
static int call_msg_return(void *data1, int argc, char **argv, char **azColName) {
    char *send_msg=0;
    data.count+=1;
    if(strcmp(data.login,argv[0])==0){
        send_msg = concat_msg("LOGIN::", argv[0]);
        send_msg = concat_msg(send_msg, "TEXTMSG::");
        send_msg = concat_msg(send_msg, argv[1]);
        send_msg = concat_msg(send_msg, "TIME::");
        send_msg = concat_msg(send_msg, argv[2]);
        send_msg = concat_msg(send_msg, "ID::");
        send_msg = concat_msg(send_msg, argv[3]);
        send_msg = concat_msg(send_msg, "NUM_MSG::");
        send_msg = concat_msg(send_msg, argv[4]);
        send_msg = concat_msg(send_msg, "\0");
        
            send(connfd, send_msg, strlen(send_msg), 0);
    }
   return 0;
}
void msg_return(char *user, char *name_usr)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    data.login = name_usr;
    data.count=0;
   rc = sqlite3_open("database.db", &db);
    sql = "SELECT * FROM ";
    sql = mx_strjoin(sql, user);
    rc = sqlite3_exec(db, sql, call_msg_return, 0, &zErrMsg);
    sqlite3_close(db);
   
}
void RegIn(char *Login, char *Pass, char *Name)
{
    sqlite3 *db;
    char *zErrMsg = 0;
   int rc;
    char *sql;
    rc = sqlite3_open("database.db", &db);
    char *tmp = NULL;
    char *log = NULL;
    char *pass = NULL;
    char *name = NULL;
    log = mx_strjoin(log, "'");
    log = mx_strjoin(log, Login);
    log = mx_strjoin(log, "'");
    pass = mx_strjoin(pass, "'");
    pass = mx_strjoin(pass, locked(Pass));
    pass = mx_strjoin(pass, "'");
    name = mx_strjoin(name, "'");
    name = mx_strjoin(name, Name);
    name = mx_strjoin(name, "'");
    tmp = mx_strjoin(tmp, " VALUES (");
    tmp = mx_strjoin(tmp, log);
    tmp = mx_strjoin(tmp, ", ");
    tmp = mx_strjoin(tmp, pass);
    tmp = mx_strjoin(tmp, ", ");
    tmp = mx_strjoin(tmp, name);
    tmp = mx_strjoin(tmp, " );");

    sql = "INSERT INTO USER (LOGIN, PASSWORD, NAME)";
    sql = mx_strjoin(sql, tmp);
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    sqlite3_close(db);
}
void Message(char *Login, char *Login_who, char *msg, char *time, char *ident,int Index_m)
{

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
   rc = sqlite3_open("database.db", &db);
    char *tmp = NULL;
    char *log = NULL;
    char *tim = NULL;
    char *log_who = NULL;
    char *message = NULL;
    char *id = NULL;
    char* Index=mx_itoa(Index_m);
    char *index__msg=NULL;
    log = mx_strjoin(log, "'");
    log = mx_strjoin(log, Login);
    log = mx_strjoin(log, "'");
    log_who = mx_strjoin(log_who, "'");
    log_who = mx_strjoin(log_who, Login_who);
    log_who = mx_strjoin(log_who, "'");
    message = mx_strjoin(message, "'");
    message = mx_strjoin(message, locked(msg));
    message = mx_strjoin(message, "'");
    tim = mx_strjoin(tim, "'");
    tim = mx_strjoin(tim, time);
    tim = mx_strjoin(tim, "'");
    id = mx_strjoin(id, "'");
    id = mx_strjoin(id, ident);
    id = mx_strjoin(id, "'");
    index__msg = mx_strjoin(index__msg, "'");
    index__msg = mx_strjoin(index__msg, Index);
    index__msg = mx_strjoin(index__msg, "'");
    tmp = mx_strjoin(tmp, " VALUES (");
    tmp = mx_strjoin(tmp, log_who);
    tmp = mx_strjoin(tmp, ", ");
    tmp = mx_strjoin(tmp, message);
    tmp = mx_strjoin(tmp, ", ");
    tmp = mx_strjoin(tmp, tim);
    tmp = mx_strjoin(tmp, ", ");
    tmp = mx_strjoin(tmp, id);
    tmp = mx_strjoin(tmp, ", ");
    tmp = mx_strjoin(tmp, index__msg);
    tmp = mx_strjoin(tmp, " );");

    sql = "INSERT INTO ";
    sql = mx_strjoin(sql, Login);
    sql = mx_strjoin(sql, " (LOGIN_WHOM, TEXTMSG, TIME, ID, INDEX_MSG)");
    sql = mx_strjoin(sql, tmp);
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  
   sqlite3_close(db);
}

char *CheckUser(char *name)
{
    sqlite3 *db;
   
    int rc1;
    char *sql;
    char *UserDB = NULL;
int rc;
   rc = sqlite3_open("database.db", &db);
    sql = "SELECT LOGIN_WHOM FROM";
    sql = concat_msg(sql, name);

    sqlite3_stmt *stmt;
    int flag = 0;

    rc1 = sqlite3_prepare(db, sql, -1, &stmt, NULL);

    if (rc1 != SQLITE_OK)
    {
        fprintf(stderr, "1");
        return 0;
    }

    while ((rc1 = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        char *a = strdup((char*)sqlite3_column_text(stmt, 0));
        if (UserDB == NULL)
        {
            UserDB = concat_msg(UserDB, a);
            inUsr++;
        }
        else if (strstr(UserDB, a) == 0)
        {
            UserDB = concat_msg(UserDB, a);
            inUsr++;
        }
        flag = 0;
    }
    if (rc1 == SQLITE_ROW)
        sqlite3_finalize(stmt);
    sqlite3_close(db);
    return UserDB;
}
void CreateDb()
{
    sqlite3 *db;
    char *zErrMsg = 0;
   int rc;
    char *sql;
   rc = sqlite3_open("database.db", &db);

    sql = "CREATE TABLE USER("
          "LOGIN TEXT NOT NULL,"
          "PASSWORD TEXT NOT NULL,"
          "NAME TEXT NOT NULL);";

     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
     sqlite3_close(db);
}
void CreateUserDB(char *argv)
{

     sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    rc = sqlite3_open("database.db", &db);

    sql = "CREATE TABLE ";
    sql = mx_strjoin(sql, argv);
    sql = mx_strjoin(sql, "(LOGIN_WHOM TEXT NOT NULL,");
    sql = mx_strjoin(sql, "TEXTMSG TEXT NOT NULL,");
    sql = mx_strjoin(sql, "TIME TEXT NOT NULL,");
    sql = mx_strjoin(sql, "ID TEXT NOT NULL,");
    sql = mx_strjoin(sql, "INDEX_MSG TEXT NOT NULL);");
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    sqlite3_close(db);
}
void del_msg(char *login, char *who, char *text,char *msg){
    char *zErrMsg = 0;
   int rc;
    char *sql;
    char *time=NULL;
    time = mx_strjoin(time, "'");
    time= mx_strjoin(time, msg);
    time = mx_strjoin(time, "'");
    char *txt=NULL;
    txt = mx_strjoin(txt, "'");
    txt= mx_strjoin(txt, text);
    txt = mx_strjoin(txt, "'");
    char *wh=NULL;
    wh = mx_strjoin(wh, "'");
    wh= mx_strjoin(wh, who);
    wh = mx_strjoin(wh, "'");
    sql = "DELETE FROM ";
    sql = mx_strjoin(sql, login);
    sql = mx_strjoin(sql, " WHERE LOGIN_WHOM = ");
    sql=mx_strjoin(sql,wh);
    sql=mx_strjoin(sql," AND TEXTMSG = ");
    sql=mx_strjoin(sql,txt);
    sql=mx_strjoin(sql," AND TIME = ");
    sql=mx_strjoin(sql,time);
  
    rc = sqlite3_exec(delete.db, sql, 0, 0, &zErrMsg);
    sqlite3_close(delete.db);
    
}

static int call_mess_delete(void *data, int argc, char **argv, char **azColName) {
    if(strcmp(delete.who,argv[0])==0){  
        if(delete.index==delete.iter){
           if (strcmp(argv[3], "TRUE") == 0){
               del_msg(delete.login,delete.who,argv[1],argv[2]);
               del_msg(delete.who,delete.login,argv[1],argv[2]);
            delete.flag=0;
           }else delete.flag=1;
       }
       delete.index++;
    }
   return 0;
}
int delete_mes(char *login, char *who, char *iter)
{
    int num = atoi(iter);
    sqlite3 *db;
     char *error=0;
     char *sql;
     int rc;
    rc = sqlite3_open("database.db",&db);
    sql = "SELECT * FROM ";
    sql = mx_strjoin(sql, login);
    delete.login=login;
    delete.who=who;
    delete.iter=num;
    delete.index=0;
    delete.flag=1;
    delete.db=db;
    rc = sqlite3_exec(db, sql, call_mess_delete, 0, &error);
    if(delete.flag==0)
    return 0;
    else return 1;
   
}
static int index_msg(void *data, int argc, char **argv, char **azColName){
    if(strcmp(msg.name_whom,argv[0])==0){
        msg.index++;
        
    }
    return 0;
}
int in_message(char *a,char *b){
    sqlite3 *db;
     char *error=0;
     char *sql;
     int rc;
    rc = sqlite3_open("database.db",&db);
    sql = "SELECT * FROM ";
    sql = mx_strjoin(sql, a);
    msg.name_who=a;
    msg.name_whom=b;
    msg.index=0;
    rc = sqlite3_exec(db, sql, index_msg, 0, &error);
    msg.index+=1;
    return msg.index;
}

void delete_message_index(char *login, char *who, char *iter)
{
    int num = atoi(iter);
    sqlite3 *db;
     char *error=0;
     char *sql;
     int rc;
    rc = sqlite3_open("database.db",&db);
  
     sqlite3_stmt *res;
     char *whom=NULL;
     char *lg=NULL;
     char *number=NULL;
     lg = mx_strjoin(lg, "'");
    lg= mx_strjoin(lg, login);
    lg = mx_strjoin(lg, "'");
    whom = mx_strjoin(whom, "'");
    whom= mx_strjoin(whom, who);
    whom = mx_strjoin(whom, "'");
    number= mx_strjoin(number, iter);
    sql = "SELECT * FROM ";
    sql = mx_strjoin(sql, login);
    sql=mx_strjoin(sql," WHERE LOGIN_WHOM = ");
    sql=mx_strjoin(sql,whom);
   
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
     while (sqlite3_step(res) != SQLITE_DONE) {
        char* value = (char*)sqlite3_column_text(res, 4);
        int val=atoi(value);
        if(val>num){
            num++;
            sql="UPDATE ";
            sql=mx_strjoin(sql,lg);
            sql=mx_strjoin(sql," SET ");
            sql=mx_strjoin(sql, "INDEX_MSG = ");
           number=NULL;
            number= mx_strjoin(number, mx_itoa(num));
            sql=mx_strjoin(sql,number);
            sql=mx_strjoin(sql, " WHERE INDEX_MSG = ");
            sql=mx_strjoin(sql,mx_itoa(val));
            rc = sqlite3_exec(db, sql, NULL, 0, &error);
        }
        
     }
    sqlite3_finalize(res);
    sqlite3_close(db);
}
void change_msg(char*login,char*name_who,char *iter,char*text_change,int id){
int num = atoi(iter)+1;
    sqlite3 *db;
     char *error=0;
     char *sql;
     int rc;
    rc = sqlite3_open("database.db",&db);
    sqlite3_stmt *res;
     char *whom=NULL;
     char *lg=NULL;
     char *change=NULL;
     lg = mx_strjoin(lg, "'");
    lg= mx_strjoin(lg, login);
    lg = mx_strjoin(lg, "'");
    whom = mx_strjoin(whom, "'");
    whom= mx_strjoin(whom, name_who);
    whom = mx_strjoin(whom, "'");
    change= mx_strjoin(change, "'");
    change= mx_strjoin(change, locked(text_change));
    change= mx_strjoin(change, "'");
    sql = "SELECT * FROM ";
    sql = mx_strjoin(sql, login);
    sql=mx_strjoin(sql," WHERE LOGIN_WHOM = ");
    sql=mx_strjoin(sql,whom);
   
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
     while (sqlite3_step(res) != SQLITE_DONE) {
        char* value = (char*)sqlite3_column_text(res, 4);
        char*flag=(char*)sqlite3_column_text(res, 3);
        int val=atoi(value);
        if(val==num ){
            if(id==0 && strcmp(flag,"TRUE")==0){
                sql="UPDATE ";
                sql=mx_strjoin(sql,lg);
                sql=mx_strjoin(sql," SET ");
                sql=mx_strjoin(sql, "TEXTMSG= ");
                sql=mx_strjoin(sql,change);
                sql=mx_strjoin(sql, " WHERE INDEX_MSG = ");
                sql=mx_strjoin(sql,mx_itoa(val));
                rc = sqlite3_exec(db, sql, NULL, 0, &error);
            }
            else if (id==1){
                sql="UPDATE ";
                sql=mx_strjoin(sql,lg);
                sql=mx_strjoin(sql," SET ");
                sql=mx_strjoin(sql, "TEXTMSG= ");
                sql=mx_strjoin(sql,change);
                sql=mx_strjoin(sql, " WHERE INDEX_MSG = ");
                sql=mx_strjoin(sql,mx_itoa(val));
                rc = sqlite3_exec(db, sql, NULL, 0, &error);
            }
        }
        
     }
    sqlite3_finalize(res);
    sqlite3_close(db);
}
static int call_msg_return_pth(void *data1, int argc, char **argv, char **azColName) {
    char *send_msg=0;
    if(atoi(argv[4])==data.count+1){
        if(strcmp(data.login,argv[0])==0){
            send_msg = concat_msg("LOGIN::", argv[0]);
            send_msg = concat_msg(send_msg, "TEXTMSG::");
            send_msg = concat_msg(send_msg, unlocked(argv[1]));
            send_msg = concat_msg(send_msg, "TIME::");
            send_msg = concat_msg(send_msg, argv[2]);
            send_msg = concat_msg(send_msg, "ID::");
            send_msg = concat_msg(send_msg, argv[3]);
            send_msg = concat_msg(send_msg, "NUM_MSG::");
            send_msg = concat_msg(send_msg, argv[4]);
            send_msg = concat_msg(send_msg, "\0");
            send(connfd, send_msg, strlen(send_msg), 0);
            data.count+=1;
        }
    }
   return 0;
}
void msg_return_pth(char *user, char *name_usr,char *index)
{
    int num=atoi(index);
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;
    data.login = name_usr;
    data.count=num;
   rc = sqlite3_open("database.db", &db);
    sql = "SELECT * FROM ";
    sql = mx_strjoin(sql, user);
    rc = sqlite3_exec(db, sql, call_msg_return_pth, 0, &zErrMsg);
    sqlite3_close(db);
}
void *doWork()
{
    int n = 0;
    char sendBuff[1025];
    memset(sendBuff, '\0', sizeof(sendBuff));
    n = read(connfd, sendBuff, sizeof(sendBuff) - 1);
    char **arr = mx_strsplit(sendBuff, ' ');
    if (strcmp(arr[0], "RegIn") == 0)
    {
        RegIn(arr[1], arr[2], arr[3]);
        write(connfd, "True", strlen("True"));
        data.check = 1;
        CreateUserDB(arr[1]);
    }
    else if (strcmp(arr[0], "SingIn") == 0)
    {
        LogIn(arr[1], arr[2]);
        if (data.check == 1)
        {
            write(connfd, "True", strlen("True"));
        }
        else
        {
            write(connfd, "False", strlen("False"));
        }
    }
    else if (strcmp(arr[0], "Mes") == 0)
    {
        char *message_send = NULL;
        int i = 0;
        while (arr[i] != NULL)
        {
            if (i >= 3)
            {
                message_send = concat_msg(message_send, arr[i]);
            }
            i++;
        }
        
        long int msg_time;
        msg_time = time(NULL);
        char *tm = ctime(&msg_time);
        int k = 1;
        char *time_m = NULL, **Arr = NULL;
        Arr = mx_strsplit(tm, ' ');
        while (k < 4)
        {
            time_m = concat_msg(time_m, Arr[k]);
            k++;
        }
        int Index_mess=in_message(arr[1],arr[2]);
        Message(arr[1], arr[2], message_send, time_m, "TRUE",Index_mess);
        Message(arr[2], arr[1], message_send, time_m, "FALSE",Index_mess);
        free(time_m);
    }
    else if (strcmp(arr[0], "ReturnMes") == 0)
    {
        msg_return(arr[1], arr[2]);
    }
    else if (strcmp(arr[0], "Search") == 0)
    {
        Search(arr[1]);
        if (data.check == 1)
        {
            write(connfd, "1", strlen("1"));
        }
        else
        {
            write(connfd, "0", strlen("0"));
        }
    }
    else if (strcmp(arr[0], "Chats") == 0)
    {
        char *Usrs = NULL;
        Usrs = CheckUser(arr[1]);
        if (Usrs != NULL)
        {
            write(connfd, Usrs, strlen(Usrs));
            memset(Usrs, '\0', strlen(Usrs));
        }
        else
            write(connfd, "FALSE", strlen("FALSE"));
    }
    else if (strcmp(arr[0], "DELETEMes") == 0)
    {
        if (delete_mes(arr[1], arr[2], arr[3]) == 0)
        {   
            delete_message_index(arr[1], arr[2], arr[3]);
            delete_message_index(arr[2], arr[1], arr[3]);
            write(connfd, "TRUE", strlen("TRUE"));
        }
        else
            write(connfd, "FALSE", strlen("FALSE"));
    }
    else if (strcmp(arr[0], "CheckMsg") == 0)
    {
        int Index_mess=in_message(arr[1],arr[2])-1;
        int number_msg=atoi(arr[3]);
        if (Index_mess==number_msg)
        {
            write(connfd, "TRUE", strlen("TRUE"));
        }
        else
            write(connfd, "FALSE", strlen("FALSE"));
    }
    else if (strcmp(arr[0], "ChangeMes") == 0)
    {
        change_msg(arr[1],arr[2],arr[3],arr[4],0);
        change_msg(arr[2],arr[1],arr[3],arr[4],1);
    }
    else if (strcmp(arr[0], "ReturnMesPTH") == 0)
    {
       msg_return_pth(arr[1], arr[2],arr[3]);
        
    }
    memset(sendBuff, '\0', strlen(sendBuff));
    write(connfd, "\0", strlen(sendBuff));
    close(connfd);
    return 0;
}
void close_lis(){
    close(listenfd);
}
int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    pthread_t pth;
    daemon(1, 1);
    CreateDb();
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        write(2, "No socket", 9);
    }
    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

    const int fl = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &fl, sizeof(int)) < 0)
    {
        write(2, "error2", 5);
    }


    if(bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        exit(errno);
    }
    listen(listenfd, 10);
    
    while ((connfd = accept(listenfd, (struct sockaddr *)NULL, NULL)))
    {
        pthread_create(&pth, NULL, doWork, NULL);

        sleep(1);
    }
    signal(SIGINT,&close_lis);
    return 0;
}
