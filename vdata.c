#include "v.h"

/***********************************************/
//  global variable;
/***********************************************/

/***********************************************/
//  static function;
/***********************************************/

/***********************************************/
//  open function;
/***********************************************/


/***********************************************/
//  general data type;
/***********************************************/

#if 0
int* int_new(int n)
{
	int* pInt = (int*)vnew(sizeof(int));
#if 1
	*pInt = n;
	printf ("int_new(%d)\n", *pInt);
#else
	*pInt = vrandom(10);
	printf ("int_new(%d)\n", *pInt);
#endif
	return pInt;
}

void int_del(int* pInt)
{
#if 0
	printf ("int_del(%d)\n", *pInt);
#endif
	vdel(pInt, sizeof(int));
}
#endif

/***********************************************/
//  AnyData;
/***********************************************/

vdata_t data_null()
{
	vdata_t d = {.type=0, .alloc=0, .L=0};
	return d;
}

vdata_t data_ch(char x)
{
	vdata_t d = {.type=CH, .alloc=0, .C=x};
	return d;
}
vdata_t data_int(n32t x)
{
	vdata_t d = {.type=INT, .alloc=0, .I=x};
	return d;
}
vdata_t data_long(n64t x)
{
	vdata_t d = {.type=INT, .alloc=0, .L=x};
	return d;
}
vdata_t data_float(double x)
{
	vdata_t d = {.type=FLOAT, .alloc=0, .F=x};
	return d;
}
vdata_t data_ptr(void* x)
{
	vdata_t d = {.type=PTR, .alloc=0, .P=x};
	return d;
}
vdata_t data_cstr(const char* x)
{
	vdata_t d = {.type=PTR, .alloc=0, .S=(char*)x};
	return d;
}



vdata_t data_newbyte(char* x, int alloc)
{
	vdata_t d = {.type=BYTE, .alloc=(alloc), .S=x};
	d.B = vnew(alloc);
	memcpy(d.B, x, alloc);
	return d;
}
vdata_t data_newstr(char* x)
{
	vdata_t d = data_newbyte(x, strlen(x)+1);
	d.type = STR;
	return d;
}
vdata_t data_newcstr(const char* x)
{
	vdata_t d = data_newbyte((char*)x, strlen(x)+1);
	d.type = STR;
	return d;
}

void data_delbyte(vdata_t d)
{
	if (d.type==BYTE || d.type==STR)
	{
		vdel(d.B, d.alloc);
	}
}


/***********************************************/
//  operator;
/***********************************************/

OP* OP_new()
{
	OP* op = (OP*) vnew(sizeof(OP));
	return op;
}

void OP_del(OP* op)
{
	return vdel(op, sizeof(OP));
}

void OP_ctl(OP* op, int opt, void* ptr)
{
	switch (opt)
	{
	case SET_eql_fn:
		op->eql = ptr;
		break;
	case SET_cmp_fn:
		op->cmp = ptr;
		break;
	case SET_pnt_fn:
		op->pnt = ptr;
		break;
	case SET_del_fn:
		op->del = ptr;
		break;
	default:
		break;
	}
}

/***********************************************/
//  chain;
/***********************************************/

Node* Node_new(vdata_t value)
{
	Node* node = (Node*) vnew(sizeof(Node));
	//node->key = key;
	node->value = value;
	return node;
}

void Node_del(Node* node)
{
	return vdel(node, sizeof(Node));
}


Chain* Chain_new()
{
	Chain* C = (Chain*) vnew(sizeof(Chain));
	return C;
}

void Chain_del(Chain* C)
{
	return vdel(C, sizeof(Chain));
}


vdata_t Chain_pop(Chain* C, Node* node)
{
	if (!node)
	{
		return data_null();
	}
	else if (!node->L && !node->R)
	{
		/*
		   +------+                          
		   | node |                          
		   +------+                          
		   (mostL/mostR)
		   */
		C->mostL = NULL;
		C->mostR = NULL;
	}
	else if (node->L && !node->R)
	{
		/*
		   +-----+      +-----+       +------+
		   | ... | <--> |  p  | <-/-> | node |
		   +-----+      +-----+       +------+
		                (mostR)
		   */
		Node* p = node->L;
		p->R = NULL;
		C->mostR = p;
	}
	else if (!node->L && node->R)
	{
		/*
		   +------+       +-----+      +-----+
		   | node | <-/-> |  p  | <--> | ... |
		   +------+       +-----+      +-----+
		                  (mostL)
		   */
		Node* p = node->R;
		p->L = NULL;
		C->mostL = p;
	}
	else if (node->L && node->R)
	{
		/*
		   +----+       +------+       +----+
		   | pL | <-/-> | node | <-/-> | pR |
		   +----+       +------+       +----+
		   (mostL)                     (mostR)
		   */
		Node* pL = node->L;
		Node* pR = node->R;
		pL->R = pR;
		pR->L = pL;
	}

	node->L = NULL;
	node->R = NULL;
	vdata_t value = node->value;
	Node_del(node);
	C->size--;
	return value;
}

