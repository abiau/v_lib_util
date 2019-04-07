#ifndef __V__HEADER__
#define __V__HEADER__
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************/
//  header files;
/***********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/time.h>
#include <limits.h> 
#include <ctype.h>
/* network. */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
/* file. */
#include <sys/stat.h>
#include <libgen.h> 
#include <dirent.h> 
/* trace. */
#include <execinfo.h> 

/***********************************************/
//  debug;
/***********************************************/

#define VEXIT()      do{printf("EXIT_FAILURE: %s:%d\n", __func__, __LINE__);exit(EXIT_FAILURE);}while(0)
#define NOUSE(x)     (void)(x)
#define VTT(x)       do{printf("<VTT> %s=%d\n", #x, x);}while(0)
#define VENO         do{printf("%s:%d| eno=%d|%s\n", __func__, __LINE__, errno, strerror(errno));}while(0)
#define DDD          do{printf("%s():%d\n", __func__, __LINE__);}while(0)
#define WARN_IF(EXP) do{if(EXP) fprintf (stderr, "Warning: " #EXP "\n"); }while(0) 
#define vmax(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define vmin(a,b)     ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

/***********************************************/
//  enum definition;
/***********************************************/

typedef enum {
	________________DATA_TYPE = 1100,
	CH,
	INT,
	LONG,
	FLOAT,
	PTR,
	STR,
	CSTR,
	BYTE,
	________________ORDER = 1200,
	DLR,
	LDR,
	LRD,
	FROM_L,
	FROM_R,
	________________SETTING = 1300,
	SET_eql_fn,
	SET_cmp_fn,
	SET_pnt_fn,
	SET_todo_fn,
	SET_del_fn,
	________________TIME = 2000,
	SEC,
	US,
	MS,
	________________FILE = 3000,
	SET_log_rotate_size,
	SET_log_rotate_time,
	SET_log_fmt,
	SET_log_dir,
	SET_log_file,
	SET_log_output,
	________________NETWORK = 5000,
	TCP,
	UDP,
	SET_nonblock,
	SET_reuseaddr,
	________________END,
} SETTING_OPTION;

typedef enum {
#ifndef OKAY
	OKAY = 0,
	FAIL = -1,
#endif
	____ERROR____  = -1000,
	VERROR_TIMEOUT = -1001,
	VERROR_END,
} VERROR;

/***********************************************/
//  Definition;
/***********************************************/

#define F_OTHERS  (0x0000)
#define F_FILE    (0x0001)
#define F_DIR     (0x0002)
#define F_LINK    (0x0004)
#define F_SOCK    (0x0008)

#define L_STDOUT  (0x0001)
#define L_STDERR  (0x0002)
#define L_FILE    (0x0004)

/***********************************************/
//  Macro;
/***********************************************/

#define vdata(X) \
	    _Generic((X) \
				, char: data_ch, unsigned char: data_ch \
				, n32t: data_int, u32t: data_int \
				, n64t: data_long, u64t: data_long, size_t: data_long \
				, float: data_float, double: data_float \
				, const char*: data_cstr, const unsigned char*: data_cstr \
				, void*: data_ptr \
				, default: data_ptr \
				)(X)
#define vdataS(X) \
	    _Generic((X) \
				, char*: data_newstr,        unsigned char*: data_newstr \
				, const char*: data_newcstr, const unsigned char*: data_newcstr \
				, default: data_null \
				)(X)

#define vnull        data_null() 
#define vdataB(X,Y)  do{data_newbyte(X,Y);}while(0)
#define vdataDel(X)  do{data_delbyte(X);}while(0)

/***********************************************/
//  type definition;
/***********************************************/

typedef unsigned long long  u64t;
typedef          long long  n64t;
typedef unsigned int        u32t;
typedef          int        n32t;
typedef unsigned char       uchar;
typedef pthread_mutex_t     vmutex_t;
typedef pthread_t           vthread_t;
typedef struct sockaddr_in  saddrin_t;

typedef struct {
	char exe[1024];
	char func[1024];
	char offset[1024];
} vtrace_t;

typedef struct {
	u64t  epoch;
	short Y;
	char  M;
	char  D;
	char  h;
	char  m;
	char  s;
	int   u3;
	int   u6;
} vtime_t;

typedef struct {
	int        port;
	char       ip[32];
	saddrin_t  addr;
} vaddr_t;

