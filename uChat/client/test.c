#include "./inc/client.h"

typedef struct s_list
{
    GtkWidget *Label;
    GtkWidget *Box;
    struct s_list *next;
} t_list;

enum
{
    LIST_ITEM = 0,
    N_COLUMNS
};
void get_msg_PTH(gpointer name);
void remove_all(GtkWidget *widget, gpointer selection);
void Profile(GtkButton *butt, gpointer data);
void MainD(GtkButton *button, gpointer data);
void CheckSingIn(GtkButton *button, gpointer data);
void MainMenu();
GtkWidget *window = NULL;
GtkWidget *Login, *Password;
GtkWidget *Name, *PasswordRep;
GtkWidget *message;
GtkWidget *Entry;
GtkWidget **Pointer = NULL;
GtkTreeSelection *SEll;
GtkListStore *store;
GtkTreeIter iter;
GtkWidget *Sv;
pthread_t pth;
GtkWidget *list;

char *login_en, *pass_en, *name_en;
char lg[1024];
int sockfd = 0, n = 0, count_main = 0,count_msg=0;
char recvBuff[4096];
char res[2048];
char **ARGV, **arr, *name_who;
struct sockaddr_in serv_addr;
volatile sig_atomic_t flag = 0;
int FLAG_PTH=0;
char *mx_itoa(int number);
void init_list(GtkWidget *list)
{

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;

    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes("List Item",
                                                      renderer, "text", LIST_ITEM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));

    g_object_unref(store);
}

int mx_count_bukwi(const char *s, char c)
{
    int i = 0;

    while (s[i] != c && s[i])
        i++;
    return i;
}

int mx_count_words(const char *str, char c)
{
    bool state = true;
    unsigned wordcount = 0;
    int i = 0;

    if (str == NULL)
        return -1;
    else
    {
        while (str[i])
        {
            if (str[i] == c)
                state = true;
            else if (state == true)
            {
                state = false;
                ++wordcount;
            }
            ++i;
        }
        return wordcount;
    }
}

char *mx_strncpy(char *dst, const char *src, int len)
{
    for (int i = 0; i < len && src[i] != '\0'; i++)
    {
        dst[i] = src[i];
    }
    return dst;
}

char *mx_strnew(const int size)
{
    if (size < 0)
    {
        return 0;
    }
    char *a = malloc(size + 1);
    for (int i = 0; i <= size; i++)
    {
        a[i] = '\0';
    }
    return a;
}

char *mx_strndup(const char *str, size_t n)
{
    char *a = mx_strnew(n);
    mx_strncpy(a, str, n);
    return a;
}

char **mx_strsplit(const char *s, char c)
{
    int word_length = 0;
    int i = 0;
    char **arr = NULL;

    if (!s)
        return NULL;
    arr = (char **)malloc((mx_count_words(s, c) + 1) * sizeof(char *));
    while ((*s) && (*s != '\0'))
    {
        if (*s != c)
        {
            word_length = mx_count_bukwi(s, c);
            arr[i] = mx_strndup(s, word_length);
            s += word_length;
            i++;
            continue;
        }
        s++;
    }
    arr[i] = NULL;
    return arr;
}

void catch_ctrl_c_and_exit(int sig)
{
    flag = 1;
}

void SingIn(GtkButton *button, gpointer data);
void Warning();
void get_msg(gpointer name);
char *concat(char *str1,char *str2)
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
char *concat_time(char *str1, char *str2)
{
    if (str1 == NULL)
        return str2;
    if (str2 == NULL)
        return str1;
    char *result = malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}
void ChatWin(GtkButton *butt, gpointer data);

void remove_item(GtkWidget *widget, gpointer selection)
{

    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter iter;
    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        
    }

    ///

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

    if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
    {
        return;
    }

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection),
                                        &model, &iter))
    {
        gchar *name = gtk_tree_model_get_string_from_iter(model, &iter);
        char *Mes = concat("DELETEMes", lg);
        Mes = concat(Mes, name_who);
        Mes = concat(Mes, name);

        write(sockfd, Mes, strlen(Mes));
        while (recv(sockfd, recvBuff, 4096, 0) > 0)
        {
            if (strcmp(recvBuff, "FALSE"))
            {
                gtk_list_store_remove(store, &iter);
            }
        }
        count_msg=0;
        remove_all(NULL, (gpointer)SEll);
        get_msg_PTH(name_who);
        while (gtk_events_pending()) {
        gtk_main_iteration();
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(Sv));
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
        gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(Sv), adj);
    }
    }
}

