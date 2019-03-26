#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__


#include    "includes.h"	//��������ϵͳ���ͷ�ļ�

/* HANDLE is used for sys_sem_t but we won't include windows.h */
typedef OS_EVENT* sys_sem_t;		//�����ź�������Ϊ����ϵͳ�ϵ�һ���¼�ָ��
#define SYS_SEM_NULL NULL		//���ź���
#define sys_sem_valid(sema) ((*sema) != NULL)	//�ж�semaָ����ź����Ƿ�Ϊ��
#define sys_sem_set_invalid(sema) ((*sema) = NULL)	//��semaָ����ź�����Ϊ��Ч

/* let sys.h use binary semaphores for mutexes */
/* ���廥������ص����ͺͺ�*/
#define LWIP_COMPAT_MUTEX 1		//�����ֵΪ1 ���ں��Զ������ź���������ʵ�ֻ���������

/*����������ص����ͺͺ� ����������û��λ��������ʵ��*/
#ifndef MAX_QUEUE_ENTRIES
#define MAX_QUEUE_ENTRIES 100		//��������ɻ���������Ϣ����
#endif
struct lwip_mbox {	//����ṹ�����Ͷ���
  sys_sem_t sem;	//�ź��� ������������Ϣ��ͬ������
  sys_sem_t mutex;	//������ �û����仺�����Ļ������
  void* q_mem[MAX_QUEUE_ENTRIES];	//���价�λ����� һ������ָ������ 
  u32_t head, tail;		//������ͷ����β�� head��ʾ��һ��д�����Ϣ�ٻ��λ������е�λ�� 
  						//tail ��ʾ��һ����ȡ����Ϣ�ڻ��λ������е�λ��
  
  u32_t msg_num; 	//����������Ϣ����			   	��¼�˵�ǰ�����е���Ϣ����
};
typedef struct lwip_mbox sys_mbox_t; //����������Ϣ����
#define SYS_MBOX_NULL NULL	//������
/* �ж�mboxָ��������Ƿ���Ч*/
#define sys_mbox_valid(mbox) ((mbox != NULL) && ((mbox)->sem != NULL))	
/* ��mboxָ�����������Ϊ��Ч */
#define sys_mbox_set_invalid(mbox) ((mbox)->sem = NULL)

/* DWORD (thread id) is used for sys_thread_t but we won't include windows.h */
typedef INT8U sys_thread_t; //�߳�ID Ҳ�����������ȼ�


#endif