void Chain_lpush(Chain* C, Node* p, Node* node)
{
	C->size++;
	if (!p)
	{
		/*
		   +------+                          
		   | node |                          
		   +------+                          
		   (mostL/mostR)
		   */
		C->mostL = node;
		C->mostR = node;
	}
	else if (p && p->L)
	{
		/*
		   +----+      +------+      +----+
		   | pL | <--> | node | <--> | pR |
		   +----+      +------+      +----+
		                              (p)
		   */
		Node* pL = p->L;
		Node* pR = p;

		node->L = pL;
		node->R = pR;
		pL->R = node;
		pR->L = node;
	}
	else if (p && !p->L)
	{
		/*
		                +------+      +----+
		      pL   <--- | node | <--> | pR |
		                +------+      +----+
		    (NULL)       (mostL)       (p)
		   */
		Node* pL = p->L;
		Node* pR = p;
		
		node->L = pL;
		node->R = pR;
		pR->L = node;
		C->mostL = node;
	}
}

void Chain_rpush(Chain* C, Node* p, Node* node)
{
	C->size++;
	if (!p)
	{
		/*
		   +------+                          
		   | node |                          
		   +------+                          
		   (mostL/mostR)
		   */
		C->mostL = node;
		C->mostR = node;
	}
	else if (p && p->R)
	{
		/*
		   +----+      +------+      +----+
		   | pL | <--> | node | <--> | pR |
		   +----+      +------+      +----+
		    (p)
		   */
		Node* pL = p;
		Node* pR = p->R;

		node->L = pL;
		node->R = pR;
		pL->R = node;
		pR->L = node;
	}
	else if (p && !p->R)
	{
		/*
		    +----+      +------+            
		    | pL | <--> | node | --->   pR  
		    +----+      +------+            
		     (p)         (mostR)      (NULL)
		   */
		Node* pL = p;
		Node* pR = p->R;
		
		node->L = pL;
		node->R = pR;
		pL->R = node;
		C->mostR = node;
	}
}

void Chain_traversalFrom(int order, Node* node, idd_ft todo, vdata_t arg)
{
	Node* p = node;
	while (p)
	{
		todo(p->value, arg);

		if (order==FROM_L)
			p = p->R;
		else if (order==FROM_R)
			p = p->L;
	}
	return;
}

Node* Chain_sortFrom(int order, Node* node, idd_ft cmp, vdata_t value)
{
	Node* p = node;
	while (p)
	{
		if (order==FROM_L)
		{
			if ( cmp(p->value, value)==0 )
				break;
			else
				p = p->R;
		}
		else if (order==FROM_R)
		{
			if ( cmp(p->value, value)!=0 )
				break;
			else
				p = p->L;
		}
	}
	return p;
}

Node* Chain_searchFrom(int order, Node* node, idd_ft cmp, vdata_t value)
{
	Node* p = node;
	while (p)
	{
		if ( cmp(p->value, value)==0 )
			break;
		else
			if (order==FROM_L)
				p = p->R;
			else if (order==FROM_R)
				p = p->L;
	}
	return p;
}





/***********************************************/
//  Queue;
/***********************************************/

#if 1

Queue* Queue_new()
{
	Queue* Q = (Queue*) vnew(sizeof(Queue));
	Q->chain = Chain_new();
	Q->op = OP_new();
	return Q;
}

void Queue_del(Queue* Q)
{
	Queue_clear(Q);
	Chain_del(Q->chain);
	OP_del(Q->op);
	vdel(Q, sizeof(Queue));
}

void Queue_ctl(Queue* Q, int opt, void* ptr)
{
	OP* op = Q->op;
	OP_ctl(op, opt, ptr);
}

