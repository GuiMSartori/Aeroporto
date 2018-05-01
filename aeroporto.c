#include "aeroporto.h"
#include "aviao.h"
#include "aviao.c"
#include "semaphore.h"

/**
 * aeroporto.c
 * Implementação das funções do aeroporto.h
 * Descrições em aeroporto.h
 **/

aeroporto_t* iniciar_aeroporto (size_t* args, size_t n_args) {
	aeroporto_t * aeroporto = (aeroporto_t *) malloc(sizeof(aeroporto_t));
	aeroporto->n_pistas = args[0];
	aeroporto->n_portoes = args[1];
	aeroporto->n_esteiras = args[2];
	aeroporto->n_max_avioes_esteira = args[3];
	aeroporto->t_pouso_decolagem = args[4];
	aeroporto->t_remover_bagagens = args[5];
	aeroporto->t_inserir_bagagens = args[6];
	aeroporto->t_bagagens_esteira = args[7];
	sem_init(&aeroporto->sem_pistas, 0, args[0]);
	sem_init(&aeroporto->sem_portoes, 0, args[1]);
	aeroporto->sem_esteiras = (sem_t *) malloc(sizeof(sem_t) * aeroporto->n_esteiras);
	for(int i = 0; i < aeroporto->n_esteiras; i++) {
		sem_init(&aeroporto->sem_esteiras[i], 0, aeroporto->n_max_avioes_esteira);
	}
	return aeroporto;
}

void aproximacao_aeroporto (aeroporto_t* aeroporto, int* id) {
	printf("Aviao id:%d se aproxima do aeroporto\n", id);
}

void pousar_aviao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->sem_pistas);
	printf("Aviao id:%d esta pousando\n", aviao->id);
	usleep(aeroporto->t_pouso_decolagem);
	printf("Aviao id:%d pousou\n", aviao->id);
	sem_post(&aeroporto->sem_pistas);
}

void acoplar_portao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->sem_portoes);
	printf("Aviao id:%d se acoplou\n", aviao->id);
}

void transportar_bagagens (aeroporto_t* aeroporto, aviao_t* aviao) {
	printf("Aviao id:%d esta trasnportando as bagagens\n", aviao->id);
	usleep(aeroporto->t_inserir_bagagens);
	printf("Aviao id:%d terminou de transportar as bagagens\n", aviao->id);
	sem_post(&aeroporto->n_portoes);
}

void adicionar_bagagens_esteira (aeroporto_t* aeroporto, aviao_t* aviao) {
	int ocupar_esteira = 0;
	int esteira_ocupada = 0;
	while(ocupar_esteira == 0) {
		for(int i = 0; i < aeroporto->n_esteiras; i++) {
			if(sem_trywait(&aeroporto->sem_esteiras[i]) == 0) {
				esteira_ocupada = i;
				ocupar_esteira = 1;
				break;
			}
		}
	}
	printf("Aviao id:%d esta transferindo as bagagens a esteira\n", aviao->id);
	usleep(aeroporto->t_remover_bagagens);
	printf("Aviao id:%d terminou de transferir as bagagens a esteira\n", aviao->id);
	sem_post(&aeroporto->sem_esteiras[esteira_ocupada]);
}

void decolar_aviao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->n_pistas);
	printf("Aviao id:%d decolou\n", aviao->id);
	sem_post(&aeroporto->n_pistas);

}

int finalizar_aeroporto (aeroporto_t* aeroporto) {
	sem_destroy(&aeroporto->sem_pistas);
	sem_destroy(&aeroporto->sem_portoes);
	for(int i = 0; i < aeroporto->n_esteiras; i++) {
		sem_destroy(&aeroporto->sem_esteiras[i]);
	}
	free(aeroporto->sem_esteiras);
	free(aeroporto);
	return 0;
}