void change_item(GtkWidget *widget, gpointer selection)
{

    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter iter;
    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        
    }

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

    if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
    {
        return;
    }

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection),
                                        &model, &iter))
    {   
        char *Messag = (char *)gtk_entry_get_text(GTK_ENTRY(message));
        if (strcmp(Messag,"")==0)
        printf("%s\n",Messag);
        if(strcmp(Messag,"")!=0){
        gchar *name = gtk_tree_model_get_string_from_iter(model, &iter);
        char *Mes = concat("ChangeMes", lg);
        Mes = concat(Mes, name_who);
        Mes = concat(Mes, name);
        Mes = concat(Mes,Messag);

        write(sockfd, Mes, strlen(Mes));
        read(sockfd, recvBuff, sizeof(recvBuff) - 1);
        remove_all(NULL, (gpointer)SEll);
        count_msg=0;
        get_msg_PTH(name_who);
       }
        gtk_entry_set_text((GtkEntry*)message, "");
        while (gtk_events_pending()) {
        gtk_main_iteration();
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(Sv));
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
        gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(Sv), adj);
    }
    }
}
void remove_all(GtkWidget *widget, gpointer selection)
{

    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter iter;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

    if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
    {
        return;
    }

    gtk_list_store_clear(store);
}
int mx_recvBuff(char recvBuff[4096]){
    for(int i=0;i<4096;i++){
        if(recvBuff[i]=='\0'){
            return 1;
            
        }
    }
    return 0;
}
void get_msg(gpointer name)
{
    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
    }
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    char *Mes = concat("ReturnMes", lg);
    Mes = concat(Mes, name);

    remove_all(NULL, (gpointer)SEll);
    write(sockfd, Mes, strlen(Mes));
    int count_w;
    char *mes_ar = NULL, *message_array = NULL;
    int i;
    int Index=1;
   while ((recv(sockfd, recvBuff, 4096, 0)) > 0)
    {
        char **arr = mx_strsplit(recvBuff, ' ');
        count_w = mx_count_words(recvBuff, ' ');
         i = 0;
         while (i < count_w)
        {
            if (strcmp(arr[i], "LOGIN::") == 0)
            {
                i++;
                if (strcmp(arr[i], "TEXTMSG::") != 0)
                    message_array = concat(message_array, arr[i]);
            }
            if (strcmp(arr[i], "TEXTMSG::") == 0)
            {
                i++;
                while (strcmp(arr[i], "TIME::") != 0)
                {
                    mes_ar = concat(mes_ar, arr[i]);
                    i++;
                }
                if (strcmp(arr[i], "TIME::") == 0)
                {
                    i++;
                    mes_ar = concat(mes_ar, "[");
                    while (strcmp(arr[i], "ID::") != 0)
                    {
                        mes_ar = concat(mes_ar, arr[i]);
                        i++;
                    }

                    mes_ar = concat(mes_ar, "]");
                }

                if (strcmp(arr[i], "ID::") == 0)
                {
                    i++;
                    while(strcmp(arr[i],"NUM_MSG::")!=0){
                        if (strcmp(arr[i], "TRUE") == 0)
                        {
                            message_array = NULL;
                            message_array = concat("I:   ", mes_ar);
                        }
                        else
                        {
                            message_array = NULL;
                            message_array = concat(arr[1], ":   ");
                            message_array = concat(message_array, mes_ar);
                        }
                       i++;
                    }
                }
                if(strcmp(arr[i],"NUM_MSG::")==0){
                    i++;
                    int count=atoi(arr[i]);
                    if(count==Index){
                        gtk_list_store_append(store, &iter);
                        gtk_list_store_set(store, &iter, 0, message_array, -1);
                        
                        mes_ar = NULL;
                        message_array = NULL;
                        Index++;
                    }
                    i++;
                    
                   count_msg=count; 
                }
                   
            }    mes_ar = NULL;

            i++;
       }
    }
    FLAG_PTH=1;
}
void get_msg_PTH(gpointer name){
    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
    }
    GtkTreeModel *model;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    char *Mes = concat("ReturnMesPTH", lg);
    Mes = concat(Mes, name);
    char *aa=mx_itoa(count_msg);
    Mes=concat(Mes,aa);
    write(sockfd, Mes, strlen(Mes));
    int count_w=0;
    char *mes_ar = NULL, *message_array = NULL;
    int i=0;
    while ((recv(sockfd, recvBuff, 4096, 0)) > 0)
    {   

        char **arr = mx_strsplit(recvBuff, ' ');
        count_w = mx_count_words(recvBuff, ' ');
         i = 0;
         while (i < count_w)
        {
            if (strcmp(arr[i], "LOGIN::") == 0)
            {
                i++;
                if (strcmp(arr[i], "TEXTMSG::") != 0)
                    message_array = concat(message_array, arr[i]);
            }
            if (strcmp(arr[i], "TEXTMSG::") == 0)
            {
                i++;
                while (strcmp(arr[i], "TIME::") != 0)
                {
                    mes_ar = concat(mes_ar, arr[i]);
                    i++;
                }
                if (strcmp(arr[i], "TIME::") == 0)
                {
                    i++;
                    mes_ar = concat(mes_ar, "[");
                    while (strcmp(arr[i], "ID::") != 0)
                    {
                        mes_ar = concat(mes_ar, arr[i]);
                        i++;
                    }

                    mes_ar = concat(mes_ar, "]");
                }

                if (strcmp(arr[i], "ID::") == 0)
                {
                    i++;
                    while(strcmp(arr[i],"NUM_MSG::")!=0){
                        if (strcmp(arr[i], "TRUE") == 0)
                        {
                            message_array = NULL;
                            message_array = concat("I:   ", mes_ar);
                            
                        }
                        else
                        {
                            message_array = NULL;
                            message_array = concat(arr[1], ":   ");
                            message_array = concat(message_array, mes_ar);
                        }
                       i++;
                    }
                }
                if(strcmp(arr[i],"NUM_MSG::")==0){
                    i++;
                    int count=atoi(arr[i]);
                     if(count==count_msg+1){
                        
                        gtk_list_store_append(store, &iter);
                        gtk_list_store_set(store, &iter, 0, message_array, -1);
                        
                        mes_ar = NULL;
                        message_array = NULL;
                        count_msg=count;
                     }
                    i++;
                    
                }
                   mes_ar = NULL;
            }    

            i++;
       }
    }
   
}
void send_msg_handler(GtkWidget *Button, gpointer data)
{
    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
    }

    char *Mes = (char *)gtk_entry_get_text(GTK_ENTRY(message));
    

    if (strcmp(Mes, "") != 0)
    {
        char *sendS = concat("Mes", lg);
        sendS = concat(sendS, name_who);
        sendS = concat(sendS, Mes);
        write(sockfd, sendS, strlen(sendS));
    }
    GtkTreeModel *model;
    char *message_array=NULL;
    long int msg_time;
    msg_time = time(NULL);
    char *tm = ctime(&msg_time);
    char *time_m = NULL, **Arr = NULL;
    int k=1;
        Arr = mx_strsplit(tm, ' ');
        while (k < 4)
        {
            time_m = concat(time_m, Arr[k]);
            k++;
        }
    message_array=concat(message_array,"I:  ");
    message_array=concat(message_array,Mes);
    message_array=concat(message_array,"[");
    message_array=concat(message_array,time_m);
    message_array=concat(message_array,"]");
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, message_array, -1);
    count_msg++;
    gtk_entry_set_text((GtkEntry*)message, "");
    while (gtk_events_pending()) {
        gtk_main_iteration();
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(Sv));
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
        gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(Sv), adj);
    }
}

