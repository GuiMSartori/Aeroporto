#include "fila.h"
#include "aviao.h"
#include <pthread.h>

/**
 * fila.c
 * Implementação das funções da fila.h
 * Descrições em fila.h
 **/

 elemento_t * aloca_elemento (aviao_t * dado) {
    elemento_t * elemento = (elemento_t *) malloc(sizeof(elemento_t));
    elemento->proximo = NULL;
    elemento->anterior = NULL;
    elemento->dado = dado;
    return elemento;
 }

void desaloca_elemento (elemento_t * elemento) {
  free(elemento);
}

fila_ordenada_t * criar_fila (size_t combustivel_max) {
    fila_ordenada_t * fila_ordenada = (fila_ordenada_t*) malloc(sizeof(fila_ordenada_t));
    fila_ordenada->primeiro = NULL;
    fila_ordenada->ultimo = NULL;
    fila_ordenada->n_elementos = 0;
    fila_ordenada->combustivel_max = combustivel_max;
    pthread_mutex_init(&fila_ordenada->mutex);
    return fila_ordenada;
}

void desaloca_fila (fila_ordenada_t * fila) {
  for(int i = 0; i < fila->n_elementos; i++) {
    desaloca_aviao(remover(fila));
  }
  free(fila);
}

void inserir (fila_ordenada_t * fila, aviao_t * dado) {
  elemento_t * novo = aloca_elemento(dado);
  pthread_mutex_lock(&fila->mutex);
  if (fila->n_elementos == 0) {
    fila->primeiro = novo;
    fila->ultimo = novo;
    fila->n_elementos++;
    return;
  }
  if (((float) (dado->combustivel/fila->combustivel_max)) > 0.1) {
    fila->ultimo->anterior = novo;
    novo->proximo = fila->ultimo;
    fila->ultimo = novo;
  } else {
    fila->primeiro->proximo = novo;
    novo->anterior = fila->primeiro;
    fila->primeiro = novo;
  }
  fila->n_elementos++;
  pthread_mutex_unlock(&fila->mutex);
}

aviao_t * remover (fila_ordenada_t * fila) {
  pthread_mutex_lock(&fila->mutex);
  if (fila->n_elementos == 0){
    return NULL;
  }
  aviao_t * retorno = fila->primeiro->dado;
  if (fila->n_elementos == 1) {
    desaloca_elemento(fila->primeiro);
    fila->n_elementos--;
    return retorno;
  }
  fila->primeiro = fila->primeiro->anterior;
  desaloca_elemento(fila->primeiro->proximo);
  fila->n_elementos--;
  pthread_mutex_unlock(&fila->mutex);
  return retorno;
}
