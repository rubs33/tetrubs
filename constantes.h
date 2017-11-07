/**
 * TetrubS
 * Descricao: Arquivo de constantes
 * Autor: Rubens Takiguti Ribeiro
 * E-mail: rubs33@gmail.com
 * Versao: 1.0.0.0
 * Data Criacao: 16/01/2008
 * Modificacao: 15/02/2008
 * Licenca: GNU/GPL 3 (LICENSE.TXT)
 * Copyright (C) 2008  Rubens Takiguti Ribeiro
 */
#ifndef CONSTANTES_H
#define CONSTANTES_H


/* Se nao especificou compilacao com debug */
#ifndef DEBUG
#define DEBUG 0
#endif


/* Outras constantes*/
#include "imagens.h"
#include "sons.h"
#include "textos.h"
#include "config.h"


/* Tipos de Retorno do Programa */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS       0
#endif
#define EXIT_ERRO_MEMORIA  1
#define EXIT_ERRO_VIDEO    2
#define EXIT_ERRO_IMAGENS  3
#define EXIT_ERRO_SONS     4
#define EXIT_ERRO_PECAS    5
#define EXIT_ERRO_PECA     6


/* Arquivos e Diretorios de recursos */
#define ARQ_IMAGENS  "./dados/imagens.dat"
#define ARQ_SONS     "./dados/sons.dat"
#define ARQ_PECAS    "./dados/pecas.txt"
#define ARQ_RECORDES "./dados/recordes.dat" 
#define DIR_MUSICAS  "./musicas/"


/* Constantes do Jogo */
#define OP_SAIR    0
#define OP_MENU    1
#define OP_JOGAR   2
#define OP_RECORDE 3


/* Constantes e Macros gerais */
#define AREA_RETANGULAR   0
#define AREA_CIRCULAR     1

#define ESQUERDA 0
#define DIREITA  1

#define loop_infinito for (;;)
#define sair_loop return
#define quadrado(x) (x) * (x)
#define inicio_game_speed while (game_speed > 0) {
#define fim_game_speed game_speed--; }

#endif