void Return(GtkButton *button, gpointer data)
{
    SingIn(button, (GtkWidget *)data);
}

void CheckReg(GtkButton *button, gpointer data)
{
    login_en = strdup((char *)gtk_entry_get_text(GTK_ENTRY(Login)));
    pass_en = strdup((char *)gtk_entry_get_text(GTK_ENTRY(Password)));
    char *c = (char *)gtk_entry_get_text(GTK_ENTRY(PasswordRep));
    name_en = strdup((char *)gtk_entry_get_text(GTK_ENTRY(Name)));

    if (strcmp(login_en, "") != 0 && strcmp(pass_en, "") != 0 && strcmp(c, "") != 0 && strcmp(name_en, "") != 0)
    {
        memset(recvBuff, '\0', sizeof(recvBuff));
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Error : Could not create socket \n");
            return;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8888);

        if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
        {
            printf("\n inet_pton error occured\n");
            return;
        }

        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\n Error : Connect Failed \n");
        }

        if (strcmp(c, pass_en) == 0)
        {
            char *sendS = concat("RegIn", login_en);
            sendS = concat(sendS, pass_en);
            sendS = concat(sendS, name_en);

            write(sockfd, sendS, strlen(sendS));

            n = read(sockfd, recvBuff, sizeof(recvBuff) - 1);
            recvBuff[n] = 0;

            if (n < 0)
                printf("\n Read error \n");

            if (isdigit(login_en[0]) == 0)
            {
                if (strcmp(login_en, "") != 0)
                {
                    memset(lg, '\0', sizeof(lg));
                    for (int i = 0; login_en[i] != '\0'; i++)
                    {
                        lg[i] = login_en[i];
                    }
                    count_main = 4;
                    free(sendS);
                    MainD(NULL, data);
                }
            }
            else
            {
                Warning("1");
            }
        }
        else
        {
            Warning("0");
        }
    }
    else
    {
        Warning("0");
    }
    return;
}

