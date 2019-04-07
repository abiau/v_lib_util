#include "v.h"

/***********************************************/
//  global variable;
/***********************************************/

static Info g_Info = {0};
static int  g_LogColor = 1;

/***********************************************/
//  static function;
/***********************************************/

/***********************************************/
//  color;
/***********************************************/

#if 0
const char* VC[200];

void init_vcolor()
{
	VC[0]  =  "\033[m";
	VC[30] =  "\033[0;30m";
	VC[31] =  "\033[0;31m";
	VC[32] =  "\033[0;32m";
	VC[33] =  "\033[0;33m";
	VC[34] =  "\033[0;34m";
	VC[35] =  "\033[0;35m";
	VC[36] =  "\033[0;36m";
	VC[37] =  "\033[0;37m";
	VC[39] =  "\033[0;39m";
	VC[130] = "\033[1;30m";
	VC[131] = "\033[1;31m";
	VC[132] = "\033[1;32m";
	VC[133] = "\033[1;33m";
	VC[134] = "\033[1;34m";
	VC[135] = "\033[1;35m";
	VC[136] = "\033[1;36m";
	VC[137] = "\033[1;37m";
	VC[140] = "\033[1;40m";
	VC[141] = "\033[1;41m";
	VC[142] = "\033[1;42m";
	VC[143] = "\033[1;43m";
	VC[144] = "\033[1;44m";
	VC[145] = "\033[1;45m";
	VC[146] = "\033[1;46m";
	VC[147] = "\033[1;47m";
}
#endif

char* cword(char color, char* msg)
{
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));

	snprintf(buf, sizeof(buf), "%s%s%s", vclr_char(color), msg, vclr_char('e'));
	return buf;
}

void cprintf(char color, char* msg, ...)
{
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));

	char* p = buf;
	snprintf(buf, sizeof(buf), "%s", vclr_char(color));
	p += strlen(vclr_char(color));

	va_list ap;
	va_start (ap, msg);
	vsprintf(p, msg, ap);
	va_end (ap);

	int len = strlen(buf);
	int CheckNewLine = (len>1 && *(buf+len-1)=='\n');

	if (CheckNewLine)
	{
		p = (buf+len-1);
		snprintf(p, (buf + sizeof(buf) - p -1 ), "%s", vclr_char('e'));
		len = strlen(buf);
		*(buf+len) = '\n';
		*(buf+len+1) = '\0';
	}
	else
	{
		p = (buf+len);
		snprintf(p, (buf + sizeof(buf) - p ), "%s", vclr_char('e'));
	}

	printf("%s", buf);
}

const char* vclr_char(char c)
{
	if (!g_LogColor)
	{
		return "";
	}

	switch (c)
	{
	case 'e':
	case 'E':
		return "\033[m";
	case 'd':
		return "\033[0;30m";
	case 'r':
		return "\033[0;31m";
	case 'g':
		return "\033[0;32m";
	case 'y':
		return "\033[0;33m";
	case 'b':
		return "\033[0;34m";
	case 'm':
		return "\033[0;35m";
	case 'c':
		return "\033[0;36m";
	case 'w':
		return "\033[0;37m";
	case 'o':
		return "\033[0;39m";
	case 'D':
		return "\033[1;30m";
	case 'R':
		return "\033[1;31m";
	case 'G':
		return "\033[1;32m";
	case 'Y':
		return "\033[1;33m";
	case 'B':
		return "\033[1;34m";
	case 'M':
		return "\033[1;35m";
	case 'C':
		return "\033[1;36m";
	case 'W':
		return "\033[1;37m";
	case '0':
		return "\033[1;40m";
	case '1':
		return "\033[1;41m";
	case '2':
		return "\033[1;42m";
	case '3':
		return "\033[1;43m";
	case '4':
		return "\033[1;44m";
	case '5':
		return "\033[1;45m";
	case '6':
		return "\033[1;46m";
	case '7':
		return "\033[1;47m";
	default:
		return "";
	}
}


/***********************************************/
//  open function;
/***********************************************/

void vendl()
{
	printf("%c", '\n');
}

void vusleep(u64t us)
{
	int sec = us/1000000;
	sleep(sec);
	usleep(us);
}

#ifndef VSAAS_NGX 
void vnonstop(void)
{
	while(1)
	{
		sleep(1);
	}
}

