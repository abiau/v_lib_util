#include "v.h"

void sample_color()
{
	vendl();
	cprintf ('G', "%s", __func__);
}


static int _eql(vdata_t item, vdata_t arg)
{
	if (item.I == arg.I)
		return 0;
	else
		return -1;
}

static int _cmp(vdata_t item, vdata_t arg)
{
	if (item.I > arg.I)
		return 0;
	else
		return -1;
}

static void _pnt(vdata_t arg)
{
	printf("Int=%d\t", arg.I);
}


void sample_queue_int()
{
	Queue* Q = Queue_new();
	Queue_push(Q, vdata(1));
	Queue_push(Q, vdata(-2));
	Queue_push(Q, vdata(3));
	//Queue_clear(Q);
	
	vdata_t data;
	while ((data=Queue_pop(Q)).exist)
	{
		printf ("Queue_pop=%d\n", data.I);
	}

	Queue_del(Q);
	return;
}

#if 0
void sample_queue_void()
{
	Queue* Q = Queue_new();
	Queue_ctl(Q, SET_del_fn, int_del);
	Queue_push(Q, vdata(int_new(1)));
	Queue_push(Q, vdata(int_new(2)));
	Queue_push(Q, vdata(int_new(3)));
	
	vdata_t data;
	while ((data=Queue_pop(Q)).exist)
	{
		int* pInt = (int*) data.P;
		printf ("Queue_pop=%d\n", *pInt);
		int_del(pInt);
	}
	Queue_del(Q);
	return;
}
#endif


void sample_list()
{
	List* L = List_new();
	List_ctl(L, SET_eql_fn, _eql);
	List_ctl(L, SET_cmp_fn, _cmp);
	List_ctl(L, SET_pnt_fn, _pnt);
	
	List_insert(L, vdata(1));
	List_insert(L, vdata(3));
	List_insert(L, vdata(2));
#if 1
	List_rinsert(L, vdata(7));
	List_rinsert(L, vdata(6));
	List_rinsert(L, vdata(8));
#endif
	printf ("[Print]\n");
	List_print(L);

	printf ("[Traversal]\n");
	List_traversal(L, (idd_ft)_pnt, vnull);
	vendl();
#if 1
	printf ("[RPrint]\n");
	List_rprint(L);
	vdata_t value = List_search(L, vdata(6));
	if (value.type)
		printf ("[Search] value=%d\n", value.I);
	List_print(L);
	value = List_obtain(L, vdata(6));
	if (value.type)
	{
		printf ("[Obtain] value=%d\n", value.I);
	}
	printf ("[Print]\n");
	List_print(L);
	List_del(L);
#endif
	

	return;
}


#if 1

void sample_tcpClient()
{
	int PORT_SERVER = 5566;
	int ret;
	int cnt=0;
	int  timeout_ms = 10;
	char wbuf[1024];
	char rbuf[1024];
	
	Net* vc = vnet_client(TCP);
	int skt = vc->skt;

	do{
		ret = vnet_connect(vc, "127.0.0.1", PORT_SERVER);
	}while(ret<0);

	while(1)
	{
		vmsleep(300);
		snprintf(wbuf, sizeof(wbuf), "%d", cnt++);
		ret = vnet_write (skt,  wbuf, sizeof(wbuf), timeout_ms); 
		while(1)
		{
			ret = vnet_read (skt, rbuf, sizeof(rbuf), timeout_ms); 
			if (ret==VERROR_TIMEOUT)
			{
				;
			}
			else if (ret<0)
			{
				VENO;
				goto __FAIL;
			}
			else
			{
				printf("echo:%s\n", rbuf);
				break;
			}
		}
	}
__FAIL:
	vnet_del(vc);
}