void Registration(GtkButton *button, gpointer data)
{

    GtkWidget *label;
    GtkWidget *SingIn, *ExitR;
    GtkWidget *vbox, *vbox1, *hbox, *hbox1, *hbox2, *hbox3, *Ibox;
    GtkWidget *box;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(box, "BoxR");

    label = gtk_label_new("uChat");
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(label, "ChatR");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, FALSE, 0);
    gtk_widget_set_size_request(label, 158, 55);

    SingIn = gtk_button_new_with_label("Sign Up");
    gtk_widget_set_name(SingIn, "SignR");
    vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), SingIn, TRUE, FALSE, 0);
    gtk_widget_set_size_request(SingIn, 328, 80);

    Name = gtk_entry_new();
    gtk_widget_set_name(Name, "NameR");
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), Name, TRUE, FALSE, 0);
    gtk_widget_set_size_request(Name, 328, 80);
    gtk_entry_set_placeholder_text(GTK_ENTRY(Name), "enter name");

    Password = gtk_entry_new();
    gtk_widget_set_name(Password, "PassR");
    vbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox1), Password, TRUE, FALSE, 0);
    gtk_widget_set_size_request(Password, 328, 80);
    gtk_entry_set_placeholder_text(GTK_ENTRY(Password), "enter password");
    gtk_entry_set_visibility((GtkEntry*)Password, FALSE);

    PasswordRep = gtk_entry_new();
    gtk_widget_set_name(PasswordRep, "RepeatR");
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), PasswordRep, TRUE, FALSE, 0);
    gtk_widget_set_size_request(PasswordRep, 328, 80);
    gtk_entry_set_placeholder_text(GTK_ENTRY(PasswordRep), "repeat password");
    gtk_entry_set_visibility((GtkEntry*)PasswordRep, FALSE);

    Login = gtk_entry_new();
    gtk_widget_set_name(Login, "LoginR");
    hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox3), Login, TRUE, FALSE, 0);
    gtk_widget_set_size_request(Login, 328, 80);
    gtk_entry_set_placeholder_text(GTK_ENTRY(Login), "enter login");

    ExitR = gtk_button_new();
    gtk_widget_set_name(ExitR, "ExitR");
    Ibox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_size_request(ExitR, 30, 30);
    gtk_box_pack_end(GTK_BOX(Ibox), ExitR, TRUE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box), Ibox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hbox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hbox3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), vbox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hbox2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), vbox, FALSE, FALSE, 0);

    gtk_container_remove(GTK_CONTAINER(window), (GtkWidget *)data);
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "destroy",
                     G_CALLBACK(gtk_main_quit), box);
    g_signal_connect(GTK_BUTTON(ExitR), "clicked", G_CALLBACK(Return),
                     box);
    g_signal_connect(GTK_BUTTON(SingIn), "clicked", G_CALLBACK(CheckReg),
                     box);
    return;

}

void StyleStandart(void)
{
    GtkCssProvider *Style;
    GdkScreen *screen;
    GdkDisplay *display;

    Style = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(Style), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    const gchar *path = "./client/StyleStandart.css";
    GError *error = 0;

    gtk_css_provider_load_from_path(Style, path, &error);
    g_object_unref(Style);
}

void StyleLight(void)
{
    GtkCssProvider *Style;
    GdkScreen *screen;
    GdkDisplay *display;

    Style = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(Style), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    const gchar *path = "./client/StyleLight.css";
    GError *error = 0;

    gtk_css_provider_load_from_path(Style, path, &error);
    g_object_unref(Style);
}

void StyleDark(void)
{
    GtkCssProvider *Style;
    GdkScreen *screen;
    GdkDisplay *display;

    Style = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(Style), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    const gchar *path = "./client/StyleDark.css";
    GError *error = 0;

    gtk_css_provider_load_from_path(Style, path, &error);
    g_object_unref(Style);
}

char *mx_strcpy(char *dst, const char *src) {
    int i = 0;
    while (src[i]) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
    return dst;
}
static void mx_swap_char(char *s1, char *s2) {
    char s3 = *s1;
    *s1 = *s2;
    *s2 = s3;
}
void mx_str_reverse(char *s) {
    int len = strlen(s) - 1;
    
    if (s)
        for (int i = 0; i <= len / 2; i++)
            mx_swap_char(&s[i], &s[len - i]);
}
static int number_length(int number) {
	int length = 0;

	while (number) {
		number /= 10;
		length++;
	}
	return length;
}

