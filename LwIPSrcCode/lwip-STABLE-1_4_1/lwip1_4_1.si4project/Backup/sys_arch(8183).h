#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

/*
#include    "os_cpu.h"
#include    "os_cfg.h"
#include    "ucos_ii.h"


#define LWIP_STK_SIZE      2048

//yangye 2003-1-27
//defines for sys_thread_new() & sys_arch_timeouts()
//max number of lwip tasks
#define LWIP_TASK_MAX    3
//first prio of lwip tasks
#define LWIP_START_PRIO  5   //so priority of lwip tasks is from 5-9 


#define SYS_MBOX_NULL   (void*)0
#define SYS_SEM_NULL    (void*)0
#define MAX_QUEUES        10
#define MAX_QUEUE_ENTRIES 10

typedef struct {
    OS_EVENT*   pQ;
    void*       pvQEntries[MAX_QUEUE_ENTRIES];
} TQ_DESCR, *PQ_DESCR;
    
typedef OS_EVENT* sys_sem_t;
typedef PQ_DESCR  sys_mbox_t;
typedef INT8U     sys_thread_t;
*/

#include    "includes.h"

/* HANDLE is used for sys_sem_t but we won't include windows.h */
typedef OS_EVENT* sys_sem_t;
#define SYS_SEM_NULL NULL
#define sys_sem_valid(sema) ((*sema) != NULL)
#define sys_sem_set_invalid(sema) ((*sema) = NULL)

/* let sys.h use binary semaphores for mutexes */
#define LWIP_COMPAT_MUTEX 1

#ifndef MAX_QUEUE_ENTRIES
#define MAX_QUEUE_ENTRIES 100
#endif
struct lwip_mbox {
  sys_sem_t sem;
  sys_sem_t mutex;
  void* q_mem[MAX_QUEUE_ENTRIES];
  u32_t head, tail;
  u32_t msg_num; 
};
typedef struct lwip_mbox sys_mbox_t;
#define SYS_MBOX_NULL NULL
#define sys_mbox_valid(mbox) ((mbox != NULL) && ((mbox)->sem != NULL))
#define sys_mbox_set_invalid(mbox) ((mbox)->sem = NULL)

/* DWORD (thread id) is used for sys_thread_t but we won't include windows.h */
typedef INT8U sys_thread_t;



/*
//Mbox num must larger than MEMP_NUM_NETCONN
#define TOTAL_MBOX_NUM  5	//�����ں��ܹ�ʹ�õ����������Ŀ
#define MAX_MSG_IN_MBOX 100	//ÿ����������ܹ���ŵ���Ϣ��Ŀ

//�����ں�ʹ�õ�����Ľṹ
struct LWIP_MBOX_STRUCT{
	OS_EVENT * ucos_queue;						//����ucos�еĶ��л�����ʵ��
	void     *mbox_msg_entris[MAX_MSG_IN_MBOX];//�����д����Ϣ��ָ��
};

//����LwIP�ڲ�ʹ�õ���������
typedef struct LWIP_MBOX_STRUCT* sys_mbox_t;  //ϵͳ��������ָ��
typedef OS_EVENT* sys_sem_t;                  //ϵͳ�ź�������ָ��
typedef INT8U     sys_thread_t;				  //ϵͳ�����ʶ

#define LWIP_COMPAT_MUTEX 1

//�ź�NULL, ����NULL ����  
#define SYS_MBOX_NULL  (sys_mbox_t)NULL
#define SYS_SEM_NULL   (sys_sem_t)NULL
*/
#endif