void sample_tcpServer()
{
	int  PORT_SERVER = 5566;
	int  timeout_ms = 10;
	int  max_session = 3;
	Net* vs = vnet_server(TCP, PORT_SERVER, max_session);
	char wbuf[1024];
	char rbuf[1024];
	while(1)
	{
		vaddr_t vaddr;
		int skt = vnet_accept(vs, &vaddr);
		while(skt>0)
		{
			int ret;
			ret = vnet_read (skt, rbuf, sizeof(rbuf), timeout_ms); 
			if (ret==VERROR_TIMEOUT)
			{
				;
			}
			else if (ret<0)
			{
				VENO;
				goto __FAIL;
			}
			else
			{
				snprintf(wbuf, sizeof(wbuf), "[%s|%d] %s", vaddr.ip, vaddr.port, rbuf);
				ret = vnet_write (skt,  wbuf, sizeof(wbuf), timeout_ms); 
			}
		}
		vnet_close(skt);
		vmsleep(1);
	}
__FAIL:
	vnet_del(vs);
}

#endif

void sample_data()
{
	vdata_t dt  = vnull;
	printf("dt.type=%d\n", dt.type);
	
	vdata_t data;
	
	printf("I=%d\n", vdata(5).I);
	printf("F=%f\n", vdata(0.5).F);
	printf("F=%d\n", vdata(0.0).I);

	char c='c';
	data = vdata(c);
	printf("C=%c\n", vdata(c).C);
	
	size_t n=5;
	data = vdata(n);
	printf("L=%lld\n", data.L);
	
	float f=0.3;
	data = vdata(f);
	printf("F=%f\n", data.F);
	
	const char* s="xxx";
	s="yyy";
	s="abc";
	data = vdata(s);
	printf("S=%s\n", data.S);
	
	Node* p = Node_new(vdata(NULL));
	data = vdata(p);
	printf("P=%p\n", data.P);
	Node_del(p);

	return;
}

void sample_findToken()
{
	vendl();
	cprintf ('G', "%s", __func__);

	char* p=NULL;
	char* line="<Ts, U:FHPABX5MRNRMKN6GYH4J, T:1526659182380, D:12000, f:2173>";
	printf("src=%s\n", line);
	if ((p = vstr_rfindToken(line, "f:", 0)))
	{
		printf("rfind,p=%s\n", p);
	}
	if ((p = vstr_findToken(line, "U:", 0)))
	{
		printf("lfind,p=%s\n", p);
	}
}

void sample_cut()
{
	vendl();
	cprintf ('G', "%s", __func__);
	
	char* line="<Ts, U:FHPABX5MRNRMKN6GYH4J, T:1526659182380, D:12000, f:2173>";
	char* p = vstr_cutWords(line, "<", ">", 1);
	if(p)
	{
		printf("Got it!!\n%s\n", p);
	}
}

void sample_time()
{
	vendl();
	cprintf ('G', "%s", __func__);
	char* strTime;
	u64t ms;
	u64t us;
	u64t epoch1;
	u64t epoch2;
	n64t diff;

	vtime_t vt;
	char* p;
	char* Time_HDhmsu3_1="0531 11:06:43.999";
	char* Time_HDhmsu3_2="0531 11:06:44.019";

	vt = vtime_str2vt(Time_HDhmsu3_1, "MD h:m:s.u3");
	epoch1 = vtime_str2epoch(Time_HDhmsu3_1, "MD h:m:s.u3");
	p = vtime_vt2str(vt, "YMD h:m:s.u3");
	printf ("Time_HDhmsu3_1=%s\n", Time_HDhmsu3_1);
	printf ("Convert(p)=%s\n", p);
	printf ("epoch(p)=%llu\n", epoch1);
	vendl();
	
	vt = vtime_str2vt(Time_HDhmsu3_2, "MD h:m:s.u3");
	epoch2 = vtime_str2epoch(Time_HDhmsu3_2, "MD h:m:s.u3");
	p = vtime_vt2str(vt, "YMD h:m:s.u3");
	printf ("Time_HDhmsu3_2=%s\n", Time_HDhmsu3_2);
	printf ("Convert(p)=%s\n", p);
	printf ("epoch(p)=%llu\n", epoch2);
	vendl();

	diff = epoch2/1000 - (epoch1/1000);
	printf ("diff(epoch2-epoch1)=%6llu\n", diff);
	vendl();

	ms = vnow_ms();
	printf ("vnow_ms()=%llu\n", ms);
	
	strTime = vtime_epoch2str(ms, "Y/M/D h:m:s.u3");
	printf ("epoch2str()=%s\n", strTime);

	us = vtime_str2epoch(strTime, "Y/M/D h:m:s.u3");
	printf ("str2epoch()=%llu\n", us);

}