char *mx_itoa(int number) {
	int length = number_length(number);
	int tmp = number;
	char *result = NULL;

	result = mx_strnew(length);
	if (number == 0)
		return mx_strcpy(result, "0");
	if (number == -2147483648)
		return mx_strcpy(result, "-2147483648");
	tmp = number;
	for (int i = 0; i < length; i++) {
		if (tmp < 0) {
			result[length] = '-';
			tmp = -tmp;
		}
		result[i] = (tmp % 10) + '0';
		tmp /= 10;
	}
	mx_str_reverse(result);
	return result;
}
void *Pth_msg (){
    
    while(1)
    {   
        memset(recvBuff, '\0', sizeof(recvBuff));
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Error : Could not create socket \n");
            return (void*)1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8888);

        if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
        {
            printf("\n inet_pton error occured\n");
            return (void*)1;
        }

        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\n Error : Connect Failed \n");
        }
            //sleep(5);
            char *Mes = concat("CheckMsg", lg);
            Mes = concat(Mes, name_who);
            char *msg=mx_itoa(count_msg);
            Mes = concat(Mes, msg);
            send(sockfd, Mes, strlen(Mes), 0);
            recv(sockfd, recvBuff, 4096, 0);
            

                if(strcmp(recvBuff, "FALSE") == 0){
                    //if(FLAG_PTH==1`)
                    get_msg_PTH(name_who);
                   
                    memset(recvBuff, '\0', 4096);
                }
                else {
                   sleep(3); 
                }
            
        
            // }
   }
    return 0;
}

void ChatWin(GtkButton *butt, gpointer data)
{
    GtkWidget *sendbutton, *ExitButt, *Delete, *Change;
    GtkWidget *Sobbes;
    GtkWidget *bhbox, *Svhbox, *EBhbox;
    GtkWidget *vbox;
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    Delete = gtk_button_new();
    gtk_widget_set_name(Delete, "DeleteMsg");
    gtk_widget_set_size_request(Delete, 50, 50);
    bhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_end(GTK_BOX(bhbox), Delete, TRUE, FALSE, 0);

    Change = gtk_button_new();
    gtk_widget_set_name(Change, "ChangeMsg");
    gtk_widget_set_size_request(Change, 50, 50);
    gtk_box_pack_end(GTK_BOX(bhbox), Change, TRUE, FALSE, 0);

    sendbutton = gtk_button_new();
    gtk_widget_set_name(sendbutton, "SButt");
    gtk_widget_set_size_request(sendbutton, 50, 50);
    gtk_box_pack_end(GTK_BOX(bhbox), sendbutton, FALSE, FALSE, 0);
    Sobbes = gtk_label_new(gtk_button_get_label(butt));

    message = gtk_entry_new();
    gtk_widget_set_name(message, "Message");
    gtk_entry_set_placeholder_text(GTK_ENTRY(message), "print");
    gtk_widget_set_size_request(message, 300, 50);
    gtk_box_pack_end(GTK_BOX(bhbox), message, FALSE, FALSE, 0);

    gtk_widget_set_name(Sobbes, "Sobbes");
    gtk_widget_set_size_request(Sobbes, 50, 50);
    EBhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(EBhbox), Sobbes, FALSE, FALSE, 0);

    ExitButt = gtk_button_new();
    gtk_widget_set_name(ExitButt, "EButt");
    gtk_widget_set_size_request(ExitButt, 30, 30);
    gtk_widget_set_name(EBhbox, "EBHB");
    gtk_box_pack_end(GTK_BOX(EBhbox), ExitButt, FALSE, FALSE, 0);
    
    Sv = gtk_scrolled_window_new(NULL, NULL);
    list = gtk_tree_view_new();
    

    gtk_widget_set_name(list, "ScrW");

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(Sv), GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(Sv), GTK_SHADOW_ETCHED_IN);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);

    Svhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_pack_start(GTK_BOX(Svhbox), Sv, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), EBhbox, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), bhbox, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), Svhbox, TRUE, TRUE, 0);
    FLAG_PTH=0;
    count_msg=0;
    if (data != NULL)
        gtk_container_remove(GTK_CONTAINER(window), (GtkWidget *)data);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    init_list(list);
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    
    char *a = (char*)gtk_button_get_label(GTK_BUTTON(butt));

    name_who = strdup(a);
    SEll = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));

    get_msg_PTH(name_who);

    gtk_container_add(GTK_CONTAINER(Sv), list);

    count_main=0;
    gtk_widget_show_all(window);
    
    pthread_create(&pth, NULL, Pth_msg, NULL);
    pthread_detach(pth); 
     
    while (gtk_events_pending()) {
        gtk_main_iteration();
        GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(Sv));
        gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));
        gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(Sv), adj);
    }
    g_signal_connect(GTK_BUTTON(ExitButt), "clicked", G_CALLBACK(MainD), vbox);
    g_signal_connect(GTK_BUTTON(sendbutton), "clicked", G_CALLBACK(send_msg_handler), (gpointer)butt);
    g_signal_connect(GTK_BUTTON(Delete), "clicked", G_CALLBACK(remove_item), (gpointer)SEll);
    g_signal_connect(GTK_BUTTON(Change), "clicked", G_CALLBACK(change_item), (gpointer)SEll);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
}

