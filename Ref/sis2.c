#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
//#include "sis2.h"

#define STACKSIZE 10485760 
#define MAXTHREADS 129
#define ERRO -1


typedef enum
{
	READY,
	RUNNING,
	BLOCKED
} State;

struct TCB
{
	int tid;
	ucontext_t* context;
	State state;
	struct TCB *Next;
	struct Dependency *waiting_for_me;
};

struct Dependency
{
	struct TCB *depende;
	struct Dependency *Next;
};

struct umutex_t	//tipo de estrutura que ele pede pras flags (semaforos)
{
	int flag;
	struct Locked  *waiting_lock;
};

struct Locked		  //lista para TCB's esperando uma determinada flag liberar
{
	struct TCB *tcb_lock;
	struct Locked *Proximo;
};


int NumberThreads;
int CounterTid;			    
struct TCB *List_READY;            // o ponteiro da lista Ready  aptos apontar para a thread.  
struct TCB *RUNNING_Thread; 
struct TCB *List_BLOCKED;
ucontext_t *contexto_saida;

void funcao_saida()
{
	struct TCB *PosAtual, *PosAnt;
	struct TCB *thread = RUNNING_Thread;
	struct Dependency *pointer = thread->waiting_for_me;
	struct Dependency *pointerAnt;

	printf("Fim da %d.\n",RUNNING_Thread->tid);

	//Desbloqueia as theads que estão esperando o termino desta
	while(pointer != NULL)
	{	
		PosAtual = List_BLOCKED;
		PosAnt = NULL;

		if(pointer->depende->tid == List_BLOCKED->tid)
		{
			List_BLOCKED = List_BLOCKED->Next;
		}
		else
		{
			while(pointer->depende->tid != PosAtual->tid)	//procura na lista de blocked
			{
				PosAnt = PosAtual;
				PosAtual = PosAtual->Next;
			}
			PosAnt->Next = PosAtual->Next;
		}
		//tira da lista de blocked

		printf("Liberando a %d.\n",PosAtual->tid);

		//troca o estado da thread que saiu
		PosAtual->state = READY;
		PosAtual->Next = NULL;

		//procura o fim a lista Ready para coloca-la
		if(List_READY == NULL)
		{
			List_READY = PosAtual;
		}
		else
		{
			PosAnt = List_READY;
			while(PosAnt->Next != NULL)	
			{
				PosAnt = PosAnt->Next;
			}
		
			PosAnt->Next = PosAtual;
		}

		pointerAnt = pointer;
		pointer = pointer->Next;	//segue o laço
		free(pointerAnt);		//libera memoria da lista de dependencias
	}

	NumberThreads--;

	// Libera memória da thread
	free(thread); 

	//coloca a próxima thread para executar;
	RUNNING_Thread = List_READY;
	List_READY = List_READY->Next;
	RUNNING_Thread->Next = NULL;
	RUNNING_Thread->state=RUNNING;

	printf("Chamando a %d.\n",RUNNING_Thread->tid);

	setcontext(RUNNING_Thread->context);

}


int libuthread_init()  
{
	CounterTid=0;
	NumberThreads=0;
	List_READY= NULL;
	RUNNING_Thread=NULL;
	List_BLOCKED=NULL;
	ucontext_t *main_Context = (ucontext_t*)malloc(sizeof(ucontext_t));
	struct TCB *main_Thread = (struct TCB*)malloc(sizeof(struct TCB));
	contexto_saida = (ucontext_t*)malloc(sizeof(ucontext_t));

	getcontext(contexto_saida);	// Criando o contexto de saída das threads
		contexto_saida->uc_stack.ss_sp = mmap(NULL,STACKSIZE,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_GROWSDOWN|MAP_ANONYMOUS,-1,0);
		contexto_saida->uc_stack.ss_size =STACKSIZE;   
		contexto_saida->uc_link = NULL;  // para onde vai quando a thread acaba...
		makecontext(contexto_saida, (void (*)(void))funcao_saida, 0);


	getcontext(main_Context);
	main_Thread->tid = CounterTid;
	main_Thread->context = main_Context;
	main_Thread->state = RUNNING;
	main_Thread->waiting_for_me = NULL;
	main_Thread->Next = NULL;

	RUNNING_Thread = main_Thread;

	CounterTid++;
	NumberThreads++;

	return 0; //OK
}

