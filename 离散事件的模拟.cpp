#include "stdio.h"
#include "stdlib.h"
#include"math.h"
#define Qu 4 //客户队列数
#define Khjg 5 //两相邻到达的客户的时间间隔最大值
#define Blsj 30//每个客户办理业务的时间最大值
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
typedef int Status;
typedef int Boolean;
typedef struct
{
	int OccurTime; //事件发生时刻
	int NType;
	//事件类型，Qu表示到达事件，
	//0-Qu-1表示 Qu个窗口的离开事件
}Event,ElemType;//事件类型，有序链表 LinkList的数据元类型
typedef struct LNode // 结点类型
{
	ElemType data;
	LNode *next;
}*Link,*Position;
struct LinkList // 链表类型
{
	Link head,tail; // 分别指向线性链表中的头结点和最后一个结点
	int len; // 指示线性链表中数据元素的个数
};
typedef LinkList EventList; // 事件链表类型，定义为有序链表
typedef struct
{
	int ArrivalTime;
	int Duration;
}QElemType;
//队列的数据元素类型
typedef struct QNode
{
	QElemType data;
	QNode *next;
}*QueuePtr;
//队列
struct LinkQueue
{
	QueuePtr front,rear; // 队头、队尾指针
};
EventList ev;//事件表
Event en; //事件
Event et;//临时变量
LinkQueue q[Qu]; //Qu个队列客户
QElemType customer; //客户记录
int TotalTime=0,CustomerNum=0;//累计客户逗留时间,客户数(初值为 0)
int CloseTime; // 银行营业时间(单位是分)
int cmp(Event a,Event b)
{ // 依事件 a的发生时刻<、=或>事件 b的发生时刻分别返回-1、0或 1
	if(a.OccurTime==b.OccurTime)
		return 0;
	else
		return (a.OccurTime-b.OccurTime)/abs(a.OccurTime-b.OccurTime);
}
Status InitList(LinkList &L)
{ // 构造一个空的线性链表
	Link p;
	p=(Link)malloc(sizeof(LNode)); // 生成头结点
	if(p)
	{
		p->next=NULL;
		L.head=L.tail=p;
		L.len=0;
		return OK;
	}
	else
	{ printf("分配内存失败!");
	return ERROR;
	}
}//InitList
Status InitQueue(LinkQueue &Q)
{ // 构造一个空队列 Q
	if(!(Q.front=Q.rear=(QueuePtr)malloc(sizeof(QNode))))
		exit(-1);
	Q.front->next=NULL;
	return OK;
}//InitQueue
Status OrderInsert(LinkList &L,ElemType e,int (*comp)(ElemType,ElemType))
{ // 已知 L为有序线性链表，将元素 e按非降序插入在 L中。（用于一元多项式）
	Link o,p,q;
	q=L.head;
	p=q->next;
	while(p!=NULL&&comp(p->data,e)<0) // p不是表尾且元素值小于 e
	{
		q=p;
		p=p->next;
	}
	o=(Link)malloc(sizeof(LNode)); // 生成结点
	o->data=e; // 赋值
	q->next=o; // 插入
	o->next=p;
	L.len++; // 表长加 1
	if(!p) // 插在表尾
		L.tail=o; // 修改尾结点
	return OK;
}//OrderInsert
void OpenForDay()
{
	int i;
	InitList(ev);
	en.OccurTime=0;
	en.NType=Qu;
	OrderInsert(ev,en,cmp);//插入事件表
	for(i=0;i<Qu;++i)
		InitQueue(q[i]);
}//OpenForDay
void Random(int &d,int &i)
{
	d=rand()%Blsj+1;//1到 Blsj之间的随机数
	i=rand()%Khjg+1;
}//Random
int QueueLength(LinkQueue Q)
{ // 求队列的长度
	int i=0;
	QueuePtr p;
	p=Q.front;
	while(Q.rear!=p)
	{
		i++;
		p=p->next;
	}
	return i;
}//QueueLength
int Minimum(LinkQueue Q[]) //返回最短队列的序号
{
	int l[Qu],i,k;
	for(i=0;i<Qu;i++)
		l[i]=QueueLength(Q[i]);
	k=0;
	for(i=1;i<Qu;i++)
		if(l[i]<l[0])
		{
			l[0]=l[i];
			k=i;
		}//if
		return k;
}//Minimum
Status EnQueue(LinkQueue &Q,QElemType e)
{ // 插入元素 e为 Q的新的队尾元素
	QueuePtr p;
	if(!(p=(QueuePtr)malloc(sizeof(QNode)))) // 存储分配失败
		exit(OVERFLOW);
	p->data=e;
	p->next=NULL;
	Q.rear->next=p;
	Q.rear=p;
	return OK;
}//EnQueue
Status DeQueue(LinkQueue &Q,QElemType &e)
{ // 若队列不空,删除 Q的队头元素,用 e返回其值,并返回 OK,否则返回 ERROR
	QueuePtr p;
	if(Q.front==Q.rear)
		return ERROR;
	p=Q.front->next;
	e=p->data;
	Q.front->next=p->next;
	if(Q.rear==p)
		Q.rear=Q.front;
	free(p);
	return OK;
}//DeQueue
void CustomerArrived()
{
	QElemType f;
	int durtime,intertime,i;
	printf("到达一个客户!");
	++CustomerNum;
	Random(durtime,intertime);
	et.OccurTime=en.OccurTime+intertime; //下一客户到达时刻，隔了 intertime来了下一//个客户
	et.NType=Qu; // 队列中只有一个客户到达事件
	if(et.OccurTime<CloseTime)
		OrderInsert(ev,et,cmp); //不让下一个进入营业厅了
	i=Minimum(q);
	//处理当前进来的人
	f.ArrivalTime=en.OccurTime;
	f.Duration=durtime;
	EnQueue(q[i],f);
	printf("排于窗口 %d",i+1);
	printf("(到达时间:%d 办理业务耗时:%d)\n",en.OccurTime,durtime);
	if(QueueLength(q[i])==1)
	{
		et.OccurTime=en.OccurTime+durtime;
		et.NType=i;
		OrderInsert(ev,et,cmp); // 设定第 i队列的一个离开事件并插入事件表
	}//if
}//CustomerArrived
Status GetHead(LinkQueue Q,QElemType &e)
{ // 若队列不空,则用 e返回 Q的队头元素,并返回 OK,否则返回 ERROR
	QueuePtr p;
	if(Q.front==Q.rear)
		return ERROR;
	p=Q.front->next;
	e=p->data;
	return OK;
}//GetHead
Position GetHead(LinkList L)
{ // 返回线性链表 L中头结点的位置
	return L.head;
}//GetHead
Status QueueEmpty(LinkQueue Q)
{ // 若 Q为空队列,则返回 TRUE,否则返回 FALSE
	if(Q.front==Q.rear)
		return TRUE;
	else
		return FALSE;
}//QueueEmpty
void CustomerDeparture()
{ // 处理客户离开事件，en.NType<Qu
	int i;
	i=en.NType;
	DeQueue(q[i],customer);
	printf("服务窗口 %d: 离开一个客户!\n",i+1);
	TotalTime+=en.OccurTime-customer.ArrivalTime;
	if(!QueueEmpty(q[i])) //设定第 i队列的一个离开事件，并插入事件表
	{
		GetHead(q[i],customer);
		et.OccurTime=en.OccurTime+customer.Duration;
		et.NType=i;
		OrderInsert(ev,et,cmp);
	}//if
}//CustomerDeparture
Status ListEmpty(LinkList L)
{ // 若线性链表 L为空表，则返回 TRUE，否则返回 FALSE
	if(L.len)
		return FALSE;
	else
		return TRUE;
}//ListEmpty
Status DelFirst(LinkList &L,Link h,Link &q) // 形参增加 L,因为需修改 L
{ // h指向 L的一个结点，把 h当做头结点，删除链表中的第一个结点并以 q返回。
	// 若链表为空(h指向尾结点)，q=NULL，返回 FALSE
	q=h->next;
	if(q) // 链表非空
	{
		h->next=q->next;
		if(!h->next) // 删除尾结点
			L.tail=h; // 修改尾指针
		L.len--;
		return OK;
	}
	else
		return FALSE; // 链表空
}//DelFirst
ElemType GetCurElem(Link p)
{ // 已知 p指向线性链表中的一个结点，返回 p所指结点中数据元素的值
	return p->data;
}//GetCurElem
void Bank_Simulation()
{
	Link p;
	OpenForDay();
	while(!ListEmpty(ev))
	{
		DelFirst(ev,GetHead(ev),p);
		en.OccurTime=GetCurElem(p).OccurTime;
		en.NType=GetCurElem(p).NType;
		if(en.NType==Qu)
			CustomerArrived();
		else
			CustomerDeparture();
	}//while
	printf(" 顾 客 总 数 :%d, 所 有 顾 客 共 耗 时 :%d 分 钟 , 平 均 每 人 耗 时 : %d 分 钟\n",CustomerNum,TotalTime,TotalTime/CustomerNum);
}//Bank_Simulation
int main()
{
	printf("请输入银行营业时间长度（单位：分）\n");
	scanf("%d",&CloseTime);
	Bank_Simulation();
	return OK;
}