typedef struct {
	union {
		int type;
		int exist;
	};
	int alloc;
	union {
		double F;
		char C;
		n32t I;
		n64t L;
		void* P;
		char* S;
		char* B;
	};
} vdata_t;

typedef int   (*idd_ft)(vdata_t item, vdata_t value);
typedef void  (*vdd_ft)(vdata_t item, vdata_t value);
typedef void  (*vd_ft)(vdata_t item);
typedef void* (*pd_ft)(vdata_t value);
typedef void* (*pp_ft)(void* arg);



/***********************************************/
//  structure definition;
/***********************************************/

typedef struct {
	idd_ft eql;
	idd_ft cmp;
	idd_ft pnt;
	vd_ft del;
} OP;

typedef struct {
	n32t memSize;
	n32t memCnt;
} Info;

typedef struct Node {
	struct Node* L;
	struct Node* R;
	vdata_t key;
	vdata_t value;
} Node;

typedef struct {
	int size;
	Node* mostL;
	Node* mostR;
} Chain;

typedef struct {
	Chain* chain;
	OP* op;
} List, Queue, Dict;

typedef struct {
	int  type;     /* TCP/UDP */
	int  skt;
	int  bindPort;
} Net;

/***********************************************/
//  API definition;
/***********************************************/

/* vcom.c */


#ifndef VSAAS_NGX 
void vnonstop(void);
void vmsleep(u64t ms);
int vrandom(int n);
#endif
void vendl();
Info vInfo(void);
void* vnew (size_t size);
void vdel (void* p, size_t size);

u64t vnow (int tsUnit);
u64t vnow_us (void);
u64t vnow_ms (void);
u64t vnow_sec (void);

/********** [data] **********/

//int*    int_new(int n);
//void    int_del(int* pInt);

vdata_t data_null();
vdata_t data_ch(char x);
vdata_t data_int(n32t x);
vdata_t data_long(n64t x);
vdata_t data_float(double x);
vdata_t data_ptr(void* x);
vdata_t data_cstr(const char* x);
vdata_t data_newbyte(char* x, int size);
vdata_t data_newstr(char* x);
vdata_t data_newcstr(const char* x);
void    data_delbyte(vdata_t d);

OP*     OP_new();
void    OP_del(OP* op);
void    OP_ctl(OP* op, int opt, void* ptr);

Node*   Node_new(vdata_t value);
void    Node_del(Node* node);

/********** [Chain] **********/

Chain*  Chain_new();
void    Chain_del(Chain* C);
vdata_t Chain_pop(Chain* C, Node* node);
void    Chain_lpush(Chain* C, Node* p, Node* node);
void    Chain_rpush(Chain* C, Node* p, Node* node);
void    Chain_traversalFrom(int order, Node* node, idd_ft todo, vdata_t arg);
Node*   Chain_sortFrom(int order, Node* node, idd_ft cmp, vdata_t value);
Node*   Chain_searchFrom(int order, Node* node, idd_ft cmp, vdata_t value);

/********** [List] **********/

List*   List_new();
void    List_del(List* L);
void    List_ctl(List* L, int opt, void* ptr);
int     List_size(List* L);
void    List_clear(List* L);

void    List_insert(List* L, vdata_t value);
vdata_t List_search(List* L, vdata_t value);
vdata_t List_obtain(List* L, vdata_t value);
void    List_remove(List* L, vdata_t value);
void    List_print(List* L);
void    List_traversal(List* L, idd_ft todo, vdata_t arg);

void    List_rinsert(List* L, vdata_t value);
vdata_t List_rsearch(List* L, vdata_t value);
vdata_t List_robtain(List* L, vdata_t value);
void    List_rremove(List* L, vdata_t value);
void    List_rprint(List* L);

/********** [Queue] **********/

Queue*  Queue_new();
void    Queue_del(Queue* Q);
void    Queue_ctl(Queue* Q, int opt, void* ptr);
void    Queue_clear(Queue* Q);
void    Queue_push(Queue* Q, vdata_t value);
vdata_t Queue_pop(Queue* Q);

/********** [Network] **********/

vaddr_t toVaddr(saddrin_t addr);

