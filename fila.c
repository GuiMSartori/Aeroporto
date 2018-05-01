#include "fila.h"
#include "aviao.h"

/**
 * fila.c
 * Implementação das funções da fila.h
 * Descrições em fila.h
 **/

 elemento_t * aloca_elemento (aviao_t * dado) {
   printf("terminou parte 5.2.1.1\n");
   fflush(stdout);
    elemento_t * elemento = (elemento_t *) malloc(sizeof(elemento_t));
    printf("terminou parte 5.2.1.2\n");
    fflush(stdout);
    elemento->proximo = NULL;
    elemento->anterior = NULL;
    printf("terminou parte 5.2.1.3\n");
    fflush(stdout);
    elemento->dado = dado;
    printf("terminou parte 5.2.1.4\n");
    fflush(stdout);
    return elemento;
 }

void desaloca_elemento (elemento_t * elemento) {
    desaloca_aviao(elemento->dado);
    free(elemento);
}

fila_ordenada_t * criar_fila (size_t combustivel_max) {
    fila_ordenada_t * fila_ordenada = (fila_ordenada_t*) malloc(sizeof(fila_ordenada_t));
    fila_ordenada->primeiro = NULL;
    fila_ordenada->ultimo = NULL;
    fila_ordenada->n_elementos = 0;
    fila_ordenada->combustivel_max = combustivel_max;
    return fila_ordenada;
}

void desaloca_fila (fila_ordenada_t * fila) {
  elemento_t *ref, *next;
  ref = fila->primeiro;
  for(int i = 0; i < fila->n_elementos; i++) {
    next = ref->proximo;
    desaloca_elemento(ref);
    ref = next;
  }
  free (fila);
}

void inserir (fila_ordenada_t * fila, aviao_t * dado) {
  printf("terminou parte 5.2.1\n");
  fflush(stdout);
  elemento_t * novo = aloca_elemento(dado);
  printf("terminou parte 5.2.1 combustivel aviao:%zu\n", novo->dado->combustivel);
  fflush(stdout);
  if (((float) (dado->combustivel/fila->combustivel_max)) > 0.1) {
    printf("terminou parte 5.2.2\n");
    fflush(stdout);
    if (fila->n_elementos > 0) {
      printf("terminou parte 5.2.3\n");
      fflush(stdout);
      fila->ultimo->anterior = novo;
      novo->proximo = fila->ultimo;
    } else {
      printf("terminou parte 5.2.4\n");
      fflush(stdout);
      fila->primeiro = novo;
    }
    printf("terminou parte 5.2.5\n");
    fflush(stdout);
    fila->ultimo = novo;
  } else {
    printf("terminou parte 5.2.6\n");
    fflush(stdout);
    fila->primeiro->proximo = novo;
    novo->anterior = fila->primeiro;
    fila->primeiro = novo;
    printf("terminou parte 5.2.7\n");
    fflush(stdout);
  }

}

aviao_t * remover (fila_ordenada_t * fila) {
  aviao_t * retorno = fila->primeiro->dado;
  fila->primeiro = fila->primeiro->anterior;
  desaloca_elemento(fila->primeiro->proximo);
  return retorno;
}