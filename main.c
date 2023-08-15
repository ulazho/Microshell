#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#define PATH_MAXX 4096
#define true 1
#define false 0
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


char PATH[PATH_MAXX];
char PATH_HISTORY[PATH_MAXX];
char HOME_PATH[PATH_MAXX];
int count = 0;
char comand[256][PATH_MAXX];

struct PS_UNIX_FORMAT{
    char* USER;
    int RUID;
    char* LABLE;
    char* F;
    int S;
    int UID;
    int EUID;
    int PID;
    int PPID;
    int LWP;
    int PGID;
    int SID;
    float C;
    long NLWP;
    char* ADDR;
    long SZ;
    char* WCHAN;
    long RSS;
    int PSR;
    char* STIME;
    char* TTY;
    char* TIME;
    char* SIMPLE_CMD;
    char* DIF_CMD;
};

struct OUT_PS_UNIX_FORMAT{
    bool USER;
    bool LABLE;
    bool F;
    bool S;
    bool UID;
    bool RUID;
    bool EUID;
    bool PID;
    bool PPID;
    bool LWP;
    bool PGID;
    bool SID;
    bool C;
    bool NLWP;
    bool ADDR;
    bool SZ;
    bool WCHAN;
    bool RSS;
    bool PSR;
    bool STIME; //?
    bool TTY;
    bool TIME; //?
    bool SIMPLE_CMD;
    bool DIF_CMD;
};

struct stack{
    int size;
    int last;
    int* el;
};


struct stack children;


void push(int pid){
    children.el = realloc(children.el, sizeof(int) * children.size + 1);
    children.el[children.size] = pid;
    children.last = pid;
    children.size++;
}

void pop(){
    if(children.size < 1) return;
    children.el = realloc(children.el, sizeof(int) * (children.size - 1));
    children.size--;
    if(children.size >= 1) children.last = children.el[children.size - 1];
}

struct PS_UNIX_FORMAT procu;
struct OUT_PS_UNIX_FORMAT procub;

void clear(){
    int i, j;
    i = j = 0;
    while(true){
        if(comand[i][j] == '\0' && j == 0) break;
        if(comand[i][j] != '\0'){
            comand[i][j] = '\0';
            j++;
        }
        else{
            i = j = 0;
        }
    }
    count = 0;
}

void clear_ps_unix_format(){
    procub.USER = procub.LABLE = procub.F = procub.S = procub.RUID = procub.EUID = procub.UID = procub.SIMPLE_CMD = procub.LWP = procub.PGID = procub.SID = procub.C = procub.NLWP =
    procub.ADDR = procub.SZ = procub.WCHAN = procub.RSS = procub.PSR = procub.STIME = procub.DIF_CMD = false;
    procub.PID = procub.TTY = procub.TIME = procub.SIMPLE_CMD = true;
}

void emthy_comand(){
    for(int i = 0; i < 256;i++)
        for(int j = 0; j < PATH_MAXX; j++)
            comand[i][j] = '\0';
}

void input(){
    int len = 0;
    while (true){
        scanf("%c", &comand[count][len]);
        if(comand[count][len] == '\n') {
            comand[count][len] = '\0';
            break;
        }
        if(comand[count][len] == ' '){
            comand[count][len] = '\0';
            if(len == 0) continue;
            count++;
            len = 0;
        }else
            len++;
    }
    if(strcmp(comand[count],"")) count++;
}

void cd(){
    int len = strlen(PATH);
    if(count == 1 || !strcmp("~", comand[1])){
        strcpy(PATH,HOME_PATH);
        chdir(PATH);
        return;
    }else
    if(!strcmp(comand[1], ".")){

    }else
    if(!strcmp(comand[1],"-")){
        strcpy(PATH, PATH_HISTORY);
        printf("%s\n", PATH);
    }else
    if(!strcmp(comand[1], "..")){
        int i = len - 1;
        while(PATH[i] != '/'){
            PATH[i] = '\0';
            i--;
        }
        if(strcmp(PATH, "/")) PATH[i] = '\0';
    }else{

        char s[PATH_MAXX] = "";
        if(count > 2){
            for(int i = 1; i < count; i++){
                comand[i][strlen(comand[i])] = ' ';
                strcat(s, comand[i]);
            }
        } else strcpy(s, comand[1]);

        if (comand[1][0] == '/') {
            DIR* cur_dir = opendir(s);
            if (cur_dir != NULL) {
                strcpy(PATH, s);
            } else {
                printf("Błąd : %s \n", strerror(errno));
            }
        }
        else{
            char cur_path[PATH_MAXX];
            strcpy(cur_path, PATH);
            if(strcmp(PATH, "/"))   strcat(cur_path, "/");
            strcat(cur_path, s);
            DIR* cur_dir = opendir(cur_path);
            if(cur_dir != NULL){
                strcpy(PATH, cur_path);
            }else{
                printf("Błąd : %s \n", strerror(errno));
            }
        }
    }
    strcpy(PATH_HISTORY, PATH);
    chdir(PATH);
}