void SearchDB(GtkButton *butt, gpointer data)
{
    char *ToSearch = (char *)gtk_entry_get_text(GTK_ENTRY(Entry));

    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if(inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
    }

    GtkWidget *Button = gtk_button_new_with_label(ToSearch);

    ToSearch = concat("Search", ToSearch);
    send(sockfd, ToSearch, strlen(ToSearch), 0);
    // count_main = 0;
    char get;
    read(sockfd, &get, sizeof(get));
    if (get == '1')
    {
        count_main = 0;
        ChatWin((GtkButton*)Button, data);
    }
    else
    {
        Warning("3");
    }
    gtk_widget_destroy(Button);
}

void StandartTh(GtkButton *butt, gpointer data)
{
    StyleStandart();
}

void LightTh(GtkButton *butt, gpointer data)
{
    StyleLight();
}

void DarkTh(GtkButton *butt, gpointer data)
{
    StyleDark();
}

void Profile(GtkButton *butt, gpointer data)
{
    GtkWidget *Exit, *ExitAcc;
    GtkWidget *Ehbox, *Thbox, *EAhbox;
    GtkWidget *vbox, *sbox, *tbox;
    GtkWidget *Sbutton;
    GtkWidget *Bhbox;
    GtkWidget *Main, *Dark, *Light;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    Exit = gtk_button_new();
    gtk_widget_set_name(Exit, "EButt");
    gtk_widget_set_size_request(Exit, 30, 30);
    Ehbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(Ehbox, "EBHB2");
    gtk_box_pack_end(GTK_BOX(Ehbox), Exit, FALSE, FALSE, 0);

    sbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    Sbutton = gtk_button_new();
    gtk_widget_set_size_request(Sbutton, 45, 45);
    gtk_widget_set_name(Sbutton, "SerButton");
    Bhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(Bhbox, "Box");
    gtk_widget_set_size_request(Bhbox, 250, 55);
    gtk_box_pack_start(GTK_BOX(Bhbox), Sbutton, FALSE, FALSE, 0);

    Entry = gtk_entry_new();
    gtk_widget_set_size_request(Entry, 250, 55);
    gtk_widget_set_name(Entry, "SerEntry");
    gtk_box_pack_start(GTK_BOX(Bhbox), Entry, TRUE, TRUE, 0);
    gtk_entry_set_placeholder_text(GTK_ENTRY(Entry), "create chat");
    gtk_box_pack_start(GTK_BOX(sbox), Bhbox, FALSE, FALSE, 0);

    tbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    Main = gtk_button_new_with_label(NULL);
    gtk_widget_set_name(Main, "Theme1");
    gtk_widget_set_size_request(Main, 50, 50);
    Thbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(Thbox, "Tbox");
    gtk_box_pack_end(GTK_BOX(Thbox), Main, FALSE, FALSE, 15);

    Light = gtk_button_new_with_label(NULL);
    gtk_widget_set_name(Light, "Theme2");
    gtk_widget_set_size_request(Light, 50, 50);
    gtk_box_pack_end(GTK_BOX(Thbox), Light, FALSE, FALSE, 15);

    Dark = gtk_button_new_with_label(NULL);
    gtk_widget_set_name(Dark, "Theme3");
    gtk_widget_set_size_request(Dark, 50, 50);
    gtk_box_pack_end(GTK_BOX(Thbox), Dark, FALSE, FALSE, 15);

    gtk_box_pack_start(GTK_BOX(tbox), Thbox, FALSE, FALSE, 0);

    ExitAcc = gtk_button_new_with_label("Sign Out");
    gtk_widget_set_name(ExitAcc, "SearchButt");
    gtk_widget_set_size_request(ExitAcc, 415, 90);
    EAhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_end(GTK_BOX(EAhbox), ExitAcc, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), Ehbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), sbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), tbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), EAhbox, FALSE, FALSE, 0);

    if (data != NULL)
        gtk_container_remove(GTK_CONTAINER(window), (GtkWidget *)data);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    count_main=0;
    gtk_widget_show_all(window);
    
    g_signal_connect(GTK_BUTTON(Light), "clicked", G_CALLBACK(LightTh), (gpointer)(vbox));
    g_signal_connect(GTK_BUTTON(Dark), "clicked", G_CALLBACK(DarkTh), (gpointer)(vbox));
    g_signal_connect(GTK_BUTTON(Main), "clicked", G_CALLBACK(StandartTh), (gpointer)(vbox));
    g_signal_connect(GTK_BUTTON(Sbutton), "clicked", G_CALLBACK(SearchDB), (gpointer)(vbox));
    g_signal_connect(GTK_BUTTON(ExitAcc), "clicked", G_CALLBACK(SingIn), (gpointer)(vbox));
    g_signal_connect(GTK_BUTTON(Exit), "clicked", G_CALLBACK(MainD), (gpointer)(vbox));
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

}