void vmsleep(u64t ms)
{
	int sec = ms/1000;
	int us = (ms%1000)*1000;
	sleep(sec);
	usleep(us);
}

int vrandom(int n)
{
	static int bInitSrandom=0;
	if (!bInitSrandom)
	{
		srandom(time(NULL));
		bInitSrandom=1;
	}
	return (random()%n);
}
#endif

Info vInfo(void)
{
	return g_Info;
}

void* vnew (size_t size)
{
	void* p = malloc (size); 
	if (!p) goto _fail_;

	__sync_add_and_fetch (&g_Info.memCnt, 1);
	__sync_add_and_fetch (&g_Info.memSize, size);
	memset(p, 0, size);
	return p;

_fail_:
	exit (EXIT_FAILURE);
}

void vdel (void* p, size_t size)
{
	__sync_add_and_fetch (&g_Info.memCnt, -1);
	__sync_add_and_fetch (&g_Info.memSize, -(size));
	free (p);
	return ;
}

/***********************************************/
//  Time;
/***********************************************/

u64t vnow (int tsUnit)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	u64t us = (tv.tv_sec)*1000000 + (tv.tv_usec) ;
	u64t ms = us/1000 ;
	u64t sec = us/1000000 ;

	switch(tsUnit)
	{
	case US:
		return us;
	case MS:
		return ms;
	case SEC:
		return sec;
	default:
		return ms;
	}
}

u64t vnow_us (void)
{
	return vnow (US);
}

u64t vnow_ms (void)
{
	return vnow (MS);
}

u64t vnow_sec (void)
{
	return vnow (SEC);
}


vtime_t vtime_now()
{
    return vtime_epoch2vt(vnow(US));
}

vtime_t vtime_str2vt (char* str, char* fmt)
{
    vtime_t vt_now = vtime_now();
    vtime_t vt;
	memset(&vt, 0, sizeof(vtime_t));
	vt.Y = vt_now.Y;
	vt.M = vt_now.M;
	vt.D = vt_now.D;
	
	char* pFmt = fmt;
	char* pStr = str;

	while (*pFmt != '\0')
	{
		switch(*pFmt)
		{
		case 'Y':
			vt.Y = atoi( vstr_cutBits(pStr, 4) );
			pStr += 4;
			break;
		case 'M':
			vt.M = atoi( vstr_cutBits(pStr, 2) );
			pStr += 2;
			break;
		case 'D':
			vt.D = atoi( vstr_cutBits(pStr, 2) );
			pStr += 2;
			break;
		case 'h':
			vt.h = atoi( vstr_cutBits(pStr, 2) );
			pStr += 2;
			break;
		case 'm':
			vt.m = atoi( vstr_cutBits(pStr, 2) );
			pStr += 2;
			break;
		case 's':
			vt.s = atoi( vstr_cutBits(pStr, 2) );
			pStr += 2;
			break;
		case 'u':
			if (*(pFmt+1)=='3')
			{
				vt.u3 = atoi( vstr_cutBits(pStr, 3) );
				vt.u6 = vt.u3 * 1000;
				pStr += 3;
				pFmt++;
			}
			else if (*(pFmt+1)=='6')
			{
				vt.u6 = atoi( vstr_cutBits(pStr, 6) );
				vt.u3 = vt.u6 / 1000;
				pStr += 6;
				pFmt++;
			}
			break;
		default:
			pStr++;
			break;
		}
		pFmt++;
	}
	vt.epoch = vtime_vt2epoch(vt);
	return vt;
}

char* vtime_vt2str(vtime_t vt, char* fmt)
{
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));

	char* pEnd=buf+sizeof(buf);
	char* p=buf;
	char* pFmt=fmt;

	while (*pFmt != '\0')
	{
		switch(*pFmt)
		{
		case 'Y':
			snprintf(p, (pEnd-p), "%04d", vt.Y);
			p += 4;
			break;
		case 'M':
			snprintf(p, (pEnd-p), "%02d", vt.M);
			p += 2;
			break;
		case 'D':
			snprintf(p, (pEnd-p), "%02d", vt.D);
			p += 2;
			break;
		case 'h':
			snprintf(p, (pEnd-p), "%02d", vt.h);
			p += 2;
			break;
		case 'm':
			snprintf(p, (pEnd-p), "%02d", vt.m);
			p += 2;
			break;
		case 's':
			snprintf(p, (pEnd-p), "%02d", vt.s);
			p += 2;
			break;
		case 'u':
			if (*(pFmt+1)=='3')
			{
				snprintf(p, (pEnd-p), "%03d", vt.u3);
				pFmt++;
				p += 3;
			}
			else if (*(pFmt+1)=='6')
			{
				snprintf(p, (pEnd-p), "%06d", vt.u6);
				pFmt++;
				p += 6;
			}
			break;
		default:
			snprintf(p, (pEnd-p), "%c", *pFmt);
			p++;
			break;
		}
		pFmt++;
	}
	return buf;
}