void exe(){
    pid_t pid = fork();
    if(pid == -1){
        perror("Problem with fork()");
    }
    if(pid == 0){
        char* ar[256];
        for(int i = 0; i < count; i++)
            ar[i] = comand[i];
        ar[count] = NULL;
        execvp(ar[0], ar);
        if(errno != 0){
            perror("");
            exit(0);
        }
    }else {
        wait(NULL);
    }
}

char* uid_to_username(int uid){
    if(uid == 0){
        return "root";
    }
    else{
        return getlogin();
    }
    char* username = malloc(sizeof(char) * 32);
    FILE* fp;
    fp = fopen("/etc/passwd", "r");
    while (!feof(fp)) {
        int now_uid, now_pos = 0;
        do {
            username[now_pos] = fgetc(fp);
            now_pos++;
        } while (username[now_pos - 1] != ':');
        username[now_pos - 1] = '\0';
        while (fgetc(fp) != ':') {}

        fscanf(fp, "%d", &now_uid);
        if (now_uid == uid) {
            return  username;
        }
        else{
            while (!feof(fp)){
                if(fgetc(fp) == '\n') break;
            }
        }
    }
    return NULL;
}

long long get_boot_time(){
    FILE* file = fopen("/proc/stat", "r");
    char s[64] = "";
    while(strcmp(s, "btime")) {
        fscanf(file,"%s", s);
    }
    fscanf(file, "%s", s);
    long long tick = atoll(s);
    return tick;
}

void get_tty(int tty){
    if(tty == 0) {
        strcpy(procu.TTY, "?");
        return;
    }
    if(tty >= 1024 && tty <= 1088) {
        tty -= 1024;
        strcpy(procu.TTY,"tty");
    }
    if(tty >= 34816) {
        tty -= 34816;
        strcpy(procu.TTY,"pts/");
    }
    char str[2];
    sprintf(str, "%d", tty);
    strcat(procu.TTY, str);
}


void get_TIME(long tick){ // сделять для hour > 24 прошлый день
    long sec, min, hour;
    tick /= 100;
    sec = min = hour = 0;
    sec = tick % 60;
    tick -= sec;
    tick /= 60;
    min = tick % 60;
    tick -= min;
    tick /= 60;
    hour = tick;
    char str[4] = "";
    char ans[32] = "";
    sprintf(str, "%lu", hour);
    if(hour< 10)  strcat(ans,"0");
    strcat(ans, str);
    strcat(ans, ":");
    sprintf(str, "%lu", min);
    if(min < 10) strcat(ans,"0");  /// отделить в функцию вохможно
    strcat(ans, str);
    strcat(ans, ":");
    sprintf(str, "%lu", sec);
    if(sec < 10) strcat(ans,"0");;
    strcat(ans, str);
    strcpy(procu.TIME, ans);
}

void get_STIME(long long tick){
    tick /= 100;
    tick += get_boot_time();
    time_t time = tick;
    struct tm* date;
    date = localtime(&time);
    char str[4] = "";
    char ans[32] = "";
    sprintf(str, "%d", date->tm_hour);
    if(date->tm_hour< 10)  strcat(ans,"0");
    strcat(ans, str);
    strcat(ans, ":");
    sprintf(str, "%d", date->tm_min);
    if(date->tm_min < 10) strcat(ans,"0");  /// отделить в функцию вохможно
    strcat(ans, str);
    strcpy(procu.STIME, ans);
}


float get_CPU(long long sys_uptime,long long proces_use_time, long long proces_uptime){
    proces_use_time /= 100;
    proces_uptime /= 100;
    long int sec =  sys_uptime - proces_uptime;
    long int cpu = proces_use_time * 1000 / sec;
    float  ans;
    ans = (cpu % 10) / 10.0 + cpu / 10;
    return ans;
}


