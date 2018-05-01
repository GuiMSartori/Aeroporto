#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "semaphore.h"

#include "aeroporto.h"
#include "aeroporto.c"
#include "aviao.h"
#include "aviao.c"
#include "fila.h"
#include "fila.c"

#define NOVO_AVIAO_MIN 30
#define NOVO_AVIAO_MAX 120
#define COMBUSTIVEL_MIN 1
#define COMBUSTIVEL_MAX 10
#define TEMPO_POUSO_DECOLAGEM 40
#define TEMPO_REMOVER_BAGAGENS 90
#define TEMPO_INSERIR_BAGAGENS 110
#define TEMPO_BAGAGENS_ESTEIRA 200
#define TEMPO_SIMULACAO 10000

int rodar_programa = 1;
size_t t_novo_aviao_min, t_novo_aviao_max, t_novo_aviao, t_simulacao;
size_t p_combustivel_min, p_combustivel_max;
fila_ordenada_t * fila_avioes;
aeroporto_t* meu_aeroporto;


//Thread que controla o tempo de execucao do programa
void * cronometro(void *arg) {
	usleep(t_simulacao);
	rodar_programa = 0;
	pthread_exit(NULL);
}

//Thread que controla a rotina do aviao
void * rotina_aviao(void *arg) {
	int id_aviao = *((int *) arg);
	if(rodar_programa == 1) {
		//1.Aproximação ao aeroporto
		aproximacao_aeroporto(meu_aeroporto, &id_aviao);
		//2.Pouso.
		//while(id_aviao != fila_avioes->primeiro->dado->id){}

		printf("Tamanho antes de remover %ld: \n", fila_avioes->n_elementos);
		fflush(stdout);
		aviao_t * aviao = remover(fila_avioes);
		printf("Tamanho depois de remover %ld: \n", fila_avioes->n_elementos);
		fflush(stdout);
		pousar_aviao(meu_aeroporto, aviao);

		//3.Acoplagem a um portão.
		acoplar_portao(meu_aeroporto, aviao);

		//4.1Desembarque/Retirada das bagagens
		adicionar_bagagens_esteira(meu_aeroporto, aviao);

		//4.2Embarque/Transporte de bagagens.
		transportar_bagagens(meu_aeroporto, aviao);

		//5.Decolagem.
		decolar_aviao(meu_aeroporto, aviao);
		desaloca_aviao(aviao);
	}
	pthread_exit(NULL);
}

//Thread que cria os avioes
void * fabrica_aviao(void *arg) {
	int ini_id = 0;
	srand(time(NULL));
	while(rodar_programa == 1) {
		usleep(t_novo_aviao);  //Tempo de criacao de um novo aviao
		int ini_combustivel = rand() % (p_combustivel_max + 1 - p_combustivel_min) + p_combustivel_min;
		aviao_t * aviao = aloca_aviao(ini_combustivel, ini_id);
		printf("Tamanho antes de inserir %ld: \n", fila_avioes->n_elementos);
		fflush(stdout);
		inserir(fila_avioes, aviao);
		printf("Tamanho antes de depois %ld: \n", fila_avioes->n_elementos);
		fflush(stdout);
		ini_id++;
		pthread_create(&aviao->thread, NULL, rotina_aviao, (void *)&aviao->id);
		t_novo_aviao = rand() % (t_novo_aviao_max + 1 - t_novo_aviao_min) + t_novo_aviao_min;
	}
	pthread_exit(NULL);
}