int uthread_create(void * (*start_routine)(void*), void * arg)
{   
	  
	   ucontext_t* main_Context= (ucontext_t*)malloc(sizeof(ucontext_t));
	   ucontext_t* thread_context = (ucontext_t*)malloc(sizeof(ucontext_t)); // aloca contexto;
	   struct TCB *PosAtual;
	   struct TCB *newThread = (struct TCB*)malloc(sizeof(struct TCB));       // cria a thread

	   
	   if(thread_context==NULL )  // se der problema ao alocar contexto.
	   {  
	       return -1;
	   }
	   else
	   {    
		getcontext(main_Context);   // salva atual contexto
		thread_context = main_Context;
		thread_context->uc_stack.ss_sp = mmap(NULL,STACKSIZE,PROT_WRITE|PROT_READ,MAP_PRIVATE|MAP_GROWSDOWN|MAP_ANONYMOUS,-1,0);
		thread_context->uc_stack.ss_size =STACKSIZE;   
		thread_context->uc_link = contexto_saida;  // aponta para o contexto com a função de saída

		makecontext(thread_context, (void (*)(void))start_routine, 1, arg);  
	   }   
		
	   if(NumberThreads + 1 > MAXTHREADS) // Confere se criação não estora o limite maximo 
	   {
	       return ERRO;
	   }
	   
	   if(newThread == NULL)
	   {
	       return ERRO;
	   }
	   else
	   {
	       newThread->tid =CounterTid;         //seta a thread
	       newThread->context = thread_context;
	       newThread->state = READY;
	       newThread->Next = NULL;
	       newThread->waiting_for_me = NULL;
	   }
	   
	if(List_READY == NULL)        
	{   
	       List_READY = newThread;  // Seta ponteiro Inicial da Lista. 
	}    

	else
	{   
		PosAtual= List_READY;

		while(PosAtual->Next!=NULL)	// Acha a última posição da lista Ready;
		{        
			PosAtual = PosAtual->Next; 
		}

		PosAtual->Next = newThread;  // insere no fim da lista;            
	}

	CounterTid++;
	NumberThreads++;

	return newThread->tid; // Contains thread id
}

int uthread_yield()
{	
	int Save;
	
	struct TCB *PosAtual = List_READY;
	printf("Yield da %d.\n",RUNNING_Thread->tid);
	
	if(PosAtual == NULL)
	{
		return 0;
	}
	else
	{
		while(PosAtual->Next!=NULL)		// Acha a última posição da lista Ready;
		{        
			PosAtual = PosAtual->Next; 	
		}

		PosAtual->Next = RUNNING_Thread;	// Coloca a thread que esta rodando no fim da list READY
		RUNNING_Thread->state=READY;
		RUNNING_Thread->Next = NULL;

		getcontext(RUNNING_Thread->context);	//salva o contexto atual;

		if(RUNNING_Thread->state == RUNNING)	//verificação para o retorno de contexto sair da função
		{
			return 0;
		}

		printf("Proximo a entrar eh %d com STATE %d.\n",List_READY->tid,List_READY->state);
		
		RUNNING_Thread = List_READY; 		// coloca a 1ª thread para executar;
		List_READY = List_READY->Next;
		RUNNING_Thread->state = RUNNING;

		printf("Yield para %d.\n",RUNNING_Thread->tid);

		setcontext(RUNNING_Thread->context);

		return 0;
	}
}