Net*    vnet_server(int type, int bindPort, int maxSession);
Net*    vnet_client(int type);
void    vnet_del(Net* N);
int     vnet_accept(Net* N, vaddr_t* pAddr);
int     vnet_connect(Net* N, char* ip, int port);
int     vnet_close(int skt);
int     vnet_write(int skt,  char* buf, int bufLen, u64t timeout_ms);
int     vnet_read(int skt,  char* buf, int bufLen, u64t timeout_ms);
//int vnet_sendto(int skt, char* buf, int bufLen, char* ip, int port);
//int vnet_recvfrom(int skt, char* buf, int bufLen, vaddr_t* pAddr);


/********** [Time] **********/

vtime_t  vtime_now();
vtime_t  vtime_epoch2vt(u64t epoch);
u64t     vtime_vt2epoch(vtime_t vt);

vtime_t  vtime_str2vt(char* str, char* fmt);
char*    vtime_vt2str(vtime_t vt, char* fmt);

char*    vtime_epoch2str(u64t epoch, char* fmt);
u64t     vtime_str2epoch(char*str, char* fmt);

/********** [Parse] **********/

char*    vstr_hasToken(char* src, char* token);
char*    vstr_findToken(char* src, char* token, char bInclude);
char*    vstr_rfindToken(char* src, char* token, char bInclude);
char*    vstr_cutBits(char* src, u32t nBits);
char*    vstr_cutWords(char* src, char* start, char* end, char bInclude);
char*    vstr_rcutWords(char* src, char* start, char* end, char bInclude);


/***********************************************/
//  TODO;
/***********************************************/

/********** [Trace] **********/


void      vtrace();
void      vtrace_file(char* file);
vtrace_t  vtrace_parseSymbols(char* bt_symbols);
void      vtrace_shell(char* exe, char* file);

/********** [Path] **********/

int       vpath_exist(const char* path);
int       vpath_type(const char* path);
n64t      vpath_size(const char* path);
int       vpath_isfile(const char* path);
int       vpath_isdir(const char* path);
int       vpath_mkdir(const char* dir);

/********** [Dir] **********/

#if 0
typedef struct vdir_t{
	int type;
	char name[1024];
} vdir_t;

typedef struct {
	int sum;
	char dir[2048];
	vdir_t* vdir;
	int it;
} Dir;

Dir* vdir_new(const char* dir);
Dir* vdir_refresh(Dir* D);
void vdir_del(Dir* D);
void vdir_rewind(Dir* D);
vdir_t* vdir_foreach(Dir* D, int type);

#endif

/********** [Routine] **********/

typedef struct {
	vthread_t pid;
	pp_ft     func;
	void*     arg;
} Routine;

Routine* Routine_new();
void     Routine_del(Routine* R);
int      Routine_detach(Routine* R);
int      Routine_join(Routine* R);

/********** [Log] **********/

typedef struct {
	FILE*     fp;
	FILE*     fp_bak;
	int       output;
	int       iRotate;
	vmutex_t  mutex;
	u64t      rotateSize;
	u64t      rotateTime;
	u64t      epoch;
	char      dir[192];
	char      file[64];
	char      path[256];
	char      fmt[64];
	char      buf[4096];
} Log;

const char* vclr_char(char c);
void        vclr_switch(int enable);
char*       cword(char color, char* msg);
void        cprintf(char color, char* msg, ...);

Log*  Log_new();
void  Log_del(Log* L);
void  Log_ctl(Log* L, int opt, vdata_t value);
char* Log_premsg (char* buf, int len, u64t epoch, char* fmt, const char* func, int line);
void  Log_print__ (Log* L, const char* func, int line, char* msg, ...);

int   Log_NumberOfDigits (const char* str);
u64t  Log_fmt2time (const char* UYMWDhms);
u64t  Log_fmt2byte (const char* GMKB);

#define Log_print(L, msg, ...) Log_print__(L, __func__, __LINE__, msg, ##__VA_ARGS__)


/***********************************************/
//  sample;
/***********************************************/

void sample_queue_int();
void sample_queue_void();
void sample_list();

void sample_dir();
void sample_cut();
void sample_time();
void sample_bt();

void program_parse_publish_uid(int argc, char* argv[]);
void program_parse_framelog(int argc, char* argv[]);
void program_parse_utdlog(int argc, char* argv[]);

/***********************************************/
//  END;
/***********************************************/

#ifdef __cplusplus
}
#endif
#endif

