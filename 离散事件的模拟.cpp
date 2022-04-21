#include "stdio.h"
#include "stdlib.h"
#include"math.h"
#define Qu 4 //�ͻ�������
#define Khjg 5 //�����ڵ���Ŀͻ���ʱ�������ֵ
#define Blsj 30//ÿ���ͻ�����ҵ���ʱ�����ֵ
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0
typedef int Status;
typedef int Boolean;
typedef struct
{
	int OccurTime; //�¼�����ʱ��
	int NType;
	//�¼����ͣ�Qu��ʾ�����¼���
	//0-Qu-1��ʾ Qu�����ڵ��뿪�¼�
}Event,ElemType;//�¼����ͣ��������� LinkList������Ԫ����
typedef struct LNode // �������
{
	ElemType data;
	LNode *next;
}*Link,*Position;
struct LinkList // ��������
{
	Link head,tail; // �ֱ�ָ�����������е�ͷ�������һ�����
	int len; // ָʾ��������������Ԫ�صĸ���
};
typedef LinkList EventList; // �¼��������ͣ�����Ϊ��������
typedef struct
{
	int ArrivalTime;
	int Duration;
}QElemType;
//���е�����Ԫ������
typedef struct QNode
{
	QElemType data;
	QNode *next;
}*QueuePtr;
//����
struct LinkQueue
{
	QueuePtr front,rear; // ��ͷ����βָ��
};
EventList ev;//�¼���
Event en; //�¼�
Event et;//��ʱ����
LinkQueue q[Qu]; //Qu�����пͻ�
QElemType customer; //�ͻ���¼
int TotalTime=0,CustomerNum=0;//�ۼƿͻ�����ʱ��,�ͻ���(��ֵΪ 0)
int CloseTime; // ����Ӫҵʱ��(��λ�Ƿ�)
int cmp(Event a,Event b)
{ // ���¼� a�ķ���ʱ��<��=��>�¼� b�ķ���ʱ�̷ֱ𷵻�-1��0�� 1
	if(a.OccurTime==b.OccurTime)
		return 0;
	else
		return (a.OccurTime-b.OccurTime)/abs(a.OccurTime-b.OccurTime);
}
Status InitList(LinkList &L)
{ // ����һ���յ���������
	Link p;
	p=(Link)malloc(sizeof(LNode)); // ����ͷ���
	if(p)
	{
		p->next=NULL;
		L.head=L.tail=p;
		L.len=0;
		return OK;
	}
	else
	{ printf("�����ڴ�ʧ��!");
	return ERROR;
	}
}//InitList
Status InitQueue(LinkQueue &Q)
{ // ����һ���ն��� Q
	if(!(Q.front=Q.rear=(QueuePtr)malloc(sizeof(QNode))))
		exit(-1);
	Q.front->next=NULL;
	return OK;
}//InitQueue
Status OrderInsert(LinkList &L,ElemType e,int (*comp)(ElemType,ElemType))
{ // ��֪ LΪ��������������Ԫ�� e���ǽ�������� L�С�������һԪ����ʽ��
	Link o,p,q;
	q=L.head;
	p=q->next;
	while(p!=NULL&&comp(p->data,e)<0) // p���Ǳ�β��Ԫ��ֵС�� e
	{
		q=p;
		p=p->next;
	}
	o=(Link)malloc(sizeof(LNode)); // ���ɽ��
	o->data=e; // ��ֵ
	q->next=o; // ����
	o->next=p;
	L.len++; // ���� 1
	if(!p) // ���ڱ�β
		L.tail=o; // �޸�β���
	return OK;
}//OrderInsert
void OpenForDay()
{
	int i;
	InitList(ev);
	en.OccurTime=0;
	en.NType=Qu;
	OrderInsert(ev,en,cmp);//�����¼���
	for(i=0;i<Qu;++i)
		InitQueue(q[i]);
}//OpenForDay
void Random(int &d,int &i)
{
	d=rand()%Blsj+1;//1�� Blsj֮��������
	i=rand()%Khjg+1;
}//Random
int QueueLength(LinkQueue Q)
{ // ����еĳ���
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
int Minimum(LinkQueue Q[]) //������̶��е����
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
{ // ����Ԫ�� eΪ Q���µĶ�βԪ��
	QueuePtr p;
	if(!(p=(QueuePtr)malloc(sizeof(QNode)))) // �洢����ʧ��
		exit(OVERFLOW);
	p->data=e;
	p->next=NULL;
	Q.rear->next=p;
	Q.rear=p;
	return OK;
}//EnQueue
Status DeQueue(LinkQueue &Q,QElemType &e)
{ // �����в���,ɾ�� Q�Ķ�ͷԪ��,�� e������ֵ,������ OK,���򷵻� ERROR
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
	printf("����һ���ͻ�!");
	++CustomerNum;
	Random(durtime,intertime);
	et.OccurTime=en.OccurTime+intertime; //��һ�ͻ�����ʱ�̣����� intertime������һ//���ͻ�
	et.NType=Qu; // ������ֻ��һ���ͻ������¼�
	if(et.OccurTime<CloseTime)
		OrderInsert(ev,et,cmp); //������һ������Ӫҵ����
	i=Minimum(q);
	//����ǰ��������
	f.ArrivalTime=en.OccurTime;
	f.Duration=durtime;
	EnQueue(q[i],f);
	printf("���ڴ��� %d",i+1);
	printf("(����ʱ��:%d ����ҵ���ʱ:%d)\n",en.OccurTime,durtime);
	if(QueueLength(q[i])==1)
	{
		et.OccurTime=en.OccurTime+durtime;
		et.NType=i;
		OrderInsert(ev,et,cmp); // �趨�� i���е�һ���뿪�¼��������¼���
	}//if
}//CustomerArrived
Status GetHead(LinkQueue Q,QElemType &e)
{ // �����в���,���� e���� Q�Ķ�ͷԪ��,������ OK,���򷵻� ERROR
	QueuePtr p;
	if(Q.front==Q.rear)
		return ERROR;
	p=Q.front->next;
	e=p->data;
	return OK;
}//GetHead
Position GetHead(LinkList L)
{ // ������������ L��ͷ����λ��
	return L.head;
}//GetHead
Status QueueEmpty(LinkQueue Q)
{ // �� QΪ�ն���,�򷵻� TRUE,���򷵻� FALSE
	if(Q.front==Q.rear)
		return TRUE;
	else
		return FALSE;
}//QueueEmpty
void CustomerDeparture()
{ // ����ͻ��뿪�¼���en.NType<Qu
	int i;
	i=en.NType;
	DeQueue(q[i],customer);
	printf("���񴰿� %d: �뿪һ���ͻ�!\n",i+1);
	TotalTime+=en.OccurTime-customer.ArrivalTime;
	if(!QueueEmpty(q[i])) //�趨�� i���е�һ���뿪�¼����������¼���
	{
		GetHead(q[i],customer);
		et.OccurTime=en.OccurTime+customer.Duration;
		et.NType=i;
		OrderInsert(ev,et,cmp);
	}//if
}//CustomerDeparture
Status ListEmpty(LinkList L)
{ // ���������� LΪ�ձ��򷵻� TRUE�����򷵻� FALSE
	if(L.len)
		return FALSE;
	else
		return TRUE;
}//ListEmpty
Status DelFirst(LinkList &L,Link h,Link &q) // �β����� L,��Ϊ���޸� L
{ // hָ�� L��һ����㣬�� h����ͷ��㣬ɾ�������еĵ�һ����㲢�� q���ء�
	// ������Ϊ��(hָ��β���)��q=NULL������ FALSE
	q=h->next;
	if(q) // ����ǿ�
	{
		h->next=q->next;
		if(!h->next) // ɾ��β���
			L.tail=h; // �޸�βָ��
		L.len--;
		return OK;
	}
	else
		return FALSE; // �����
}//DelFirst
ElemType GetCurElem(Link p)
{ // ��֪ pָ�����������е�һ����㣬���� p��ָ���������Ԫ�ص�ֵ
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
	printf(" �� �� �� �� :%d, �� �� �� �� �� �� ʱ :%d �� �� , ƽ �� ÿ �� �� ʱ : %d �� ��\n",CustomerNum,TotalTime,TotalTime/CustomerNum);
}//Bank_Simulation
int main()
{
	printf("����������Ӫҵʱ�䳤�ȣ���λ���֣�\n");
	scanf("%d",&CloseTime);
	Bank_Simulation();
	return OK;
}