void make_procu(){
    procu.F = malloc(sizeof(char));
    procu.TTY = malloc(sizeof (char) * 32);
    procu.TIME = malloc(sizeof(char) * 10);
    procu.STIME = malloc(sizeof(char) * 10);
    procu.DIF_CMD = malloc(sizeof(char )* PATH_MAXX);
    procu.SIMPLE_CMD = malloc(sizeof (char) * 256);
    procu.USER = malloc(sizeof (char ) * 256);
}

void read_stat(int pid){
    FILE* file;
    char path[PATH_MAXX] = "/proc/";
    char c_pid[PATH_MAXX];
    char buf[PATH_MAXX];
    int num_buf;
    long buf_tick, buf_tick_2;
    long long buf_ll;
    sprintf(c_pid, "%d", pid);
    strcat(path, c_pid);
    strcat(path, "/stat");
    file = fopen(path, "r");
    fscanf(file, "%d", &procu.PID); // 1
    procu.LWP = procu.PID;
    fscanf(file, "%c", &buf[0]); // same atr
    fscanf(file, "%c", &buf[0]); // same atr
    fscanf(file, "%[^)]", procu.SIMPLE_CMD); // same atr
    fscanf(file, "%s", buf);    // same atr
    fscanf(file,"%s", procu.F);
    fscanf(file, "%d", &procu.PPID);
    fscanf(file, "%d", &procu.PGID); // 5
    fscanf(file, "%d", &procu.SID);
    fscanf(file, "%d", &num_buf);
    get_tty(num_buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf); // 10
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%lu", &buf_tick);
    fscanf(file, "%lu", &buf_tick_2); //15
    buf_tick += buf_tick_2;
    get_TIME(buf_tick);
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%s", buf);
    fscanf(file, "%ld", &procu.NLWP); // 20
    fscanf(file, "%s", buf);
    fscanf(file, "%lld", &buf_ll);
    get_STIME(buf_ll);
    procu.C = get_CPU(time(NULL) - get_boot_time(),buf_tick,buf_ll);
    procu.C = (int)procu.C;
    for(int i = 23; i <= 38; i++)
        fscanf(file, "%s", buf);
    fscanf(file, "%d", &procu.PSR);
    fclose(file);
}

void read_statm(){
    FILE* file;
    char path[64] = "/proc/";
    char snum[10] = "";
    sprintf(snum, "%d", procu.PID);
    strcat(path, snum);
    strcat(path, "/statm");
    file = fopen(path, "r");
    fscanf(file, "%ld",&procu.SZ);
    fclose(file);
}

void read_cmdlinne() {
    FILE *file;
    char path[64] = "/proc/";
    char snum[10] = "";
    sprintf(snum, "%d", procu.PID);
    strcat(path, snum);
    strcat(path, "/cmdline");
    file = fopen(path, "r");
    fscanf(file, "%s", procu.DIF_CMD);
    fclose(file);
}

void read_status(){
    FILE *file;
    char str[256] = "";
    char path[64] = "/proc/";
    char snum[10] = "";
    sprintf(snum, "%d", procu.PID);
    strcat(path, snum);
    strcat(path, "/status");
    file = fopen(path, "r");
    while(strcmp(str, "Uid:")) fscanf(file, "%s", str);
    fscanf(file, "%d",&procu.RUID);
    fscanf(file, "%d",&procu.EUID);
    while(strcmp(str, "SigQ:") && strcmp(str, "VmRSS"))
        fscanf(file, "%s", str);
    if(!strcmp(str, "VmRSS:")) fscanf(file, "%ld", &procu.RSS);
    procu.UID = procu.RUID;
    procu.USER = uid_to_username(procu.UID);
    fclose(file);
}

void get_children_list(){
    FILE* file;
    char path[128] = "/proc/";
    char snum[10] = "";
    sprintf(snum, "%d", procu.PID);
    strcat(path, snum);
    strcat(path, "/task/");
    strcat(path, snum);
    strcat(path, "/children");
    file = fopen(path, "r");
    int pid;
    fscanf(file, "%d", &pid);
    while(!feof(file)){
        push(pid);
        fscanf(file, "%d", &pid);
    }

}

void out_top_line(){
    char str[256] = "";
    if(procub.UID || procub.USER) strcat(str, "UID     ");
    if(procub.PID) strcat(str, "PID\t\t");
    if(procub.PPID) strcat(str, "PPID\t");
    if(procub.PGID) strcat(str, "PGID\t");
    if(procub.SID) strcat(str, "SID\t");
    if(procub.LWP) strcat(str, "LWP\t");
    if(procub.C) strcat(str, "C\t");
    if(procub.NLWP) strcat(str, "NLWP\t");
    if(procub.SZ) strcat(str, "SZ\t");
    if(procub.RSS) strcat(str, "RSS\t");
    if(procub.PSR) strcat(str, "PSR\t");
    if(procub.STIME) strcat(str, "STIME\t");
    if(procub.TTY) strcat(str, "TTY\t");
    if(procub.TIME) strcat(str, "TIME        ");
    if(procub.SIMPLE_CMD || procub.DIF_CMD) strcat(str, "CMD\n");
    printf("%s", str);
}