void MainMenu()
{
    memset(recvBuff, '\0', sizeof(recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);

    if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        return;
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        return;
    }
    char *str = concat("Chats", login_en);

    send(sockfd, str, strlen(str), 0);
    read(sockfd, res, sizeof(res) - 1);

    if (strcmp(res, "FALSE") == 0)
        count_main = 4;
}

void MainD(GtkButton *button, gpointer data)
{
    GtkWidget *ScrWin;
    GtkWidget *SWhorBox, *SWverbox;
    GtkWidget *Profbutton, *NameLab;
    GtkWidget *Phbox, *Chhbox;
    GtkWidget *vbox;

    if (pth != 0)
    {
        pthread_cancel(pth);
    }

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    SWverbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    ScrWin = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_name(ScrWin, "ScrWin");
    SWhorBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(ScrWin), GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(ScrWin), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(SWhorBox), ScrWin, FALSE, FALSE, 0);
    Profbutton = gtk_button_new();
    gtk_widget_set_name(Profbutton, "PrButt");
    gtk_widget_set_size_request(Profbutton, 20, 15);
    Phbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(Phbox, "PB");
    gtk_box_pack_start(GTK_BOX(Phbox), Profbutton, FALSE, FALSE, 0);
    if (count_main == 0)
    {
        MainMenu();
        if (count_main == 0)
        {
            count_main++;
            arr = mx_strsplit(res, ' ');
        }
    }
    memset(res, '\0', sizeof(res));
    int i = 0, arrlen = 0;

    if (arr != NULL)
    {
        while (arr[i] != NULL)
        {
            i++;
        }
    }
    GtkWidget *ChatButt[i];
    if (count_main != 4)
    {
        if (arr != NULL)
        {
            arrlen = i;
            i = 0;
            while (arr[i] != NULL)
            {
                ChatButt[i] = gtk_button_new_with_label(arr[i]);
                gtk_widget_set_name(ChatButt[i], "ChButt");
                gtk_widget_set_size_request(ChatButt[i], 400, 70);
                Chhbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
                gtk_widget_set_name(Chhbox, "ChB");
                gtk_box_pack_start(GTK_BOX(Chhbox), ChatButt[i], FALSE, FALSE, 0);
                gtk_box_pack_start(GTK_BOX(SWverbox), Chhbox, FALSE, FALSE, 0);
                i++;
            }
        }
    }
    NameLab = gtk_label_new("uChat");
    gtk_widget_set_name(NameLab, "NameLab");
    gtk_box_pack_end(GTK_BOX(Phbox), NameLab, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), Phbox, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(ScrWin), SWverbox);
    gtk_box_pack_start(GTK_BOX(vbox), SWhorBox, TRUE, TRUE, 0);

    if (data != NULL)
        gtk_container_remove(GTK_CONTAINER(window), (GtkWidget *)data);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    g_signal_connect(GTK_BUTTON(Profbutton), "clicked", G_CALLBACK(Profile), (gpointer)(vbox));
    for (int x = 0; x < arrlen; x++)
    {
        g_signal_connect(GTK_BUTTON(ChatButt[x]), "clicked", G_CALLBACK(ChatWin), (gpointer)(vbox));
    }
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
}

void CloseWind(GtkButton *button, gpointer data)
{
    gtk_window_close(GTK_WINDOW(data));
}

void Warning(gpointer data)
{
    GtkWidget *warning;
    GtkWidget *ok_button;
    GtkWidget *Warn, *Cont=NULL;
    GtkWidget *vbox, *hbox;
    GtkWidget *hbox1, *hbox2;

    warning = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(warning, "Warn");
    gtk_window_set_title(GTK_WINDOW(warning), "Warning");
    gtk_widget_set_size_request(warning, 380, 185);
    gtk_window_move(GTK_WINDOW(warning), 35, 290);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    ok_button = gtk_button_new_with_label("Ok");
    gtk_widget_set_name(ok_button, "OK");
    gtk_widget_set_size_request(ok_button, 100, 30);
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox), ok_button, FALSE, FALSE, 0);

    Warn = gtk_label_new("Warning");
    gtk_widget_set_name(Warn, "WarnLab");
    gtk_widget_set_size_request(Warn, 125, 35);
    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox1), Warn, FALSE, FALSE, 0);

    if (strcmp((char *)data, "1") == 0)
        Cont = gtk_label_new("Your login or password is incorrect");
    else if (strcmp((char *)data, "0") == 0)
        Cont = gtk_label_new("Something is wrong! Check input!");
    else if (strcmp((char *)data, "3") == 0)
        Cont = gtk_label_new("No such user!");
    gtk_widget_set_name(Cont, "ContLab");
    gtk_widget_set_size_request(Cont, 125, 35);
    hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox2), Cont, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(warning), vbox);
    gtk_widget_show_all(warning);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_window_close), NULL);
    g_signal_connect(G_OBJECT(ok_button), "clicked", G_CALLBACK(CloseWind), warning);
}

