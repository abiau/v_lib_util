#include "v.h"

	
void sample_log()
{
	cprintf('G', "%s:%d\n", __func__, __LINE__);
	Log* L = Log_new();
	Log_ctl(L, SET_log_fmt, vdata("MD h:m:s | F:L | "));
	Log_ctl(L, SET_log_rotate_size, vdata("2G"));
	Log_ctl(L, SET_log_rotate_time, vdata("1D"));
	Log_ctl(L, SET_log_dir, vdata("./"));
	Log_ctl(L, SET_log_file, vdata("abc.log"));
	Log_ctl(L, SET_log_output, vdata(L_STDOUT|L_FILE));

	vclr_switch(0);
	Log_print(L, "%s| abc=%d\n", cword('G', "OK"), 6655);
	Log_print(L, "%s| abc=%d\n", cword('R', "FAIL"), 6655);
	Log_print(L, "rotate_size=%llu Byte.\n", L->rotateSize);
	Log_print(L, "rotate_time=%llu usec.\n", L->rotateTime);
	Log_del(L);
}

void* routine_add(void* arg)
{
	size_t id = (size_t)arg;
	while(1)
	{
		vmsleep(vrandom(500));
		printf ("id=%zd\n", id);
	}
	return NULL;
}

void sample_routine()
{
	int i;
	Routine* R[10] = {0};
	for (i=0; i<2; i++)
	{
		R[i] = Routine_new(routine_add, (size_t)i);
		Routine_detach(R[i]);
	}
	vnonstop();
}

int main(int argc, char* argv[])
{
	NOUSE(argc);
	NOUSE(argv);

#if 1
	sample_routine();
	//sample_list();
	//sample_log();
#else
	//program_parse_publish_uid(argc, argv);
	//program_parse_framelog(argc, argv);
	//program_parse_utdlog(argc, argv);
#endif

	printf ("\n[Info] mem.size=%d, mem.cnt=%d\n", vInfo().memSize, vInfo().memCnt);

	return EXIT_SUCCESS;
}


#if 0
	sample_dir();
	sample_file();
	sample_findToken();
	sample_thread();
	sample_color();
	sample_time();
	sample_bt();
	sample_cut();
	sample_queue_int();
	sample_queue_void();
	sample_list();
#endif


