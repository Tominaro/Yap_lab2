//bref, j'ai modifi? deux ou trois choses et supprim? printf foncs... quel d?sordre, с'est foutu
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <locale.h>
#include <stdlib.h>
#include <wincon.h>
//#include <windows.h>  -- ломает blocked
//#pragma comment (lib, "<libname>") для диалогового окна

//память и отслеживание ее утечки
//-------------------------------------new
//int timer;
LARGE_INTEGER timer;
const char *timed_c[]={"insert","select","delete","update","uniq"};
//float
double sec[5]={0};//commands for each actiona
unsigned long long ccalls[5]={0};//calls for each action
double timer_new(void){
    //int integer1;
    /*// Gets the current number of ticks from QueryPerformanceCounter. Throws an
// exception if the call to QueryPerformanceCounter fails.
static inline int64_t GetTicks()
{
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks))
    {
        winrt::throw_last_error();
    }
    return ticks.QuadPart;
}*/
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks))
    {
        //winrt::throw_last_error(); C++ only
        exit(EXIT_FAILURE);
    }
    return (double)ticks.QuadPart/(double)timer.QuadPart;
}
//-------------------------------------new end
static long malloc_cntv = 0;
static long calloc_cntv = 0;
static long realloc_cntv = 0;
static long mfree_cntv = 0;
static void *malloc_trk1(size_t siz)
{
    malloc_cntv++;
    return malloc(siz);
}

static void *calloc_trk(size_t n,size_t siz)
{
    calloc_cntv++;
    return calloc(n,siz);
}

//realloc(NULL,siz) -- compte comme malloc selon la sp?c

static void *realloc_trk0(void *ptr, size_t siz)
{
    if (ptr == NULL)
        malloc_cntv++;
    else
        realloc_cntv++;
    return realloc(ptr, siz);
}

static void memfree_co(void *ptr)
{
    if (ptr) {
        mfree_cntv++;
        free(ptr);
    }
}

// макросы, чтобы писать сразу с слежкой
#define MALLOCA(siz) malloc_trk1(siz)
#define CALLOCA(n,siz) calloc_trk(n,siz)
#define REALLOCA(p,siz) realloc_trk0(p,siz)
#define MFREE(p) memfree_co(p)

// типы

// hours*3600+minutes*60+seconds
typedef struct{
    int h;//0-23
    int s;//0-59
    int m;//0-59
} val_t;
/*
BOOL GetOpenFileNameA(
    [in, out] LPOPENFILENAMEA unnamedParam1
//DLL 	Comdlg32.dll
    );

//These routines provide a way to change the representation of the file between a FILE structure, a file descriptor, and a Win32 file handle.
HANDLE FileFun(char* szFile,int size){
    OPENFILENAME ofn = {0};       // common dialog box structure
    HWND hwnd;              // owner window
    HANDLE hf;              // file handle
    //szFile = NULL;
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = size;
    ofn.lpstrFilter = "Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    if (GetOpenFileName(&ofn)==TRUE){
        hf = CreateFile(ofn.lpstrFile,
                        GENERIC_READ,
                        0,
                        (LPSECURITY_ATTRIBUTES) NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        (HANDLE) NULL);
        return hf;
    }
    return NULL;
}

FILE *handle_converter(HANDLE hf){
    if (hf==INVALID_HANDLE_VALUE){
        printf("\n обр умер\n");
        return NULL;
    }

O_APPEND 	Repositions the file pointer to the end of the file before every write operation.
_O_CREAT 	Creates and opens a new file for writing; the constant has no effect if the file specified by filename exists.
_O_EXCL 	Returns an error value if the file specified by filename exists. Only applies when used with _O_CREAT.
_O_RDONLY 	Opens file for reading only; if this flag is given, _O_RDWR and _O_WRONLY can't be given.
_O_RDWR 	Opens file for both reading and writing; if this flag is given, _O_RDONLY and _O_WRONLY can't be given.
_O_TRUNC 	Opens and truncates an existing file to zero length; the file must have write permission. The contents of the file are destroyed. If this flag is given, you can't specify _O_RDONLY.
_O_WRONLY 	Opens file for writing only; if this flag is given, _O_RDONLY and _O_RDWR can't be given.
     *
     *

    //int _open_osfhandle (
    //intptr_t osfhandle,    <- файл
    //int flags              <- см. <fcntl.h>
    //);

    //returns 0 if the file was successfully closed. A return value of -1 indicates an error.

    int discriptor=_open_osfhandle((intptr_t)hf,_O_RDONLY|_O_BINARY);//hf: HANDLE -> intptr_t; ???? а надо ли  _O_BINARY \\ надо
    if (discriptor==0||discriptor==(-1)){
        printf("\nдиск не считался/не передался\n");
        return NULL;
    }

    FILE *file=_fdopen(discriptor,"rb");//rb если _O_BINARY
    if (!file){
        printf("\nфайл откис при диск->файл\n");
        _close(discriptor);
        return NULL;
    }else{
        return file;
    }

    return NULL;
}


*/
typedef enum {
    RUNNING = 0,
    READY,
    PAUSED,
    BLOCKED,
    DYING,
    SLEEPING,
    INVALID = -1
} Status;
/*int c_symbol=0;
DWORD symbols=0;*/
/*void sort(int arr[][2],int size){
    int t1[2]={0};
    bool tr=true;
    while(tr){
        tr=false;
        for(int i =1;i<=size-1;i++){
            if(arr[i-1][1]<arr[i][1]){
                tr=true;
                t1[0]=arr[i-1][0];
                t1[1]=arr[i-1][1];
                arr[i-1][1]=arr[i][1];
                arr[i-1][0]=arr[i][0];
                arr[i][0]=t1[0];
                arr[i][1]=t1[1];
            }
        }
    }
}*/
static const char *STATUS_NAMES[]= {"running","ready","paused","blocked","dying","sleeping"};
#define NUM_STATUSES 6