void out_process(){
    if(procub.USER) printf("%s\t", procu.USER);
    if(procub.PID) printf("%d\t", procu.PID);
    if(procub.PPID) printf("%d\t", procu.PPID);
    if(procub.PGID) printf("%d\t", procu.PGID);
    if(procub.SID) printf("%d\t", procu.SID);
    if(procub.LWP) printf("%d\t", procu.LWP);
    if(procub.C) printf("%.0f\t", procu.C);
    if(procub.NLWP) printf("%ld\t", procu.NLWP);
    if(procub.SZ) printf("%ld\t", procu.SZ);
    if(procub.RSS) printf("%ld\t", procu.RSS);
    if(procub.PSR) printf("%d\t", procu.PSR);
    if(procub.STIME) printf("%s\t", procu.STIME);
    if(procub.TTY) printf("%s\t", procu.TTY);
    if(procub.TIME) printf("%s\t", procu.TIME);
    if(procub.SIMPLE_CMD || !strcmp(procu.DIF_CMD,"")) printf("%s\t", procu.SIMPLE_CMD); else
    if(procub.DIF_CMD) printf("%s\t", procu.DIF_CMD);
    printf("\n");
}

void fill_proc(int pid){
    procu.PID = pid;
    read_stat(pid);
    read_statm();
    read_cmdlinne();
    read_status();
}

void ps(){
    clear_ps_unix_format();
    pid_t pid = getppid();
    fill_proc(pid);
    if(count == 1){
        out_top_line();
        int euid = procu.EUID;
        get_children_list();
        out_process();
        while (children.size > 0){
            fill_proc(children.last);
            pop();
            if(procu.EUID == euid) out_process();
        }
    } else{
        bool all = false;
        char str[256] = "";
        int str_size = 0;
        int i, j;
        i = 1;
        j = 0;
        for(;;){
            if(j == 0 && comand[i][j] == '\0') break;
            if(comand[i][j] == '\0'){
                j = 0;
                i++;
            }
            else{
                str[str_size] = comand[i][j];
                str_size++;
                j++;
            }
        }
        if(str[0] != '-' || str_size < 2) {
            fprintf(stderr, "Error: using only standart sytax\n");
            return;
        }
        for(int i = 1; i < str_size; i++){
            bool f = true;
            if (str[i] == 'j') {
                f = false;
                procub.PGID = true;
                procub.SID = true;
            }
            if (str[i] == 'F') {
                procub.SZ = true;
                procub.RSS = true;
                str[i] = 'f';
            }
            if (str[i] == 'f') {
                f = false;
                procub.USER = true;
                procub.PPID = true;
                procub.C = true;
                procub.STIME = true;
                procub.TTY = true;
                procub.TIME = true;
                procub.SIMPLE_CMD = false;
                procub.DIF_CMD = true;
            }
            if(str[i] == 'e') {
                f = false;
                all = true;
            }
            if(f){
                perror("Wrong arg for ps");
                break;
            }
        }
        out_top_line();
        if(all){
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir("/proc")) == NULL)
                perror("Error opendir() function");
            else{
                while ((ent = readdir(dir)) != NULL){
                    int pid = atoi(ent->d_name);
                    if(pid > 0){
                        fill_proc(pid);
                        out_process();
                    }
                }
                closedir(dir);
            }
        }
        else{
            out_process();
            int euid = procu.EUID;
            get_children_list();
            while (children.size > 0) {
                fill_proc(children.last);
                pop();
                if (procu.EUID == euid) out_process();
            }
        }
    }
}


int main(){
    emthy_comand();
    make_procu();
    getcwd(PATH, PATH_MAXX);
    strcpy(HOME_PATH, PATH);
    while (true) {
        clear();
        printf("[%s]\n$", PATH);
        input();
        if(!strcmp(comand[0], "exit")) break;
        if(!strcmp(comand[0], "cd")) cd(); else
        if(!strcmp(comand[0], "ps")) ps();
        else{
            exe();
        }
    }
    return 0;
}