static unsigned short days[4][12] =
{
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};

u64t vtime_vt2epoch(vtime_t vt)
{
	/* We calculate epoch start from 2000, and 946684800 is total sec from 1970/01/01 ~ 2000/01/01 */
    u64t second = vt.s;     // 0-59
    u64t minute = vt.m;     // 0-59
    u64t hour   = vt.h;     // 0-23
    u64t day    = vt.D-1;   // 0-30
    u64t month  = vt.M-1;   // 0-11
    u64t year   = vt.Y%100; // 0-99
	
	u64t epoch;
	epoch = (((year/4*(365*4+1) + days[year%4][month]+day)*24 + hour)*60 + minute)*60 + second + 946684800;
	epoch = (epoch*1000000) + vt.u6;

	return epoch;
}

vtime_t vtime_epoch2vt (u64t epoch)
{
	/* We calculate epoch start from 2000, and 946684800 is total sec from 1970/01/01 ~ 2000/01/01 */
	vtime_t  vt;
	memset(&vt, 0, sizeof(vtime_t));
	
	if (epoch>1000000000000000) {
		/* (16 bits) micro second. */
		epoch -= 946684800000000;
		vt.epoch = epoch;
		vt.u6 = epoch % 1000000;
		vt.u3 = vt.u6 / 1000;
	} else if (epoch>1000000000000) {
		/* (13 bits) milli second. */
		epoch -= 946684800000;
		vt.epoch = epoch * 1000;
		vt.u3 = epoch % 1000;
		vt.u6 = vt.u3 * 1000;
	} else {
		/* (10 bits) second. */
		epoch -= 946684800;
		vt.epoch = epoch * 1000000;
		vt.u3 = 0;
		vt.u6 = 0;
	}
	
	u64t T = vt.epoch / 1000000;

    int second = T % 60; T /= 60;
    int minute = T % 60; T /= 60;
    int hour   = T % 24; T /= 24;

	int years = T/(365*4+1)*4; T %= (365*4+1);

	int year;
	for (year=3; year>0; year--)
	{
		if (T >= days[year][0])
			break;
	}

	int month;
	for (month=11; month>0; month--)
	{
		if (T >= days[year][month])
			break;
	}

    vt.Y = years+year+2000;
    vt.M = month+1;
    vt.D = T-days[year][month]+1;
	vt.s = second;
	vt.m = minute;
	vt.h = hour;

	return vt;
}

char* vtime_epoch2str(u64t epoch, char* fmt)
{
	vtime_t vt = vtime_epoch2vt(epoch);
	return vtime_vt2str(vt, fmt);

}

u64t vtime_str2epoch(char*str, char* fmt)
{
	vtime_t vt = vtime_str2vt(str, fmt);
	return vtime_vt2epoch(vt);
}

/***********************************************/
//  Trace;
/***********************************************/

char* vstr_hasToken(char* src, char* token)
{
	return vstr_findToken(src, token, 1);
}

char* vstr_cutBits(char* src, u32t nBits)
{
	if (!src) {return NULL;}
	
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));
	
	nBits = (nBits<=strlen(src)) ? nBits : strlen(src);
	memcpy (buf, src, nBits);
	return buf;
}

char* vstr_findToken(char* src, char* token, char bInclude)
{
	if (!src || !token) {return NULL;}

	int   srcSize = strlen(src);
	int   tokenSize = strlen(token);
	char* p = src;
	char* t = token;

	for (; p != (src + srcSize) ; p++)
	{
		if (*p == *t)
		{
			t++;
			if (t== (token + tokenSize)) // if(*t=='\0')
			{
				if (bInclude)
				{
					p = p - (tokenSize - 1);
					return p;
				}
				else
				{
					return p+1;
				}
			}
		}
		else
		{
			t=token;
		}
	}
	return NULL;
}