int uthread_join(int thr)
{
	struct TCB *PosAtual = List_READY;	
	struct Dependency *waited, *newWaited;

	newWaited = (struct Dependency*)malloc(sizeof(struct Dependency));
	newWaited->depende=RUNNING_Thread;
	newWaited->Next = NULL;

	printf("Join da %d, por causa da %d.\n",RUNNING_Thread->tid,thr);

	// Procurando nas duas listas Ready e Blocked

	if(List_READY == NULL) 		// Primeiro na Lista Ready
	{
		if(List_BLOCKED == NULL)	// READY e BLOCKED vazias, thread thr nao existe
		{
			printf("Estou na %d e saindo por nao achar a %d (Listas Vazias).\n",RUNNING_Thread->tid,thr);
			return 0;
		}
		else	// Procura o TCB da Thread que tranca no BLOCKED
		{
			PosAtual = List_BLOCKED;

			while(PosAtual->tid != thr  && PosAtual->Next != NULL)	
			{
				printf("So imprimindo pq passei pela %d na BLOCKED.\n",PosAtual->tid);		
				PosAtual = PosAtual->Next;	
			}
			if(PosAtual->tid != thr)	// Ready Vazia e não encontrou na BLOCKED, então a thread thr já acabou
			{
				printf("Estou na %d e estou saindo por nao achar a %d (READY vazia).\n",RUNNING_Thread->tid,thr);
				return 0;
			}
		}	
	}
	else	// Procuro na READY
	{
		PosAtual = List_READY;

		while(PosAtual->tid != thr  && PosAtual->Next != NULL)	// Procura o TCB da Thread que tranca no READY.
		{
			printf("So imprimindo pq passei pela %d na READY.\n",PosAtual->tid);		
			PosAtual = PosAtual->Next;	
		}

		if(PosAtual->tid != thr)	// Não encontrou na READY
		{
			if(List_BLOCKED == NULL)	// BLOCKED Vazia, então a thread nao existe
			{
				printf("Estou na %d e estou saindo por nao achar a %d (BLOCKED Vazia).\n",RUNNING_Thread->tid,thr);
				return 0;
			}
			else	// Procura o TCB da Thread que tranca no BLOCKED
			{
				PosAtual = List_BLOCKED;

				while(PosAtual->tid != thr  && PosAtual->Next != NULL)	
				{
					printf("So imprimindo pq passei pela %d na BLOCKED.\n",PosAtual->tid);		
					PosAtual = PosAtual->Next;	
				}
				if(PosAtual->tid != thr)	// Não encontrou na READY nem na BLOCKED, então a thread thr já acabou
				{
					printf("Estou na %d e estou saindo por nao achar a %d (READY vazia).\n",RUNNING_Thread->tid,thr);
					return 0;
				}
			}	
		}
			
	}

	printf("Busquei a %d e achei a %d.\n",thr,PosAtual->tid);

	// PosAtual aqui tem a posição da thread thr
	waited = PosAtual->waiting_for_me;	
	
	if(PosAtual->waiting_for_me == NULL)
	{		
		PosAtual->waiting_for_me = newWaited;
	}
	else 	//newWaited!=NULL acha ultima posição da lista de dependencia.
	{
		while(waited->Next != NULL)
		{
			waited = waited->Next;
		}
		waited->Next = newWaited;	
		
	}

	printf("Dizendo que a %d depende dela.\n",PosAtual->waiting_for_me->depende->tid);

	RUNNING_Thread->state = BLOCKED; 	//Bloqueia a thread em execução e coloca ela na lista BLOCKED.

	getcontext(RUNNING_Thread->context);

	if(RUNNING_Thread->state == RUNNING)	//verificação para o retorno de contexto sair da função
		return 0;
		
	if(List_BLOCKED ==NULL)
	{
		List_BLOCKED = RUNNING_Thread;
		printf("Bloqueando a %d.\n",List_BLOCKED->tid);
		List_BLOCKED->Next = NULL;
	}
	else
	{
		PosAtual = List_BLOCKED;
		while(PosAtual->Next !=NULL)
		{
			PosAtual = PosAtual->Next;
		}

		PosAtual->Next = RUNNING_Thread;
		printf("Bloqueando a %d.\n",PosAtual->Next->tid);
		PosAtual->Next->Next = NULL;

	}

	if(List_READY==NULL)
		printf("Ninguem pra executar. Fudeu!\n");
	
	RUNNING_Thread = List_READY; 	// coloca a próxima thread para executar;
	List_READY = List_READY->Next;
	RUNNING_Thread->Next = NULL;
	RUNNING_Thread->state=RUNNING;

	printf("Join foi pra %d.\n",RUNNING_Thread->tid);
	
	setcontext(RUNNING_Thread->context);
	return 0;
}