typedef struct {
    int pid;
    char *name;
    int priority;
    val_t kern_tm;
    val_t file_tm;
    double cpu_usage;
    Status status;
} re2cord;

typedef struct lin_obj {
    re2cord rec;
    struct lin_obj *next;
} lin_obj;

typedef struct {
    lin_obj *head;
    lin_obj *tail;
    int   count;
} DB;

// обр выхода

static FILE *ouputf = NULL;

static void out(const char *fmt_cfa, ...)
{
    va_list ap;
    va_start(ap, fmt_cfa);
    vprintf(fmt_cfa, ap);
    va_end(ap);

    if (ouputf) {
        va_list ap2;
        va_start(ap2, fmt_cfa);
        vfprintf(ouputf, fmt_cfa, ap2);
        va_end(ap2);
    }
}

//string util.. REFAIRE!!!

//duplicate
static char *str_dup(const char *s)
{
    if (!s) return NULL;
    size_t len=strlen(s)+1;
    char *p=MALLOCA(len);
    if (p){
        memcpy(p,s,len);
    }
    return p;
}

static char *str_srez(char *s)
{
    while (*s&&isspace((unsigned char)*s))s++;
    char *end=s+strlen(s);
    while ((end>s)&&(isspace((unsigned char)*(end-1)))) end--;
    *end='\0';
    return s;
}

//парсы

static int parse_int(const char *s, int *out_val)
{
    char *end;
    if (!s || !*s) return 0;
    long v = strtol(s, &end, 10);
    if (*end != '\0') return 0;
    *out_val = (int)v;
    return 1;
}

static int parse_double(const char *s, double *out_val)
{
    char *end;
    if (!s || !*s) return 0;
    double v = strtod(s, &end);
    if (*end != '\0') return 0;
    *out_val = v;
    return 1;
}

// парс времени
static int parse_time(const char *s, val_t *t)
{
    if (!s) return 0;
    char buf[64];
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    char *p = buf;
    if (*p == '\'' || *p == '"') {
        p++;
        size_t l = strlen(p);
        if (l > 0 && (p[l-1] == '\'' || p[l-1] == '"'))
            p[l-1] = '\0';
    }
    int h, m, sec;
    //what is sscnaf
    //
    //сomme scanf, mais en mieux
    //Как scanf, но лучше.
    //Kak scanf, no luchshe.
    if (sscanf(p,"%d:%d:%d",&h,&m,&sec)!= 3) return 0;
    //беда с пробелами
    if ((h<0)||(h>23)||(m<0)||(m>59)||(sec<0)||(sec>59)) return 0;
    t->h = h;
    t->m = m;
    t->s = sec;
    return 1;
}

