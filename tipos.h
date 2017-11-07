/**
 * TetrubS
 * Descricao: Definicao dos tipos utilizados
 * Autor: Rubens Takiguti Ribeiro
 * E-mail: rubs33@gmail.com
 * Versao: 1.0.0.0
 * Data Criacao: 20/01/2008
 * Modificacao: 15/02/2008
 * Licenca: GNU/GPL 3 (LICENSE.TXT)
 * Copyright (C) 2008  Rubens Takiguti Ribeiro
 */
#ifndef TIPOS_H
#define TIPOS_H

#include <allegro.h>
#include "constantes.h"


/**
 * config_jogo
 * Estrutura que guarda configuracoes e recursos
 */
typedef struct {
    MIDI        *musica;        /* Musica atual         */
    char     arq_musica;        /* Numero da musica     */
    long     tam_musica;        /* Tamanho da musica    */
    DATAFILE      *sons;        /* Vetor de sons        */
    DATAFILE      *imgs;        /* Vetor de imagens     */
    int      modo_video;        /* Modo de video atual  */
} config_jogo;


/**
 * disposicao_peca
 * Estrutura de uma disposicao de peca
 */
typedef struct {
    char matriz[4][4];   /* (linhas x colunas) */
} disposicao_peca;


/**
 * tipo_peca
 * Estrutura de uma peca
 */
typedef struct {
    disposicao_peca *disposicoes;   /* Disposicoes da peca girando */
    unsigned char num_disposicoes;  /* Numero de disposicoes       */
    char disposicao;                /* Disposicao corrente da peca */
    char coluna;                    /* Coluna no tabuleiro         */
    char linha;                     /* Linha no tabuleiro          */
    char cor;                       /* Indice da cor da peca       */
} tipo_peca;


/**
 * dados_jogo
 * Estrutura que guarda elementos do jogo
 */
typedef struct {
    BITMAP *buffer;           /* Buffer da tela         */
    unsigned char opcao;      /* Opcao do menu          */
    char nivel;               /* Nivel de dificuldade   */
    char nivel_inicio;        /* Nivel em que iniciou   */
    unsigned int velocidade;  /* Velocidade do jogo     */
    unsigned int pontos;      /* Pontuacao obtida       */
    unsigned int linhas;      /* Linhas obtidas         */
    char pausado;             /* Flag indicando pause   */
    char encerrado;           /* Flag indicando termino */
    tipo_peca peca_caindo;    /* Peca que esta caindo   */
    tipo_peca peca_seguinte;  /* Proxima peca           */
    tipo_peca *pecas;         /* Lista de pecas         */
    char num_pecas;           /* Numero de pecas        */
    
    /* Tabuleiro (linhas x colunas) de 0 a (N-1) */
    char tabuleiro[NUM_LINHAS][NUM_COLUNAS];
} dados_jogo;


/**
 * dados_aux
 * Estrutura que guarda dados auxiliares para otimizar o codigo
 */
typedef struct {
    char nivel[3];          /* Os 3 digitos do nivel     */
    char pontos[7];         /* Os 7 digitos dos pontos   */
    int cores[7];           /* Cores das pecas           */
    int cores_e[7];         /* Cores escuras             */
    char flag_peca;         /* Desenhando peca descendo? */
} dados_aux;


/**
 * botao
 * Estrutura que define um botao
 */
typedef struct {
    BITMAP *img;            /* Imagem do botao   */
    char formato;           /* Codigo do formato */
    int x, y;               /* Posicao da imagem */
} botao;


/**
 * lista_botoes
 * Estrutura que guarda os botoes
 */
typedef struct {
    botao bt_menos;     /* Botao para decrementar o nivel */
    botao bt_mais;      /* Botao para incrementar o nivel */
    botao bt_jogar;     /* Botao para iniciar a partida   */
    botao bt_pausar;    /* Botao para pausar o jogo       */
    botao bt_sair;      /* Botao para encerrar o jogo     */
} lista_botoes;


/**
 * recorde
 * Estrutura que guarda um recorde
 */
typedef struct {
    char nome[50];        /* Nome do pontuador */
    unsigned int pontos;  /* Pontos obtidos    */
    unsigned int linhas;  /* Linhas obtidas    */
    char nivel;           /* Nivel alcancado   */
} recorde;

#endif