int uthread_lock(struct umutex_t *lock)
{
	struct Locked *Pointer;	//ponteiros auxiliares
	struct Locked *novoItem = (struct Locked*)malloc(sizeof(struct Locked)); 	//cria novo elemnto de espera;
	struct TCB *PosAtual;
	int ja_estah = 0;

	// Faz a troca de contexto
	getcontext(RUNNING_Thread->context);	// Salva o contexto

	printf("Começando o lock da %d.\n",RUNNING_Thread->tid);

	if(lock->flag == 1) //se a flag estiver ativada (1), a sessão esta ocupada
	{
		novoItem->tcb_lock = RUNNING_Thread; // Cria um novo item pra espera com TCB da thread atual
		novoItem->Proximo = NULL;
	
		if(lock->waiting_lock == NULL) 	//se a espera estiver vazia ele é o primeiro
		{
			lock->waiting_lock = novoItem ; 
		}
		else 		//se a espera tiver elementos, procura o fim da lista e insere
		{
			Pointer = lock->waiting_lock;
			while(Pointer->Proximo !=NULL)
			{
				Pointer = Pointer->Proximo;
			}
			Pointer->Proximo = novoItem;
		}	

		RUNNING_Thread->state = BLOCKED; 	// Bloqueia a thread em execução e coloca ela na lista BLOCKED.

		if(List_BLOCKED ==NULL)		// Insere na lista Blocked
		{
			List_BLOCKED = RUNNING_Thread;
			printf("Bloqueando a %d por Lock.\n",List_BLOCKED->tid);
			List_BLOCKED->Next = NULL;
		}
		else
		{
			PosAtual = List_BLOCKED;
			while(PosAtual->Next !=NULL)
			{
				if(PosAtual == RUNNING_Thread)	// Testa se a thread ja estah na lista de bloqueadas, daí nao insere
				{
					ja_estah = 1;	
				}
				PosAtual = PosAtual->Next;
			}
			if(ja_estah == 0)
			{
				PosAtual->Next = RUNNING_Thread;
				printf("Bloqueando a %d por lock.\n",PosAtual->Next->tid);
				PosAtual->Next->Next = NULL;
			}
			else
			{
				printf("A thread %d ja estah bloqueada (lock).\n",RUNNING_Thread->tid);
			}
		}

		
		RUNNING_Thread = List_READY; 	// coloca a próxima thread para executar;
		List_READY = List_READY->Next;
		RUNNING_Thread->Next = NULL;
		RUNNING_Thread->state=RUNNING;

		printf("Lock chamou a %d.\n",RUNNING_Thread->tid);

		setcontext(RUNNING_Thread->context);
	}
	else	//se a flag estiver livre (0), marca-a como ocupada
	{
		printf("Lock ocupou a flag com a thread %d.\n",RUNNING_Thread->tid);		
		lock->flag = 1;
		lock->waiting_lock = NULL;
	}
	return 0;
}


int uthread_unlock (struct umutex_t *lock)
{
	struct Locked *Pointer;	//ponteiros auxiliares
	struct TCB *PosAtual, *Saindo, *PosAnt; 
	
	if(lock->flag == 1)	// Se a flag estiver ocupada
	{
		printf("Liberando a flag.\n");
		lock->flag = 0;		// Desocupa a flag 

		if(lock->waiting_lock == NULL)	//Lista de espera vazia, encerra a verificação
		{
			printf("Ninguem na espera.\n");
			return 0;
		}
		else
		{		// Retira todas as thread da lista de espera
			Pointer = lock->waiting_lock;
			while(Pointer != NULL)
			{
				Saindo = Pointer->tcb_lock;
				//Pointer = Pointer->Proximo;
						
				if(Pointer != NULL)
					printf("A %d eh a proxima na ESPERA.\n",Pointer->tcb_lock->tid);

				if(Pointer->Proximo != NULL)
					printf("A %d eh a outra proxima na ESPERA.\n",Pointer->Proximo->tcb_lock->tid);


				printf("Retirando da espera e desbloqueando a thread %d.\n",Saindo->tid);

				// Removendo da lista BLOCKED
				if(List_BLOCKED->tid == Saindo->tid) // Se for a primeira entrada
				{
					List_BLOCKED = List_BLOCKED->Next;
				}
				else			// Se estiver no meio
				{
					PosAtual = List_BLOCKED;
					while(PosAtual->tid != Saindo->tid)
					{	
						PosAnt = PosAtual;
						PosAtual = PosAtual->Next;
					}

					PosAnt->Next = PosAtual->Next;
				}

				Saindo->state = READY;		// Insere a thread no fim da lista READY
				Saindo->Next = NULL;

				PosAnt= List_READY;

				if(List_READY == NULL)
				{
					List_READY = Saindo;
				}
				else
				{
					while( PosAnt->Next != NULL)
					{	
						PosAnt= PosAnt->Next;
					}
					PosAnt->Next = Saindo;
				}
				Pointer = Pointer->Proximo;
			}
		}
	return 0;
	}
	else
		printf("Erro! A flag nao esta ocupada.");
		return ERRO; // Flag nao esta ocupada
}