char* vstr_rfindToken(char* src, char* token, char bInclude)
{
	if (!src || !token) {return NULL;}
	
	int   srcSize = strlen(src);
	int   tokenSize = strlen(token);
	char* p = src + srcSize-1;
	char* t = token + tokenSize-1;

	for (; p!=(src-1) ; p--)
	{
		if (*p == *t)
		{
			t--;
			if (t==(token-1))
			{
				if (bInclude)
				{
					return p;
				}
				else
				{
					p = p + tokenSize;
					return p;
				}
			}
		}
		else
		{
			t=token+strlen(token)-1;
		}
	}
	return NULL;
}

char* vstr_cutWords(char* src, char* start, char* end, char bInclude)
{
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));
	
	char* p1 = (start==NULL) ? src : vstr_findToken(src, start, bInclude) ;
	if (!p1)
	{
		return NULL;
	}

	char* p2 = (end==NULL) ? src+strlen(src) : vstr_findToken(p1, end, !bInclude);
	if (!p2)
	{
		return NULL;
	}
	memcpy(buf, p1, (p2-p1));
	return buf;
}

char* vstr_rcutWords(char* src, char* start, char* end, char bInclude)
{
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));
	
	char* p1 = (start==NULL) ? src : vstr_rfindToken(src, start, bInclude) ;
	if (!p1)
	{
		return NULL;
	}

	char* p2 = (end==NULL) ? src+strlen(src) : vstr_rfindToken(p1, end, !bInclude);
	if (!p2)
	{
		return NULL;
	}
	memcpy(buf, p1, (p2-p1));
	return buf;
}


vtrace_t vtrace_parseSymbols(char* bt_symbols)
{
	vtrace_t trace;
	memset(&trace, 0, sizeof(vtrace_t));
	
	char* pExe = vstr_cutWords(bt_symbols, NULL, "(", 0);
	if (pExe)
	{
		snprintf(trace.exe, sizeof(trace.exe), "%s", pExe);
	}

	char* pFunc = vstr_cutWords(bt_symbols, "(", "+", 0);
	if (pFunc)
	{
		snprintf(trace.func, sizeof(trace.func), "%s", pFunc);
	}

	char* pOffset = vstr_cutWords(bt_symbols, "[", "]", 0);
	if (pOffset)
	{
		snprintf(trace.offset, sizeof(trace.offset), "%s", pOffset);
	}

	return trace;
}

void vtrace_file(char* file)
{
	FILE* fp = fopen(file, "a+");
	if (!fp) { return ; }

    void*   bt[1024];
    int     bt_size;
    char**  bt_syms;
    int     i;

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    
	u64t ms = vnow_ms();
	fprintf(fp, "%s\n\n", vtime_epoch2str(ms, "YMD h:m:s.u3"));
    for (i=1; i<bt_size; i++)
	{
		fprintf(fp, "  %s\n", bt_syms[i]);
	}
	fprintf(fp, "\n\n\n");
	fclose(fp);
	free(bt_syms);
	return;
}

void vtrace_shell(char* exe, char* file)
{
	static __thread int bUsed=0;
	if (bUsed++>=1) {return;}

	FILE* fp = fopen(file, "a+");
	if (!fp) { return ; }

    void*   bt[1024];
    int     bt_size;
    char**  bt_syms;
    int     i;

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    
	u64t ms = vnow_ms();
	fprintf(fp, "%s\n\n", vtime_epoch2str(ms, "YMD h:m:s.u3"));
    for (i=1; i<bt_size; i++)
	{
		//fprintf(fp, "  %s\n", bt_syms[i]);
		vtrace_t vbt = vtrace_parseSymbols(bt_syms[i]);
		fprintf (fp, "%20s()  ", vbt.func);
		fflush(fp);
		
		char cmd[4096];
		snprintf(cmd, sizeof(cmd), "addr2line -e %s %s >> %s", exe, vbt.offset, file);
		if(system(cmd)){/*Ignore return value.*/};
	}
	fprintf(fp, "\n\n\n");
	fclose(fp);
	free(bt_syms);
	return;
}