void sample_bt()
{
	vendl();
	cprintf ('G', "%s", __func__);
	vtrace();
	vtrace_shell("./exe", "./bt.log");
}


void sample_thread()
{
	void* routine(void* arg)
	{
		void _count(size_t id)
		{
			static __thread int cnt=0;
			printf("[%zd]  cnt=%d\n", id, cnt++);
		}

		int i;
		for(i=0; i<10; i++)
		{
			_count((size_t)arg);
			vmsleep(vrandom(5));
		}

		return NULL;
	}

	vthread_t t1;
	vthread_t t2;
	pthread_create(&t1, NULL, routine, (void*)1);
	pthread_create(&t2, NULL, routine, (void*)2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return ;
}

void sample_file()
{
	const char* path="./abc";
	int exist = vpath_exist (path);
	int size = vpath_size (path);
	int isFile = vpath_isfile (path);
	int isFolder = vpath_isdir (path);
	VTT(exist);
	VTT(size);
	VTT(isFile);
	VTT(isFolder);

	return;
}


void program_parse_utdlog(int argc, char* argv[])
{
	if (argc!=3)
	{
		printf ("Error, argc=%d\n", argc);
		printf ("<usage> %s <utd.log> <uid.log>\n", argv[0]);
		return ;
	}

	typedef struct {
		u64t sysT;
		char U[32];
		u64t T;
		n64t delta_sysT;
		n64t delta_lastT;
		u64t D;
		u64t frag;
	} entry_t;

	FILE* rfp = fopen(argv[1], "r");
	FILE* wfp = fopen(argv[2], "a+");

	char rbuf[4096];
	char wbuf[4096];
	
	entry_t e;
	u64t lastT=0;
	char* p;
	while(fgets(rbuf, sizeof(rbuf), rfp))
	{
		e.sysT = vtime_str2epoch(rbuf, "YMD h:m:s");
		
		p = vstr_cutWords(rbuf, "U:", ",", 0);
		if (p) {
			snprintf (e.U, sizeof(e.U), "%s", p);
		}
		
		p = vstr_cutWords(rbuf, "T:", ",", 0);
		if (p) {
			e.T = atoll(p);
		}

		e.delta_lastT = e.T/1000 - lastT/1000;
		e.delta_sysT = e.T/1000 - (e.sysT/1000000);
		
		p = vstr_cutWords(rbuf, "D:", ",", 0);
		if (p) {
			e.D = atoll(p);
		}
		
		p = vstr_cutWords(rbuf, "f:", ">", 0);
		if (p) {
			e.frag = atoll(p);
		}

		if (e.frag==0)
		{
			e.delta_lastT = 0;
			e.delta_sysT = 0;
		}

		snprintf (wbuf, sizeof(wbuf)
				, "%s | T:%03llu, D:%llu, delta: %3lld s. sys %3lld s.  f:%llu\n"
				, vtime_epoch2str(e.T, "MD h:m:s")
				, (e.T)%1000000/1000
				, e.D
				, e.delta_lastT
				, e.delta_sysT
				, e.frag
				);
		//printf ("%s", wbuf);
	
		lastT=e.T;

		fputs(wbuf, wfp);
		//if (i++ > 10) { break; }
	}
	fclose(rfp);
	fclose(wfp);

}

void program_parse_framelog(int argc, char* argv[])
{
	if (argc!=2)
	{
		printf ("Error, argc=%d\n", argc);
		printf ("<usage> %s <debug.log>\n", argv[0]);
		return ;
	}

	typedef struct {
		u64t sysT;
		char U[32];
		u64t T;
		n64t delta_sysT;
		u64t D;
		u64t frag;
	} entry_t;
	
	FILE* rfp = fopen(argv[1], "r");
	char outpath[1024];
	char rbuf[4096];
	char wbuf[4096];
	
	entry_t e;
	u64t lastT=0;
	char* p;
	//int i=0;
	while(fgets(rbuf, sizeof(rbuf), rfp))
	{
		e.sysT = vtime_str2epoch(rbuf, "MD h:m:s.u3");
		
		p = vstr_cutWords(rbuf, " | ", " | ", 0);
		if (p) {
			snprintf (e.U, sizeof(e.U), "%s", p);
		}
		else
			continue;

		
		e.delta_sysT = (e.sysT/1000) - (lastT/1000);
		
		p = vstr_rfindToken(rbuf, " | ", 0);
		if (!p) {
			continue;
		}
		
		lastT = e.sysT;
		
		if (e.delta_sysT > 1000)
		{
			snprintf(wbuf, sizeof(wbuf), "%s | %6lld |=====| %s", vtime_epoch2str(e.sysT, "MD h:m:s.u3"), e.delta_sysT, p);
		}
		else
		{
			snprintf(wbuf, sizeof(wbuf), "%s | %6lld | %s", vtime_epoch2str(e.sysT, "MD h:m:s.u3"), e.delta_sysT, p);
		}

		//printf ("%s", wbuf);
	
		snprintf(outpath, sizeof(outpath), "%s.diffts", e.U);
		FILE* fp = fopen(outpath, "a+");
		if (fp)
		{
			fputs(wbuf, fp);
			fclose(fp);
		}
	

		//if (i++ > 10) { break; }
	}
	fclose(rfp);

}

void _pnt_uid (vdata_t item)
{
	printf("S=%s\n", item.S);
}

void _del_uid (vdata_t item)
{
	vdataDel(item);
}

int _eql_uid (vdata_t item, vdata_t value)
{
	return strcmp(item.S, value.S);
}

int _cmp_uid (vdata_t item, vdata_t value)
{
	int i;
	int len = vmin(strlen(item.S), strlen(value.S));

	char* p1 = item.S;
	char* p2 = value.S;

	for (i=0; i<len+1; i++)
	{
		if (*(p1+i)>*(p2+i))
			return 0;
		else
			return -1;
	}
	return -1;
}

void program_parse_publish_uid(int argc, char* argv[])
{
	if (argc!=3)
	{
		printf ("Error, argc=%d\n", argc);
		printf ("<usage> %s <publish.log> <tree.log>\n", argv[0]);
		return ;
	}
	
	List* L1 = List_new();
	List_ctl(L1, SET_cmp_fn, _cmp_uid);
	List_ctl(L1, SET_todo_fn, _pnt_uid);
	List_ctl(L1, SET_del_fn, _del_uid);
	List_ctl(L1, SET_eql_fn, _eql_uid);
	

	FILE* rfp;
	char rbuf[4096];
	char* p;

	rfp = fopen(argv[1], "r");
	while(fgets(rbuf, sizeof(rbuf), rfp))
	{
		p = vstr_rcutWords(rbuf, " | ", " |  ON|", 0);
		if (p)
		{
			List_insert(L1, vdataS(p));
		}
	}
	vendl();
	List_print(L1);
	printf("size=%d\n", List_size(L1));
	vendl();
	fclose(rfp);
	

	rfp = fopen(argv[2], "r");
	while(fgets(rbuf, sizeof(rbuf), rfp))
	{
		p = vstr_rcutWords(rbuf, NULL, ".ts", 0);
		if (p) { continue; }
		p = vstr_rcutWords(rbuf, "├── ", "\n", 0);
		if (p)
		{
			vdata_t vd = List_obtain(L1, vdata((void*)p));
			if (vd.exist)
			{
				printf("uid=%s\n", vd.S);
				vdataDel(vd);
			}
		}
	}
	vendl();
	List_print(L1);
	printf("size=%d\n", List_size(L1));
	fclose(rfp);
	List_del(L1);
	
}


#if 0
void sample_dir()
{
	vdir_t* vd;
	Dir* D= vdir_new("./");

	vdir_rewind(D);
	while((vd=vdir_foreach(D, F_FILE|F_DIR)))
	{
		printf ("[%c] %s\n", (vd->type==F_DIR)?'D':'-', vd->name);
	}
	vdir_del(D);

	return;
}
#endif



