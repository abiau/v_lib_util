#include "v.h"

/***********************************************/
//  global variable;
/***********************************************/

/***********************************************/
//  Definition;
/***********************************************/

int vnet_open (int type);
int vnet_set (int skt, int opt);
int vnet_bind (int skt, int port);
int vnet_listen (int skt, int maxSession);
static saddrin_t _toSaddrIn (char* ip, int port);

/***********************************************/
//  static function;
/***********************************************/

int vnet_set (int skt, int opt)
{
	int ret=0;
	
	switch (opt)
	{
	case SET_reuseaddr:
		{
			int sock_opt = 1;
			ret = setsockopt(skt, SOL_SOCKET, SO_REUSEADDR, (void*)&sock_opt, sizeof(sock_opt) );
			if (ret<0)
			{
				VENO; 
				return -errno;
			}
		}
		break;
	case SET_nonblock:
		{
			int flag = 1;
			flag = fcntl (skt, F_GETFL, 0);
			ret = fcntl (skt, F_SETFL, flag | O_NONBLOCK);
			if (ret<0)
			{
				VENO; 
				return -errno;
			}
		}
		break;
	default:
		break;
	}

	return OKAY;
}

int vnet_bind (int skt, int port)
{
	int        ret;
	saddrin_t  addr = _toSaddrIn (NULL, port);

	ret = bind (skt, (struct sockaddr*) &addr, sizeof(saddrin_t));
	if (ret<0)
	{
		VENO; 
		return -errno;
	}
	return ret;
}

int vnet_listen (int skt, int maxSession)
{
	return listen (skt, maxSession);
}

/***********************************************/
//  open function;
/***********************************************/

vaddr_t toVaddr (saddrin_t addr)
{
	vaddr_t va;
	memcpy (&va.addr, &addr, sizeof(saddrin_t));
	snprintf (va.ip, sizeof(va.ip), "%s", inet_ntoa (addr.sin_addr));
	va.port = addr.sin_port;
	return va;
}

saddrin_t _toSaddrIn (char* ip, int port)
{
	struct hostent*  hen;
	saddrin_t        addr; 

	bzero (&addr, sizeof(saddrin_t));
	
	if (ip)
	{
		hen = gethostbyname (ip);
		if (hen==NULL)
		{
			VENO; 
			return addr;
		}
		memcpy (&addr.sin_addr.s_addr, hen->h_addr, hen->h_length);
	}
	else
	{
		addr.sin_addr.s_addr = INADDR_ANY;
	}
	addr.sin_family = PF_INET;
	addr.sin_port   = htons(port);

	return addr;
}

/*****************************************/

Net* vnet_server (int type, int bindPort, int maxSession)
{
	Net* N = malloc (sizeof(Net)); 

	int skt = vnet_open (type);
	vnet_set (skt, SET_reuseaddr);
	vnet_set (skt, SET_nonblock);
	vnet_bind (skt, bindPort);
	if (type==TCP)
	{
		vnet_listen (skt, maxSession);
	}
	
	N->type = type;
	N->bindPort = bindPort;
	N->skt = skt;

	return N;
}

Net* vnet_client (int type)
{
	Net* N = malloc (sizeof(Net)); 

	int skt = vnet_open (type);
	int port = 0;
	vnet_set (skt, SET_reuseaddr);
	vnet_bind (skt, port);
	
	N->type = type;
	N->bindPort = port;
	N->skt = skt;

	return N;
}

/*****************************************/

int vnet_open (int type)
{
	int skt;
	int sock_type = -1;

	if (type==TCP)
	{
		sock_type = SOCK_STREAM;
	}
	else if (type==UDP)
	{
		sock_type = SOCK_DGRAM;
	}
	skt = socket (PF_INET, sock_type, 0);
	if (skt<0)
	{
		VENO;
		return -errno;
	}
	return skt;
}

int vnet_close (int skt)
{
	return close (skt);
}


int vnet_accept (Net* N, vaddr_t* pAddr)
{
	int        skt_accept; 
	saddrin_t  addr;
	socklen_t  clen = sizeof(saddrin_t);

	skt_accept = accept (N->skt, (struct sockaddr *)&addr, &clen); 
	if (skt_accept<0)
	{
		return -errno;
	}
	vaddr_t va = toVaddr(addr);
	memcpy(pAddr, &va, sizeof(vaddr_t));
	vnet_set (skt_accept, SET_reuseaddr);
	vnet_set (skt_accept, SET_nonblock);

	return skt_accept;
}

int vnet_connect (Net* N, char* ip, int port)
{
	int ret;
	saddrin_t addr = _toSaddrIn(ip, port);

	while(1)
	{
		ret = connect(N->skt, (struct sockaddr *)&addr, sizeof(saddrin_t));
		if (ret<0 && errno==EINPROGRESS)
		{
			vmsleep(10);
		}
		else if(ret<0)
		{
			VENO; 
			return -errno;
		}
		else
		{
			break;
		}
	}
	
	return ret;
}


void vnet_del (Net* N)
{
	vnet_close (N->skt);
	free (N);
	return ;
}


int vnet_write (int skt,  char* buf, int bufLen, u64t timeout_ms)
{
	int ret;
	u64t ms_timeout = vnow(MS) + timeout_ms;

	while(1)
	{
		if (timeout_ms>0 && vnow(MS)>ms_timeout)
		{
			return VERROR_TIMEOUT;
		}
		ret = write (skt, buf, bufLen);
		if (ret<0 && errno==EAGAIN)
		{
			vmsleep(1);
		}
		else if (ret<0)
		{
			return -errno;
		}
		else
		{
			return ret;
		}
	}
}

int vnet_read (int skt,  char* buf, int bufLen, u64t timeout_ms) 
{
	int ret;
	u64t ms_timeout = vnow(MS) + timeout_ms;
	bzero(buf, bufLen);
	
	while(1)
	{
		if (timeout_ms>0 && vnow(MS)>ms_timeout)
		{
			return VERROR_TIMEOUT;
		}
		ret = read (skt, buf, bufLen);
		if (ret<0 && errno==EAGAIN)
		{
			vmsleep(1);
		}
		else if (ret<0)
		{
			return -errno;
		}
		else
		{
			return ret;
		}
	}
}

/***********************************************/
//  TODO;
/***********************************************/

/*
int vnet_sendto (int skt, char* buf, int bufLen, char* ip, int port)
{
	int        ret;
	socklen_t  clen = sizeof(struct sockaddr);
	saddrin_t  addr = _toSaddrIn(ip, port);
	
	ret = sendto (skt, buf, bufLen, 0, (struct sockaddr*)&(addr), clen);
	if (ret<0)
	{
		VENO; 
		return -errno;
	}
	return ret;
}

int vnet_recvfrom  (int skt, char* buf, int bufLen, vaddr_t* pAddr)
{
	int        ret;
	saddrin_t  addr;
	socklen_t  clen = sizeof(saddrin_t);

	ret = recvfrom (skt, buf, bufLen, 0, (struct sockaddr*) &addr, &clen);
	if (ret<0)
	{
		return -errno;
	}
	if (pAddr)
	{
		vaddr_t va = toVaddr(addr);
		memcpy(pAddr, &va, sizeof(vaddr_t));
	}
	return ret;
}
*/


/***********************************************/
//  END;
/***********************************************/


