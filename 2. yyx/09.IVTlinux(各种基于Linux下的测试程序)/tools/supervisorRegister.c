#include "supervisorRegister.h"

#define 	MSGKEY				9999
#define 	MAX_PARAM			10
#define     MAX_PATH_LEN		256
#define 	MAX_ARGV_LEN		32

typedef enum _cmdType
{
	cmdRegister,		/*ע��*/
   	cmdCancel,			/*ע��*/
   	cmdSuspend			/*��ͣ*/
}cmdType_t;

typedef struct argStr_t{
	char strSt[MAX_ARGV_LEN]; 
}argStr;

typedef struct msgStruct{
	cmdType_t cmdType;						//��������
	pid_t pid;								//����PID	
 	char executablePath[MAX_PATH_LEN];		//����ִ��·��
	int argc;								//��������
	argStr argv[MAX_PARAM];					//����
}msgStruct;

typedef struct msgbuf {
	long mType;//��Ϣ���͡�ʹ����Ϣ���б���Ҫ�������
	msgStruct data;
}msgBuf;

int getExecutablePath(char *executablePath)
{
	if(executablePath)
	{
		char dir[PATH_MAX] = {0};
		int nRead = readlink("/proc/self/exe", dir, PATH_MAX);

		if(nRead < MAX_PATH_LEN)
		{
			memcpy(executablePath,dir,nRead);
			printf("getExecutablePath:%s\n" ,executablePath);
			return 1;
		}
	}
	return 0;
}

int dogRegisterMsg(msgStruct* dogRegister)
{
	int msgid = -1,sndRet = -1 ,iTimes = 0;
	msgBuf sendMsg;

	//������Ϣ����
	msgid = msgget((key_t)MSGKEY, 0666 | IPC_CREAT);
	if(msgid == -1)
	{
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	sendMsg.mType = 1;//����Ҫ�� 
	memcpy((void*)&sendMsg.data ,dogRegister ,sizeof(msgStruct));
	#if 1
	while((sndRet==-1)&&(iTimes<10))//����Ϣ������д��Ϣ��ֱ��д��ɹ�
	{	
		//����з�������
		sndRet = msgsnd(msgid, (void*)&sendMsg, sizeof(msgStruct), IPC_NOWAIT);
		if(sndRet < 0)
		{
			perror("msgsnd failed");
        }     
        usleep(100);
		iTimes++;
    }
	#else
	sndRet = msgsnd(msgid, (void*)&sendMsg, sizeof(msgStruct), IPC_NOWAIT);
	if(sndRet < 0)
	{
		fprintf(stderr, "msgsnd failed\n");
    }
	#endif

    return 0;
	
}

int getPid(pid_t* pidNo)
{
    *pidNo = getpid();
    printf("pid no = %d\n", *pidNo);
	return 1;
}

void signalCancelHandler()  
{  
	msgStruct dogRegister;

	memset((void*)&dogRegister , 0 ,sizeof(msgStruct));
	dogRegister.cmdType = cmdCancel;
	getPid(&dogRegister.pid);
	dogRegisterMsg(&dogRegister);
	printf("cmdCancel\n");
    
}

#if 0
int main(int argc, char **argv)
{
	msgStruct dogRegister;
	int i = 0;

	memset((void*)&dogRegister , 0 ,sizeof(msgStruct));
	dogRegister.cmdType = cmdRegister;
	getPid(&dogRegister.pid);
	getExecutablePath(dogRegister.executablePath);
	dogRegister.argc = argc;	
	for(i=0;(i<argc)&&(i<MAX_PARAM);i++)
	{
		if(strlen(argv[i]) < MAX_ARGV_LEN)
		memcpy(dogRegister.argv[i].strSt,argv[i],strlen(argv[i]));
	}
	
	dogRegisterMsg(&dogRegister);
	
	if(signal(SIGTERM,sigHandler) == SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }
	
	if(signal(SIGINT,sigHandler) == SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }

	if(signal(SIGHUP,sigHandler)== SIG_ERR){  
        perror("signal errror");  
        exit(EXIT_FAILURE);  
    }
	
	while(1)
	{	
		sleep(1);
	}
	return 0;
}
#endif

int supervisorRegister(int argc, char **argv)
{
	msgStruct dogRegister;
	int i = 0;

	memset((void*)&dogRegister , 0 ,sizeof(msgStruct));
	dogRegister.cmdType = cmdRegister;
	getPid(&dogRegister.pid);
	getExecutablePath(dogRegister.executablePath);
	dogRegister.argc = argc;	
	for(i=0;(i<argc)&&(i<MAX_PARAM);i++)
	{
		if(strlen(argv[i]) < MAX_ARGV_LEN)
		memcpy(dogRegister.argv[i].strSt,argv[i],strlen(argv[i]));
	}
	
	dogRegisterMsg(&dogRegister);
	
	return 0;
}