static int parse_status(const char *s, Status *st)
{
    if (!s) return 0;
    char buf[64];
    strncpy(buf, s, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    char *p = buf;
    if ((*p=='\'')||(*p == '"')){
        p++;
        size_t l=strlen(p);
        if (l>0&&((p[l-1]=='\'')||(p[l-1]=='"')))
            p[l-1] = '\0';
    }
    for (int i = 0; i < NUM_STATUSES; i++) {
        if (strcmp(p, STATUS_NAMES[i]) == 0) {
            *st = (Status)i;
            return 1;
        }
    }
    return 0;
}

static int time_conv(val_t t) {
    return t.h*3600+t.m*60+t.s;
}

// создание \освобождение записей

static void re2cord_free_fields(re2cord *r)
{
    MFREE(r->name);
    r->name = NULL;
}

//операции с линейными спиками

static void db_init(DB *db){
    db->head=db->tail=NULL;
    db->count= 0;
}

static void db_append(DB *db,re2cord *r)
{
    lin_obj *n= MALLOCA(sizeof(lin_obj));
    n->rec  =*r;
    n->next= NULL;
    //начало в конец конец в начало
    if (!db->tail){
        db->head=db->tail=n;
    }
    else {
        db->tail->next=n;
        db->tail= n;
    }
    db->count++;
}

// удаляет список до (ну или if prev==NULL удаляет сам список) return след
static lin_obj *db_remove_lin_obj(DB *db, lin_obj *prev, lin_obj *cur)
{
    lin_obj *nxt= cur->next;
    if (prev){
        prev->next= nxt;
    }
    else {
        db->head= nxt;
    }
    if (cur== db->tail){
        db->tail = prev;
    }
    re2cord_free_fields(&cur->rec);
    MFREE(cur);
    db->count--;
    return nxt;
}

//названия полей, см вар

#define FIELD_PID       0
#define FIELD_NAME      1
#define FIELD_PRIORITY  2
#define FIELD_KERN_TM   3
#define FIELD_FILE_TM   4
#define FIELD_CPU_USAGE 5
#define FIELD_STATUS    6
#define NUM_FIELDS      7

static const char *FIELD_NAMES[NUM_FIELDS] = {"pid","name","priority","kern_tm","file_tm","cpu_usage","status"};

static int field_index(const char *name)
{
    for (int i = 0; i < NUM_FIELDS; i++)
        if (strcmp(name, FIELD_NAMES[i]) == 0) return i;
    return -1;
}

// распечатать одно поле

static void print_field_value(FILE *f, const re2cord *r, int fi)
{
    switch (fi) {
    case FIELD_PID:      fprintf(f, "%d",          r->pid);      break;
    case FIELD_NAME:     fprintf(f, "%s",           r->name);     break;
    case FIELD_PRIORITY: fprintf(f, "%d",          r->priority); break;
    case FIELD_KERN_TM:  fprintf(f, "%02d:%02d:%02d", r->kern_tm.h, r->kern_tm.m, r->kern_tm.s); break;
    case FIELD_FILE_TM:  fprintf(f, "%02d:%02d:%02d", r->file_tm.h, r->file_tm.m, r->file_tm.s); break;
    case FIELD_CPU_USAGE:fprintf(f, "%g",           r->cpu_usage); break;
    case FIELD_STATUS:   fprintf(f, "%s",           STATUS_NAMES[r->status]); break;
    }
}

static void out_field(const re2cord *r, int fi)
{
    print_field_value(stdout, r, fi);
    if (ouputf) print_field_value(ouputf, r, fi);
}

// услвоия (писал пьер)

typedef enum{
    OP_EQ,          /* = */
    OP_NEQ,         /* != */
    OP_LT,          /* < */
    OP_GT,          /* > */
    OP_LE,          /* <= */
    OP_GE,          /* >= */
    OP_IN,          /* /in/*/
    OP_INVALID
} OpKind;

typedef struct {
    int    field_idx;
    OpKind op;
    char  *str_val;    //raw string
    int    int_val;
    double dbl_val;
    val_t time_val;
    Status  status_val;
    //  для |in|
    Status *status_list;
    int     status_list_len;
} Condition;

//analyser une chaine de condition comme "pid>5" ou status/in/['running','ready']
static int parse_condition(const char *s,Condition *cond)
{
    memset(cond,0 ,sizeof(*cond));
    cond->field_idx=(-1);
    const char *p=s;
    char fname[64]={0};
    int fi=0;
    while (*p&&(*p!='<')&&(*p!='>')&&(*p!='=')&&(*p!='!')&&(*p!='/')&&(fi<63))
        fname[fi++]=*p++;
    fname[fi]='\0';

    int fidx = field_index(fname);
    if (fidx < 0) return 0;
    cond->field_idx = fidx;
    OpKind op = OP_INVALID;
    const char *val_start = NULL;

    if (strncmp(p, "<=", 2) == 0) {
        op =OP_LE;
        val_start= p+2;
    }
    else if (strncmp(p, ">=", 2) == 0) {
        op =OP_GE;
        val_start= p+2;
    }
    else if (strncmp(p, "!=", 2) == 0) {
        op =OP_NEQ;

        val_start= p+2;
    }
    else if (strncmp(p, "==", 2) == 0) {
        op =OP_EQ;
        val_start= p+2;
    }
    else if (*p == '<') {
        op =OP_LT;
        val_start= p+1;
    }
    else if (*p == '>') {
        op =OP_GT;
        val_start= p+1;
    }
    else if (*p == '=') {
        op =OP_EQ;
        val_start = p+1;
    }
    else if (strncmp(p, "/in/", 4) == 0) {
        op =OP_IN;
        val_start = p+4;
    }
    else return 0;
    cond->op=op;



    if (op==OP_IN){
        if (fidx!=FIELD_STATUS) return 0;
    const char *lb= strchr(val_start,'[');
    const char *rb= strchr(val_start,']');


        if (!lb||!rb||(rb<lb)) return 0;
        char buf[256];
        int len = (int)(rb - lb - 1);
        if (len <= 0) return 0;
        strncpy(buf, lb+1, len);
        buf[len] = '\0';
        Status *list = NULL;
        int cnt = 0;
        char *tok = strtok(buf, ",");
        while (tok) {
            tok = str_srez(tok);
            Status st;
            if (!parse_status(tok, &st)) {
                MFREE(list);
                return 0;
            }
            Status *tmp = REALLOCA(list, (cnt+1)*sizeof(Status));
            if (!tmp) { MFREE(list); return 0; }
            list = tmp;
            list[cnt++] = st;
            tok = strtok(NULL, ",");
        }
        cond->status_list= list;
        cond->status_list_len= cnt;
        return 1;
    }

    char vbuf[256];
    strncpy(vbuf, val_start, sizeof(vbuf)-1);
    vbuf[sizeof(vbuf)-1] = '\0';
    char *v = str_srez(vbuf);

    switch (fidx) {
    case FIELD_PID:
    case FIELD_PRIORITY:
        return parse_int(v, &cond->int_val);
    case FIELD_CPU_USAGE:
        return parse_double(v, &cond->dbl_val);
    case FIELD_KERN_TM:
    case FIELD_FILE_TM:
        return parse_time(v, &cond->time_val);
    case FIELD_STATUS:
        return parse_status(v, &cond->status_val);
    case FIELD_NAME:
        if ((*v == '\'' || *v == '"')) {
            v++;
            size_t l = strlen(v);
            if (l > 0 && (v[l-1] == '\'' || v[l-1] == '"')) v[l-1] = '\0';
        }
        cond->str_val = str_dup(v);
        return cond->str_val != NULL;
    }
    return 0;
}

static void cond_free(Condition *c)
{
    MFREE(c->str_val);
    MFREE(c->status_list);
    c->str_val       = NULL;
    c->status_list   = NULL;
}

static int cond_match(const Condition *c, const re2cord *r)
{
    int fi = c->field_idx;
    OpKind op = c->op;

    if (op == OP_IN) {
        for (int i = 0; i < c->status_list_len; i++)
            if (r->status == c->status_list[i]) return 1;
        return 0;
    }
    //je vais ?tre honn?te... je ne me souviens plus de ce que j'ai ?crit ici, cest enti?rement de sa faute "**** ******"
    int cmp = 0;
    switch (fi) {
    case FIELD_PID:      cmp = (r->pid      < c->int_val) ? -1 : (r->pid      > c->int_val) ? 1 : 0; break;
    case FIELD_PRIORITY: cmp = (r->priority < c->int_val) ? -1 : (r->priority > c->int_val) ? 1 : 0; break;
    case FIELD_CPU_USAGE: cmp = (r->cpu_usage < c->dbl_val) ? -1 : (r->cpu_usage > c->dbl_val) ? 1 : 0; break;
    case FIELD_KERN_TM: { int a=time_conv(r->kern_tm), b=time_conv(c->time_val); cmp=(a<b)?-1:(a>b)?1:0; break; }
    case FIELD_FILE_TM: { int a=time_conv(r->file_tm),  b=time_conv(c->time_val); cmp=(a<b)?-1:(a>b)?1:0; break; }
    case FIELD_STATUS:  cmp = (int)r->status - (int)c->status_val; break;
    case FIELD_NAME:    cmp = strcmp(r->name, c->str_val ? c->str_val : ""); break;
    default: return 0;
    }

    switch (op) {
    case OP_EQ:  return cmp == 0;
    case OP_NEQ: return cmp != 0;
    case OP_LT:  return cmp <  0;
    case OP_GT:  return cmp >  0;
    case OP_LE:  return cmp <= 0;
    case OP_GE:  return cmp >= 0;
    default:     return 0;
    }
}

// ИЗМЕНЕНО, теперь возвращает 1 если все норм
static int re2cord_matches(const re2cord *r, Condition *conds, int nconds)
{
    for (int i = 0; i < nconds; i++)
        if (!cond_match(&conds[i], r)) return 0;
    return 1;
}

//еще парсы
static char **split_tk_vgae(const char *line, int *out_count)
{
    char *buf = str_dup(line);
    if (!buf){
        *out_count = 0;
        return NULL;
    }

    int cnt = 0;
    char *p = buf;
    while (*p) {
        while (*p && (isspace((unsigned char)*p))) p++;
        if (!*p) break;
        cnt++;
        while (*p && (!isspace((unsigned char)*p))) p++;
    }

    if (cnt==0){
        MFREE(buf);
        *out_count= 0;
        return NULL;
    }

    char **arr = MALLOCA((cnt+1)*sizeof(char*));
    if (!arr){
        MFREE(buf);
        *out_count= 0;
        return NULL;
    }

    int i = 0;
    p = buf;
    while (*p&&(i<cnt)){
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        arr[i++] = p;
        while (*p && !isspace((unsigned char)*p)) p++;
        if (*p){
            *p++ = '\0';
        }
    }
    arr[i]= NULL;
    *out_count= i;
    // arr[0] == buf, по идее чистит еще и буфер
    return arr;
}

// parse conditions from tk[start..ntok-1]
static int parse_conditions(char **tk_vgae, int start, int ntok,
                            Condition **out_conds, int *out_nconds)
{
    int n = ntok - start;
    if (n <= 0) { *out_conds = NULL; *out_nconds = 0; return 1; }

    Condition *conds = MALLOCA(n * sizeof(Condition));
    if (!conds) return 0;
    int parsed = 0;
    for (int i = 0; i < n; i++) {
        if (!parse_condition(tk_vgae[start+i], &conds[parsed])) {
            //free already parsed
            for (int j = 0; j < parsed; j++) cond_free(&conds[j]);
            MFREE(conds);
            return 0;
        }
        parsed++;
    }
    *out_conds  = conds;
    *out_nconds = parsed;
    return 1;
}

static void free_conditions(Condition *conds, int n)
{
    for (int i = 0; i < n; i++) cond_free(&conds[i]);
    MFREE(conds);
}

//insert

// парс полей разделенных коомами
static int cmd_insert(DB *db, const char *args_str)
{
    /* Copy args */
    char *buf = str_dup(args_str);
    if (!buf) return 0;

    // suivez les champs qui ont ?t? d?finis
    int set[NUM_FIELDS] = {0};
    re2cord r;
    memset(&r, 0, sizeof(r));
    r.name = NULL;

    char *saveptr = NULL;
    char *tok = strtok_r(buf, ",", &saveptr);
    int ok = 1;
    while (tok && ok) {
        tok = str_srez(tok);
        char *eq = strchr(tok, '=');
        if (!eq) { ok = 0; break; }
        *eq = '\0';
        char *fname = str_srez(tok);
        char *fval  = str_srez(eq+1);

        int fi = field_index(fname);
        if (fi < 0) { ok = 0; break; }
        if (set[fi]) { ok = 0; break; } //поиск дупликатов
        set[fi] = 1;

        switch (fi) {
        case FIELD_PID:
            if (!parse_int(fval, &r.pid)) ok = 0;
            break;
        case FIELD_NAME:
            r.name = str_dup(fval);
            if (!r.name) ok = 0;
            break;
        case FIELD_PRIORITY:
            if (!parse_int(fval, &r.priority)) ok = 0;
            break;
        case FIELD_KERN_TM:
            if (!parse_time(fval, &r.kern_tm)) ok = 0;
            break;
        case FIELD_FILE_TM:
            if (!parse_time(fval, &r.file_tm)) ok = 0;
            break;
        case FIELD_CPU_USAGE:
            if (!parse_double(fval, &r.cpu_usage)) ok = 0;
            break;
        case FIELD_STATUS:
            if (!parse_status(fval, &r.status)) ok = 0;
            break;
        }
        tok = strtok_r(NULL, ",", &saveptr);
    }
    MFREE(buf);

    //проверка на все поля
    if (ok) {
        for (int i = 0; i < NUM_FIELDS; i++)
            if (!set[i]) { ok = 0; break; }
    }

    if (!ok) {
        MFREE(r.name);
        return 0;
    }

    db_append(db, &r);
    out("insert: %d\n", db->count);
    return 1;
}

//select

static int cmd_select(DB *db, char **tk_vgae, int ntok)
{
    //tk_vgae[1] = field list, tk_vgae[2..] = conditions
    if (ntok < 2) return 0;

    //парс полей
    char *flist_buf = str_dup(tk_vgae[1]);
    if (!flist_buf) return 0;

    int req_fields[NUM_FIELDS];
    int nreq = 0;
    char *save = NULL;
    char *tok = strtok_r(flist_buf, ",", &save);
    int ok = 1;

    while (tok && ok) {
        tok = str_srez(tok);
        int fi = field_index(tok);
        if (fi < 0) { ok = 0; break; }
        req_fields[nreq++] = fi;
        tok = strtok_r(NULL, ",", &save);
    }
    MFREE(flist_buf);
    if (!ok) return 0;
    if (nreq == 0) return 0;

    //парс conditions
    Condition *conds = NULL; int nconds = 0;
    if (!parse_conditions(tk_vgae, 2, ntok, &conds, &nconds)) return 0;

    //сканируем
    int found = 0;
    //КТ|0
    for (lin_obj *n = db->head; n; n = n->next)
        if (re2cord_matches(&n->rec, conds, nconds)) found++;

    out("select: %d\n", found);

    // КТ\1
    for (lin_obj *n = db->head; n; n = n->next) {
        if (!re2cord_matches(&n->rec, conds, nconds)) continue;
        for (int i = 0; i < nreq; i++) {
            if (i > 0) out(" ");
            out("%s=", FIELD_NAMES[req_fields[i]]);
            out_field(&n->rec, req_fields[i]);
        }
        out("\n");
    }

    free_conditions(conds, nconds);
    return 1;
}

//delete

static int cmd_delete(DB *db, char **tk_vgae, int ntok)
{
    Condition *conds = NULL; int nconds = 0;
    if (!parse_conditions(tk_vgae, 1, ntok, &conds, &nconds)) return 0;

    int deleted = 0;
    lin_obj *prev = NULL, *cur = db->head;
    while (cur) {
        if (re2cord_matches(&cur->rec, conds, nconds)) {
            cur = db_remove_lin_obj(db, prev, cur);
            deleted++;
        } else {
            prev = cur;
            cur  = cur->next;
        }
    }
    out("delete: %d\n", deleted);
    free_conditions(conds, nconds);
    return 1;
}
//update

static int cmd_update(DB *db, char **tk_vgae, int ntok)
{
    if (ntok < 2) return 0;

    /* tk_vgae[1] = field=val,... pairs; tk_vgae[2..] = conditions */
    char *upd_buf = str_dup(tk_vgae[1]);
    if (!upd_buf) return 0;

    /* Parse update pairs */
    int upd_fi[NUM_FIELDS];
    char *upd_str_vals[NUM_FIELDS];
    int  upd_int_vals[NUM_FIELDS];
    double upd_dbl_vals[NUM_FIELDS];
    val_t upd_time_vals[NUM_FIELDS];
    Status upd_status_vals[NUM_FIELDS];
    int upd_set[NUM_FIELDS] = {0};
    int nupd = 0;
    memset(upd_str_vals, 0, sizeof(upd_str_vals));

    char *save = NULL;
    char *tok = strtok_r(upd_buf, ",", &save);
    int ok = 1;
    while (tok && ok) {
        tok = str_srez(tok);
        char *eq = strchr(tok, '=');
        if (!eq) { ok = 0; break; }
        *eq = '\0';
        char *fname = str_srez(tok);
        char *fval  = str_srez(eq+1);
        int fi = field_index(fname);
        if (fi < 0 || upd_set[fi]) { ok = 0; break; }
        upd_set[fi] = 1;
        upd_fi[nupd] = fi;

        switch (fi) {
        case FIELD_PID:      if (!parse_int(fval,    &upd_int_vals[fi]))    ok=0; break;
        case FIELD_PRIORITY: if (!parse_int(fval,    &upd_int_vals[fi]))    ok=0; break;
        case FIELD_CPU_USAGE:if (!parse_double(fval, &upd_dbl_vals[fi]))   ok=0; break;
        case FIELD_KERN_TM:  if (!parse_time(fval,   &upd_time_vals[fi]))  ok=0; break;
        case FIELD_FILE_TM:  if (!parse_time(fval,   &upd_time_vals[fi]))  ok=0; break;
        case FIELD_STATUS:   if (!parse_status(fval, &upd_status_vals[fi]))ok=0; break;
        case FIELD_NAME:
            upd_str_vals[fi] = str_dup(fval);
            if (!upd_str_vals[fi]) ok=0;
            break;
        }
        if (ok) nupd++;
        tok = strtok_r(NULL, ",", &save);
    }
    MFREE(upd_buf);

    if (!ok) {
        for (int i = 0; i < NUM_FIELDS; i++) MFREE(upd_str_vals[i]);
        return 0;
    }

    Condition *conds = NULL; int nconds = 0;
    if (!parse_conditions(tk_vgae, 2, ntok, &conds, &nconds)) {
        for (int i = 0; i < NUM_FIELDS; i++) MFREE(upd_str_vals[i]);
        return 0;
    }

    int updated = 0;
    for (lin_obj *n = db->head; n; n = n->next) {
        if (!re2cord_matches(&n->rec, conds, nconds)) continue;
        /* Apply updates */
        for (int i = 0; i < nupd; i++) {
            int fi = upd_fi[i];
            switch (fi) {
            case FIELD_PID:      n->rec.pid       = upd_int_vals[fi]; break;
            case FIELD_PRIORITY: n->rec.priority  = upd_int_vals[fi]; break;
            case FIELD_CPU_USAGE:n->rec.cpu_usage = upd_dbl_vals[fi]; break;
            case FIELD_KERN_TM:  n->rec.kern_tm   = upd_time_vals[fi]; break;
            case FIELD_FILE_TM:  n->rec.file_tm   = upd_time_vals[fi]; break;
            case FIELD_STATUS:   n->rec.status    = upd_status_vals[fi]; break;
            case FIELD_NAME:
                MFREE(n->rec.name);
                n->rec.name = str_dup(upd_str_vals[fi]);
                break;
            }
        }
        updated++;
    }
    out("update: %d\n", updated);

    for (int i = 0; i < NUM_FIELDS; i++) MFREE(upd_str_vals[i]);
    free_conditions(conds, nconds);
    return 1;
}
//uniq

static int cmd_uniq(DB *db, char **tk_vgae, int ntok)
{
    int counter_aqt=0;
    if (ntok < 2) return 0;

    char *flist_buf = str_dup(tk_vgae[1]);
    if (!flist_buf) return 0;

    int key_fields[NUM_FIELDS];
    int nkeys = 0;
    char *save = NULL;
    char *tok = strtok_r(flist_buf, ",", &save);
    int ok = 1;
    while (tok && ok) {
        tok = str_srez(tok);
        int fi = field_index(tok);
        if (fi < 0) { ok = 0; break; }
        key_fields[nkeys++] = fi;
        tok = strtok_r(NULL, ",", &save);
    }
    MFREE(flist_buf);
    if (!ok || nkeys == 0) return 0;

    /*
     * отделяем голову от хвоста... также проверяюю
     * отдельную ячейку на повторения. если есть то удаляю дупликат
     * ближайшую к концу
     */
    int removed = 0;
    lin_obj *prev = NULL, *cur = db->head;
    while (cur) {
        // проверить есть ли еще схожие после текущей
        int is_dup = 0;
        for (lin_obj *later = cur->next; later; later = later->next) {
            int same = 1;
            for (int k = 0; k < nkeys && same; k++) {
                int fi = key_fields[k];
                counter_aqt++;
                switch (fi){
                case FIELD_PID:      same = (cur->rec.pid       == later->rec.pid);      break;
                case FIELD_PRIORITY: same = (cur->rec.priority  == later->rec.priority); break;
                case FIELD_CPU_USAGE:same = (cur->rec.cpu_usage == later->rec.cpu_usage);break;
                case FIELD_KERN_TM:  same = (time_conv(cur->rec.kern_tm) == time_conv(later->rec.kern_tm)); break;
                case FIELD_FILE_TM:  same = (time_conv(cur->rec.file_tm) == time_conv(later->rec.file_tm)); break;
                case FIELD_STATUS:   same = (cur->rec.status    == later->rec.status);   break;
                case FIELD_NAME:     same = (strcmp(cur->rec.name, later->rec.name) == 0); break;
                default: same = 0;
                }
            }
            if (same) { is_dup = 1; break; }
        }
        if (is_dup) {
            cur = db_remove_lin_obj(db, prev, cur);
            removed++;
        } else {
            prev = cur;
            cur  = cur->next;
        }
    }
    printf("\n--%d\n",counter_aqt);
    out("uniq: %d\n", removed);
    return 1;
}

// обр 1 линию

static void process_line(DB *db, const char *line)
{
    //пропуск пробелов и пустых линий
    const char *p = line;
    while (*p && isspace((unsigned char)*p)) p++;
    if (!*p) return;

    //ошибка(первые 20 символов)
    char err_prefix[21] = {0};
    strncpy(err_prefix, line, 20);
    //удаление префикса
    for (int i = 0; err_prefix[i]; i++)
        if ((err_prefix[i]=='\n') ||(err_prefix[i]=='\r')) {
            err_prefix[i] = '\0'; break;
        }

    int ntok = 0;
    char **tk_vgae = split_tk_vgae(p, &ntok);
    if (!tk_vgae || ntok == 0) {
        if (tk_vgae) MFREE(tk_vgae);
        return;
    }

    int ok = 0;
    const char *cmd = tk_vgae[0];// napominaniye //tk_vgae[1] = field list, tk_vgae[2..] = conditions
    //(!parse_condition(tk_vgae[start+i], &conds[parsed])) -- free parsed
    //new
    int timer_field=(-1);
    for(int i=0;i<5;i++){
        if(strcmp(cmd,timed_c[i])==0){
            timer_field=i;
            break;
            }
        }
    double start_command=timer_new();
    //new end

    if (strcmp(cmd, "insert") == 0) {
        if (ntok < 2) ok = 0;
        else {
            /* переделка линии в массив с аргументами
             * коомы не должны попадать
             * поэтому пытаемся переискать первый аргумент в линии */
            const char *rest = p;
            //skip insert keyword <----------------------------------------------------
            while (*rest &&(!isspace((unsigned char)*rest))) {
                rest++;
            }
            while (*rest &&( isspace((unsigned char)*rest))) {
                rest++;
            }
            //what are you doing here
            //
            //je ignore insert
            if (*rest)ok =cmd_insert(db,rest);
        }
    }
    else if(strcmp(cmd,"select")==0){
        ok =cmd_select(db, tk_vgae, ntok);
    }
    else if(strcmp(cmd,"delete")==0){
        ok =cmd_delete(db, tk_vgae, ntok);
    }
    else if(strcmp(cmd,"update")==0){
        if (ntok<2) ok= 0;
        else{
            /* также как с инсеротом
             * tk_vgae[1] это field=val list а tk_vgae[2] и тд это условия
             * обязательно надо использовать split_tk_vgae
             * field=val без пробелов (комменты не в счет).
*/
            ok = cmd_update(db,tk_vgae,ntok);
        }}
    else if(strcmp(cmd,"uniq")==0){
        ok=cmd_uniq(db,tk_vgae, ntok);
    }
    else{
        ok= 0;
    }
    //new
    double command_proshlo=timer_new()-start_command;
    if(timer_field>=0){
        sec[timer_field]=sec[timer_field]+command_proshlo;
        ccalls[timer_field]++;
        }
    
    //new end
    if (!ok){
        out("--!!incorrect %s\n",err_prefix);
    }

    // осв 1 \ 0 - буффер
    if (ntok>0){
        MFREE(tk_vgae[0]);
    }//vous avez oubli? de memfree[1,2,3...etc]
    MFREE(tk_vgae);
}

// очистка бд

static void db_destroy(DB *db)
{
    lin_obj *n = db->head;
    while (n) {
        lin_obj *next = n->next;
        re2cord_free_fields(&n->rec);
        MFREE(n);
        n = next;
    }
    db->head = db->tail = NULL;
    db->count = 0;
}

// getline
static long brkgf2_getline(char **lineptr, size_t *n, FILE *stream) {
    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = MALLOCA(*n); // <-- сбрил
        if (*lineptr == NULL) return -1;
    }

    size_t pos = 0;
    int c;

    while ((c = fgetc(stream)) != EOF) {
        if (pos + 1 >= *n) {
            size_t new_cap = *n * 2;
            char *new_ptr = REALLOCA(*lineptr, new_cap); // <-- поменял тут
            if (new_ptr == NULL) return -1;
            *lineptr = new_ptr;
            *n = new_cap;
        }
        (*lineptr)[pos++] = (char)c;
        if (c == '\n') break;
    }

    if (pos == 0 && c == EOF) return -1;

    (*lineptr)[pos] = '\0';
    return (long)pos;
}


