#include "fila.h"

/**
 * fila.c
 * Implementação das funções da fila.h
 * Descrições em fila.h
 **/

 elemento_t * aloca_elemento (aviao_t * dado) {
    elemento_t * elemento = (elemento_t *) malloc(sizeof(elemento_t));
    elemento->proximo = nullptr;
    elemento->anterior = nullptr;
    elemento->dado = dado;
    return elemento;
 }

void desaloca_elemento (elemento_t * elemento) {
    desaloca_aviao(&elemento->dado);
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
  for(int i = 0; i < n_elementos; i++) {
    next = ref->proximo;
    desaloca_elemento(ref);
    ref = next;
  }
  free (fila);
}

void inserir (fila_ordenada_t * fila, aviao_t * dado) {
  elemento_t * novo = aloca_elemento(dado);
  if (((float) (dado->combustivel/fila->combustivel_max)) > 0.1) {
    if (fila->n_elementos > 0) {
      fila->ultimo->anterior = novo;
      novo->proximo = fila->ultimo;
    } else {
      fila->primeiro = novo;
    }
    fila->ultimo = novo;
  } else {
    fila->primeiro->proximo = novo;
    novo->anterior = fila->primeiro;
    fila->primeiro = novo;
  }

}

aviao_t * remover (fila_ordenada_t * fila) {
  aviao_t * retorno = fila->primeiro->dado;
  fila->primeiro = fila->primeiro->anterior;
  desaloca_elemento(fila->primeiro->proximo);
  return retorno;
}