void vtrace ()
{
	/* FIXMEE: Save to file not yet. */

    void*   bt[1024];
    int     bt_size;
    char**  bt_syms;
    int     i;
	//char    cmd[1024];

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    
    for (i=1; i<bt_size; i++)
	{
#if 1
		printf ("%s\n", bt_syms[i]);
#else
		vtrace_t vt = vtrace_parseSymbols(bt_syms[i]);
		//printf ("EXE: %s\nFUNC: %s\nLINE: %s\n", vt.exe, vt.func, vt.offset);
		printf ("%20s()  ", vt.func);
		fflush(stdout);
		snprintf(cmd, sizeof(cmd), "addr2line -e %s %s", vt.exe, vt.offset);
		system(cmd);
#endif
	}
	free(bt_syms);
}


/***********************************************/
//  File;
/***********************************************/


int vpath_mv (const char *oldPath, const char* newPath)
{
	return rename (oldPath, newPath);
}

n64t vpath_size (const char* path)
{
	struct stat s;
	memset (&s, 0, sizeof(struct stat));
	stat (path, &s);
	
	return s.st_size;
}

int vpath_exist (const char* path)
{
	struct stat s;
	return (stat(path , &s)==0)?1:0 ;
}

int vpath_type (const char* path)
{
	struct stat s;
	stat(path, &s);
	if (S_ISREG(s.st_mode))
	{
		return F_FILE;
	}
	else if (S_ISDIR(s.st_mode))
	{
		return F_DIR;
	}
	else if (S_ISLNK(s.st_mode))
	{
		return F_LINK;
	}
	else if (S_ISSOCK(s.st_mode))
	{
		return F_SOCK;
	}
	else
	{
		return F_OTHERS;
	}
}

int vpath_isfile (const char* path)
{
	struct stat s;
	stat(path, &s);
	return (S_ISREG(s.st_mode)) ? 1 : 0;
}

int vpath_isdir (const char* path)
{
	struct stat s;
	stat(path, &s);
	return (S_ISDIR(s.st_mode)) ? 1 : 0;
}