void Queue_clear(Queue* Q)
{
	OP* op = Q->op;
	Chain* C = Q->chain;
	Node* p = C->mostL;
	while ((p = C->mostL))
	{
		vdata_t value = Chain_pop(C, p);
		if (op->del)
			op->del(value);
	}
}

void Queue_push(Queue* Q, vdata_t value)
{
	Chain* C = Q->chain;
	Node* node = Node_new(value);
	Chain_lpush(C, C->mostL, node);
}

vdata_t Queue_pop(Queue* Q)
{
	Chain* C = Q->chain;
	return Chain_pop(C, C->mostR);
}

#endif

/***********************************************/
//  List;
/***********************************************/

List* List_new()
{
	List* L = (List*) vnew(sizeof(List));
	L->chain = Chain_new();
	L->op = OP_new();
	return L;
}

void List_del(List* L)
{
	List_clear(L);
	Chain_del(L->chain);
	OP_del(L->op);
	vdel(L, sizeof(List));
}

void List_ctl(List* L, int opt, void* ptr)
{
	OP* op = L->op;
	OP_ctl(op, opt, ptr);
}

int List_size(List* L)
{
	return L->chain->size;
}

void List_clear(List* L)
{
	OP* op = L->op;
	Chain* C = L->chain;
	Node* p = C->mostL;
	while ((p = C->mostL))
	{
		vdata_t value = Chain_pop(C, p);
		if (op->del)
			op->del(value);
	}
}

void List_traversal(List* L, idd_ft todo, vdata_t arg)
{
	Chain* C = L->chain;
	Chain_traversalFrom(FROM_L, C->mostL, todo, arg);
}

void List_print(List* L)
{
	OP* op = L->op;
	Chain* C = L->chain;
	Chain_traversalFrom(FROM_L, C->mostL, op->pnt, vdata(NULL));
	vendl();
}

void List_rprint(List* L)
{
	OP* op = L->op;
	Chain* C = L->chain;
	Chain_traversalFrom(FROM_R, C->mostR, op->pnt, vdata(NULL));
	vendl();
}

vdata_t List_search(List* L, vdata_t value)
{
	Chain* C = L->chain;
	OP* op = L->op;
	Node* p = Chain_searchFrom(FROM_L, C->mostL, op->eql, value);
	if (p)
		return p->value;
	else
		return data_null();
}

vdata_t List_rsearch(List* L, vdata_t value)
{
	Chain* C = L->chain;
	OP* op = L->op;
	Node* p = Chain_searchFrom(FROM_R, C->mostR, op->eql, value);
	if (p)
		return p->value;
	else
		return data_null();
}

vdata_t List_obtain(List* L, vdata_t value)
{
	Chain* C = L->chain;
	OP* op = L->op;
	Node* p = Chain_searchFrom(FROM_L, C->mostL, op->eql, value);
	return Chain_pop(L->chain, p);
}

vdata_t List_robtain(List* L, vdata_t value)
{
	Chain* C = L->chain;
	OP* op = L->op;
	Node* p = Chain_searchFrom(FROM_R, C->mostR, op->eql, value);
	return Chain_pop(L->chain, p);
}

void List_insert(List* L, vdata_t value)
{
	Chain* C = L->chain;
	OP* op = L->op;
	Node* node = Node_new(value);
	Node* p = Chain_sortFrom(FROM_L, C->mostL, op->cmp, value);
	if (p)
		Chain_lpush(C, p, node);
	else
		Chain_rpush(C, C->mostR, node);
}

void List_rinsert(List* L, vdata_t value)
{
	Chain* C = L->chain;
	OP* op = L->op;
	Node* node = Node_new(value);
	Node* p = Chain_sortFrom(FROM_R, C->mostR, op->cmp, value);
	if (p)
		Chain_rpush(C, p, node);
	else
		Chain_lpush(C, C->mostL, node);
}

void List_remove(List* L, vdata_t value)
{
	OP* op = L->op;
	vdata_t a = List_obtain(L, value);
	if (a.type && op->del)
		op->del(a);
}

void List_rremove(List* L, vdata_t value)
{
	OP* op = L->op;
	vdata_t a = List_robtain(L, value);
	if (a.type && op->del)
		op->del(a);
}

/***********************************************/
//  Dict;
/***********************************************/