void CheckSingIn(GtkButton *button, gpointer data)
{

    login_en = strdup((char *)gtk_entry_get_text(GTK_ENTRY(Login)));
    pass_en = strdup((char *)gtk_entry_get_text(GTK_ENTRY(Password)));

    if (strcmp(login_en, "") != 0 && strcmp(pass_en, "") != 0)
    {
        memset(recvBuff, '\0', sizeof(recvBuff));
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Error : Could not create socket \n");
            return;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(8888);

        if (inet_pton(AF_INET, ARGV[1], &serv_addr.sin_addr) <= 0)
        {
            printf("\n inet_pton error occured\n");
            return;
        }

        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\n Error : Connect Failed \n");
            return;
        }

        char *sendS = concat("SingIn", login_en);
        sendS = concat(sendS, pass_en);
        write(sockfd, sendS, strlen(sendS));

        n = read(sockfd, recvBuff, sizeof(recvBuff) - 1);
        recvBuff[n] = 0;

        if (n < 0)
            printf("\n Read error \n");

        if (strcmp(recvBuff, "True") == 0)
        {
            memset(lg, '\0', sizeof(lg));
            for (int i = 0; login_en[i] != '\0'; i++)
            {
                lg[i] = login_en[i];
            }
            free(sendS);
            MainD(NULL, data);
        }
        else if (strcmp(recvBuff, "False") == 0)
        {
            Warning("1");
        }
    }
    else
    {
        Warning("1");
    }

    if (arr != NULL)
    {
        int h = 0;
        while (arr[h] != NULL)
        {
            h++;
        }

        for (int x = 0; x < h; x++)
        {
            memset(arr[x], '\0', strlen("\0"));
        }
        memset(arr, '\0', strlen("\0"));
    }
    return;
}

void SingIn(GtkButton *button, gpointer data)
{
    GtkWidget *SingIn, *Regist;
    GtkWidget *vbox, *vbox1, *vbox2, *hbox, *hbox1;
    GtkWidget *box, *box1;
    GtkWidget *Label;
    count_main = 0;
    Regist = gtk_button_new_with_label("Create account");
    gtk_widget_set_name(Regist, "Reg");

    SingIn = gtk_button_new_with_label("Sign In");
    gtk_widget_set_name(SingIn, "Sign");

    int coun = 0;
    if (arr != NULL)
    {
        while (arr[coun] != NULL)
        {
            coun++;
        }

        for (int x = 0; x < coun; x++)
        {
            memset(arr[x], '\0', strlen("\0"));
        }
        memset(arr, '\0', strlen("\0"));
    }

    Label = gtk_label_new("uChat");
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_name(Label, "Chat");
    gtk_box_pack_start(GTK_BOX(hbox), Label, TRUE, FALSE, 0);
    gtk_widget_set_size_request(Label, 158, 55);

    hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox1), Regist, TRUE, FALSE, 5);
    gtk_widget_set_size_request(Regist, 175, 80);

    Login = gtk_entry_new();
    gtk_widget_set_name(Login, "Login");
    gtk_widget_set_size_request(Login, 328, 80);
    vbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox1), Login, TRUE, FALSE, 5);
    gtk_entry_set_placeholder_text(GTK_ENTRY(Login), "login");

    Password = gtk_entry_new();
    gtk_widget_set_name(Password, "Pass");
    gtk_widget_set_size_request(Password, 328, 80);
    vbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox2), Password, TRUE, FALSE, 5);
    gtk_entry_set_placeholder_text(GTK_ENTRY(Password), "password");
    gtk_entry_set_visibility((GtkEntry*)Password, FALSE);

    vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), SingIn, TRUE, FALSE, 5);
    gtk_widget_set_size_request(SingIn, 328, 80);

    gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), vbox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), vbox2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), vbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), hbox1, FALSE, FALSE, 0);

    gtk_widget_set_name(box, "Entry");

    if (data != NULL)
        gtk_container_remove(GTK_CONTAINER(window), (GtkWidget *)data);
    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(GTK_BUTTON(Regist), "clicked", G_CALLBACK(Registration), box);
    g_signal_connect(GTK_BUTTON(SingIn), "clicked", G_CALLBACK(CheckSingIn), box);

}

int main(int argc, char *argv[])
{
    ARGV = argv;
    if (argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n", argv[0]);
        return 1;
    }
    gtk_init(&argc, &argv);
    signal(SIGINT, catch_ctrl_c_and_exit);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "CHAT");
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 812);
    gtk_widget_set_name(window, "Wind");
    StyleStandart();
    SingIn(NULL, NULL);
    gtk_main();
    close(sockfd);
    return 0;
}