int vpath_mkdir(const char* dir)
{
	mode_t mode = (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char *q, *r = NULL, *path = NULL, *up = NULL;
	int ret;

	ret = -1;
	if (strcmp(dir, ".") == 0 || strcmp(dir, "/") == 0)
		return (0);

	if ((path = strdup(dir)) == NULL)
		exit(1);

	if ((q = strdup(dir)) == NULL)
		exit(1);

	if ((r = dirname(q)) == NULL)
		goto out;

	if ((up = strdup(r)) == NULL)
		exit(1);

	if ((vpath_mkdir(up) == -1) && (errno != EEXIST))
		goto out;

	if ((mkdir(path, mode) == -1) && (errno != EEXIST))
		ret = -1;
	else
		ret = 0;

out:
	if (up != NULL)
		free(up);
	free(q);
	free(path);

	return (ret);
}

/***********************************************/
//  Dir;
/***********************************************/

#if 0

Dir* vdir_new (const char* dir)
{
	DIR*    dir=NULL;
	struct  dirent* ptr=NULL;
	
	int     sum=0;
	int     id=0;
	Dir*    D=NULL;

	dir = opendir(dir);
	if (!dir)
	{
		VENO;
	}
	while((ptr=readdir(dir))!=NULL)
	{
		sum++;
	}
	
	D = vnew(sizeof(Dir));
	D->vdir = vnew(sizeof(vdir_t)*sum);
	D->sum = sum;
	snprintf(D->dir, sizeof(D->dir), "%s", dir);
	
	rewinddir(dir);
	while((ptr=readdir(dir))!=NULL)
	{
		snprintf(D->vdir[id].name, sizeof(D->vdir[id].name), "%s", ptr->d_name);
		D->vdir[id].type = vpath_type(D->vdir[id].name);
		id++;
	}

	closedir(dir);
	return D;
}

Dir* vdir_refresh (Dir* D)
{
	Dir* D_new = vdir_new(D->dir);
	vdir_del(D);
	return D_new;
}

void vdir_del(Dir* D)
{
	vdel(D->vdir, sizeof(vdir_t)*(D->sum));
	vdel(D, sizeof(Dir));
	return ;
}

void vdir_rewind(Dir* D)
{
	(D->it)=0;
	return;
}

vdir_t* vdir_foreach(Dir* D, int type)
{
	for ( ; (D->it)>=0 && (D->it)<(D->sum) ; (D->it)++)
	{
		if (D->vdir[D->it].type & type)
		{
			(D->it)++;
			return (D->vdir) + ((D->it)-1);
		}
	}
	return NULL;
}
#endif

#if 0
int fd_open (const char* path)
{
	int flags = O_WRONLY | O_APPEND;
	int fd;
	
	fd = open (path, flags);
	if (fd<0)
	{
		if (errno==ENOENT)
		{
		    fd = creat (path, 0644); // fd = open (path, O_WRONLY | O_CREAT| O_TRUNK, 0644);
			if (fd<0)
			{
				return fd;
			}
		}
		else
		{
			return fd;
		}
	}

	return fd;
}

int fd_close (int fd)
{
	return close (fd);
}

int fd_write (const char* path, char* str)
{
	int fd = fd_open (path);
	if (fd>=0)
	{
		ssize_t size = write (fd, str, strlen(str));
		if (size<0) { ;/*ERROR*/ }
		fd_close (fd);
		return size;
	}
	else
	{
		return fd;
	}
}
#endif

/***********************************************/
//  Log;
/***********************************************/

#if 1

void vclr_switch(int enable)
{
	if (enable==0 || enable==1)
	{
		g_LogColor = enable;
	}
}

Log* Log_new()
{
	Log* L = (Log*) vnew(sizeof(Log));
	pthread_mutex_init(&L->mutex, NULL);
	L->fp = NULL;
	return L;
}

void Log_openfile(Log* L)
{
	L->fp = fopen(L->path, "a+");
}

void Log_closefile(Log* L)
{
	if (L->fp)
	{
		fclose(L->fp);
	}
}

void Log_del(Log* L)
{
	vdel(L, sizeof(Log));
	snprintf(L->dir, sizeof(L->dir), "./");
	snprintf(L->file, sizeof(L->file), "a.log");
}

int Log_NumberOfDigits (const char* str)
{
	int i=0;
	while ( isdigit(*(str+i)) != 0 )
	{
		i++;
	}
	return i;
}

u64t Log_fmt2time (const char* UYMWDhms)
{
	u64t us=0;
	u64t sec=0;
	int  bUnlimited=0;
	const char* p = UYMWDhms;
	while (1)
	{
		n64t nNum = atoi (p);
		int nDigit = Log_NumberOfDigits (p);
		char fmt = *(p+nDigit);
		if (fmt=='\0')
		{
			break;
		}
		if (nDigit==0)
		{
			nNum = 1;
		}

		switch (fmt)
		{
		case 'U':
			bUnlimited = 1;
			break;
		case 'Y':
			sec += nNum * 86400 * 365;
			break;
		case 'M':
			sec += nNum * 86400 * 30;
			break;
		case 'W':
			sec += nNum * 86400 * 7;
			break;
		case 'D':
			sec += nNum * 86400;
			break;
		case 'h':
			sec += nNum * 3600;
			break;
		case 'm':
			sec += nNum * 60;
			break;
		case 's':
			sec += nNum;
			break;
		default:
			break;
		}
		p = (p+nDigit)+1;
	}

	us = (bUnlimited) ? LLONG_MAX : sec * 1000000;

	return us;
}

u64t Log_fmt2byte (const char* GMKB)
{
	u64t size=0;
	const char* p = GMKB;
	while (1)
	{
		n64t nNum = atoi (p);
		int nDigit = Log_NumberOfDigits (p);
		char fmt = *(p+nDigit);
		if (fmt=='\0')
		{
			break;
		}
		if (nDigit==0)
		{
			nNum = 1;
		}

		switch (fmt)
		{
		case 'T':
		case 't':
			size += nNum << 40;
			break;
		case 'G':
		case 'g':
			size += nNum << 30;
			break;
		case 'M':
		case 'm':
			size += nNum << 20;
			break;
		case 'K':
		case 'k':
			size += nNum << 10;
			break;
		case 'B':
		case 'b':
			size += nNum;
			break;
		default:
			break;
		}
		p = (p+nDigit)+1;
	}

	return size;
}



void Log_ctl(Log* L, int opt, vdata_t value)
{
	if (value.alloc) { VEXIT(); }

	switch (opt)
	{
	case SET_log_rotate_size:
		if (value.type==INT || value.type==LONG)
		{
			L->rotateSize = value.L;
		}
		else if (value.type==PTR)
		{
			L->rotateSize = Log_fmt2byte(value.S);
		}
		break;
	case SET_log_rotate_time:
		if (value.type==INT || value.type==LONG)
		{
			L->rotateTime = value.L;
		}
		else if (value.type==PTR)
		{
			L->rotateTime = Log_fmt2time(value.S);
		}
		break;
	case SET_log_fmt:
		snprintf(L->fmt, sizeof(L->fmt), "%s", value.S);
		break;
	case SET_log_dir:
		snprintf(L->dir, sizeof(L->dir), "%s", value.S);
		snprintf(L->path, sizeof(L->path), "%s/%s", L->dir, L->file);
		break;
	case SET_log_file:
		snprintf(L->file, sizeof(L->file), "%s", value.S);
		snprintf(L->path, sizeof(L->path), "%s/%s", L->dir, L->file);
		break;
	case SET_log_output:
		L->output = value.I;
		break;
	default:
		break;
	}
}

char* Log_premsg (char* buf, int len, u64t epoch, char* fmt, const char* func, int line)
{
	char* pFmt = fmt;
	char* pEnd = buf + len - 1;
	char* p = buf;
	int offset;

	vtime_t vt = vtime_epoch2vt(epoch);
	
	while (*pFmt != '\0')
	{
		switch(*pFmt)
		{
		case 'F':
			snprintf(p, (pEnd-p), "%s", func);
			break;
		case 'L':
			snprintf(p, (pEnd-p), "%d", line);
			break;
		case 'Y':
			snprintf(p, (pEnd-p), "%04d", vt.Y);
			break;
		case 'M':
			snprintf(p, (pEnd-p), "%02d", vt.M);
			break;
		case 'D':
			snprintf(p, (pEnd-p), "%02d", vt.D);
			break;
		case 'h':
			snprintf(p, (pEnd-p), "%02d", vt.h);
			break;
		case 'm':
			snprintf(p, (pEnd-p), "%02d", vt.m);
			break;
		case 's':
			snprintf(p, (pEnd-p), "%02d", vt.s);
			break;
		case 'u':
			if (*(pFmt+1)=='3')
			{
				snprintf(p, (pEnd-p), "%03d", vt.u3);
				pFmt++;
			}
			else if (*(pFmt+1)=='6')
			{
				snprintf(p, (pEnd-p), "%06d", vt.u6);
				pFmt++;
			}
			break;
		default:
			snprintf(p, (pEnd-p), "%c", *pFmt);
			break;
		}
		pFmt++;
		offset = strlen(p);
		p += offset;
	}

	return p;
}

void Log_print__ (Log* L, const char* func, int line, char* msg, ...)
{
	static __thread char buf[4096]={0};
	memset (buf, 0, sizeof(buf));

	u64t    epoch = vnow(US);
	char*   pBuf = Log_premsg (buf, sizeof(buf), epoch, L->fmt, func, line);
	va_list ap;

	va_start (ap, msg);
	vsprintf(pBuf, msg, ap);
	va_end (ap);
	
	pthread_mutex_lock(&L->mutex);
	{
		if (L->output & L_STDOUT)
		{
			//printf("%s", buf);
			fputs(buf, stdout);
		}
		if (L->output & L_STDERR)
		{
			fputs(buf, stderr);
		}
		if (L->output & L_FILE)
		{
			if (!L->fp)
			{
				L->fp = fopen(L->path, "a+");
			}
			if (L->fp)
			{
				fputs(buf, L->fp);
			}
		}
		L->epoch = epoch;
	}
	pthread_mutex_unlock(&L->mutex);
}





#endif


/***********************************************/
//  Routine;
/***********************************************/

Routine* Routine_new(pp_ft func, void* arg)
{
	Routine* R = (Routine*) vnew(sizeof(Routine));
	R->func = func;
	R->arg = arg;
	pthread_create(&R->pid, NULL, R->func, arg);
	return R;
}

void Routine_del(Routine* R)
{
	vdel(R, sizeof(Routine));
}

int Routine_detach(Routine* R)
{
	return pthread_detach(R->pid);
}

int Routine_join(Routine* R)
{
	return pthread_join(R->pid, NULL);
}

/***********************************************/
//  END;
/***********************************************/