int main(int argc, char *argv[])
{
    //new
    //ElapsedMicroseconds.QuadPart *= 1000000;
    //ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    //if (!QueryPerformanceCounter(&timer))
    if (!QueryPerformanceFrequency(&timer))
    {
        //winrt::throw_last_error();C++ only
        return 1;
    }
    double summ_start=timer_new();
    //new end
    
    //SetConsoleOutputCP(1251);
    //SetConsoleCP(1251);
    //setlocale(LC_ALL, "Russian");
    ouputf=fopen("output.txt","w");// w<--- r wr
    if (!ouputf){
        fprintf(stderr,"ouput failed to open\n");//stderr-тип данных для ошибок
        return 1;
    }

    DB db;
    db_init(&db);
    FILE *inputf=NULL;
    if (argc>=2) {
        inputf=fopen(argv[1],"r");
        if(!inputf){
            fprintf(stderr,"doesnt worked: %s\n", argv[1]);
            fclose(ouputf);
            return 1;
        }
    } else {
        inputf = stdin;
    }

    char *line = NULL;
    size_t limit = 0;
    long len; //je le remplace votre demande
    //ssize_t len;

    while ((len = brkgf2_getline(&line, &limit, inputf)) != -1) {//faites quelque chose avec cette putain de fonс
        //suppression de toutes les lignes (\r ou \n)
        line[strcspn(line, "\r\n")] = '\0';//вместо длинного while (f->s)

        process_line(&db, line);
    }
    if (line)MFREE(line);
    if (inputf!=stdin)fclose(inputf);
    fclose(ouputf);
    db_destroy(&db);
    //memstat.txt
    //je libere et supprime le superflu
    FILE *ms=fopen("memstat.txt","w");// можно открыть в режиме trunc чтобы удалять весь файл с открытием
    if(ms){
        fprintf(ms,"malloc:%ld\n",malloc_cntv);
        fprintf(ms,"realloc:%ld\n",realloc_cntv);
        fprintf(ms,"calloc:%ld\n",calloc_cntv);
        fprintf(ms,"free:%ld\n",mfree_cntv);
        fclose(ms);
    }else{
        printf("\n--De four ms falte3--\n");
    }
    
    //new
    double summ_of_secc=timer_new()-summ_start;
    //fprintf(stderr,"ouput failed to open\n");//stderr-тип данных для ошибок
    fprintf(stderr,"\n\ntotal time:\n");
    for(int i=0;i<5;i++){
        double ms_summ=sec[i]*1000;
        double ms_averaga;
        if(ccalls[i]!=0){
            ms_averaga=ms_summ/(double)ccalls[i];
        }else{
            ms_averaga=0;
        }
        // fprintf(fp, "%-10s | ID: %04d | Р—Р°СЂРїР»Р°С‚Р°: %10.2f СЂСѓР±.\n", name, id, salary);
        fprintf(stderr,"%-10s %10llu %14.3f %14.6f\n",timed_c[i],ccalls[i],ms_summ,ms_averaga);
        
    }
    fprintf(stderr, "summ time: %.3f ms\n",summ_of_secc*1000);
    //new end
    return 0;
}