int main (int argc, char** argv) {

	// Variáveis temporais (inicio t_)
	//size_t t_novo_aviao_min, t_novo_aviao_max;
	size_t t_pouso_decolagem;
	size_t t_remover_bagagens, t_inserir_bagagens;
	size_t t_bagagens_esteira;

	// Variáveis discretas (inicio n_)
	size_t n_pistas, n_portoes;
	size_t n_max_avioes_esteira, n_esteiras;
	size_t n_args;

	// Variáveis de prioridade (inicio p_)
	//size_t p_combustivel_min, p_combustivel_max;

	if (argc == 5) { // Argumentos sem tempos de execução
		t_novo_aviao_min = NOVO_AVIAO_MIN;
		t_novo_aviao_max = NOVO_AVIAO_MAX;
		t_pouso_decolagem = TEMPO_POUSO_DECOLAGEM;
		t_remover_bagagens = TEMPO_REMOVER_BAGAGENS;
		t_inserir_bagagens = TEMPO_INSERIR_BAGAGENS;
		t_bagagens_esteira = TEMPO_BAGAGENS_ESTEIRA;
		t_simulacao = TEMPO_SIMULACAO;
		p_combustivel_min = COMBUSTIVEL_MIN;
		p_combustivel_max = COMBUSTIVEL_MAX;
		n_pistas = atoi(argv[1]);
		n_portoes = atoi(argv[2]);
		n_max_avioes_esteira = atoi(argv[3]);
		n_esteiras = atoi(argv[4]);

	} else if (argc == 14) { // Argumentos com tempos de execução
		int i = 0; // Este contador será incrementado antes de coletar um argumento
		t_novo_aviao_min = atoi(argv[++i]);
		t_novo_aviao_max = atoi(argv[++i]);
		p_combustivel_min = atoi(argv[++i]);
		p_combustivel_max = atoi(argv[++i]);
		t_pouso_decolagem = atoi(argv[++i]);
		n_pistas = atoi(argv[++i]);
		t_remover_bagagens = atoi(argv[++i]);
		t_inserir_bagagens = atoi(argv[++i]);
		n_portoes = atoi(argv[++i]);
		n_max_avioes_esteira = atoi(argv[++i]);
		t_bagagens_esteira = atoi(argv[++i]);
		n_esteiras = atoi(argv[++i]);
		t_simulacao = atoi(argv[++i]);

	} else { // Número incorreto de argumentos
		printf("Todas as entradas são inteiros positivos!!\nUso:\n");
		printf("./aeroporto  NOVO_AVIAO_MIN  NOVO_AVIAO_MAX\n");
		printf("COMBUSTIVEL_MIN COMBUSTIVEL_MAX\n");
		printf("TEMPO_POUSO_DECOLAGEM  NUMERO_PISTAS  TEMPO_REMOVER_BAGAGENS\n");
		printf("TEMPO_INSERIR_BAGAGENS  NUMERO_PORTOES  MAXIMO_AVIOES_ESTEIRA\n");
		printf("TEMPO_BAGAGENS_ESTEIRA  NUMERO_ESTEIRAS  TEMPO_SIMULACAO\n");
		printf("----------OU----------\n");
		printf("./airport  NUMERO_PISTAS  NUMERO_PORTOES  MAXIMO_AVIOES_ESTEIRA  NUMERO_ESTEIRAS\n");
		return 0;
	}

	// Impressão com os parâmetros selecionados para simulação
	printf("Simulação iniciada com tempo total: %zu\n", t_simulacao);
	printf("Tempo para criação de aviões: %zu - %zu\n", t_novo_aviao_min, t_novo_aviao_max);
	printf("Número de pistas de pouso: %zu\n", n_pistas);
	printf("Tempo de pouso e decolagem: %zu\n", t_pouso_decolagem);
	printf("Número de portões de embarque: %zu\n", n_portoes);
	printf("Tempo de inserção (%zu) e remoção (%zu) de bagagens\n", t_inserir_bagagens, t_remover_bagagens);
	printf("Número de esteiras: %zu, com %zu aviões por esteira\n", n_esteiras, n_max_avioes_esteira);
	printf("Tempo das bagagens nas esteiras: %zu\n", t_bagagens_esteira);
	// Inicialização do aeroporto
	n_args = 8;
	size_t args[8] = {n_pistas, n_portoes, n_esteiras,
				n_max_avioes_esteira,
				t_pouso_decolagem, t_remover_bagagens,
				t_inserir_bagagens, t_bagagens_esteira};
	meu_aeroporto = iniciar_aeroporto(args, n_args);

	// Descreve aqui sua simulação usando as funções definidas no arquivo "aeroporto.h"
	// Lembre-se de implementá-las num novo arquivo "aeroporto.c"
	fila_avioes = criar_fila(p_combustivel_max);
	t_novo_aviao = rand() % (t_novo_aviao_max + 1 - t_novo_aviao_min) + t_novo_aviao_min;
	pthread_t tempo, fabrica;
	pthread_create(&tempo, NULL, cronometro, (void *)&t_simulacao);
	pthread_create(&fabrica, NULL, fabrica_aviao, NULL);
	//aviao_t * aviao = aloca_aviao(10, 2); Remover tese rotina aviao
	//pthread_create(&aviao->thread, NULL, rotina_aviao, (void *)&aviao->id);Remover tese rotina aviao
	while(rodar_programa == 1);
	desaloca_fila(fila_avioes);
	finalizar_aeroporto(meu_aeroporto);
	return 1;
}
