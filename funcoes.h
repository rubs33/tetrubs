/**
 * TetrubS
 * Descricao: Cabecalho das funcoes
 * Autor: Rubens Takiguti Ribeiro
 * E-mail: rubs33@gmail.com
 * Versao: 1.0.0.0
 * Data Criacao: 16/01/2008
 * Modificacao: 01/03/2008
 * Licenca: GNU/GPL 3 (LICENSE.TXT)
 * Copyright (C) 2008  Rubens Takiguti Ribeiro
 */
#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdio.h>
#include "constantes.h"
#include "tipos.h"


/* Funcoes gerais do programa */
void inc_game_speed(void);
void abortar(const char *msg, const unsigned int cod_erro);
void iniciar_allegro(void);
void definir_modo_tela(int modo);
void criar_botao(botao *bt, BITMAP *img, const int x, const int y,
                 const char formato);
void preparar_buffer(void);
void exibir_buffer(void);
char get_digito(const int n, const unsigned int pos);
unsigned int wlclock(void);
unsigned int wlrand(void);
char passou(const unsigned int t, const unsigned int n);
char mouse_sobre(const botao *b);
char mouse_sobre_area(const int x, const int y,
                      const int w, const int h,
                      const unsigned char tipo);
void desenhar_botao(const botao *b);
void desenhar_numero(const int n, const unsigned int x, const unsigned int y);

/* Funcoes do jogo */
void ler_disposicao(tipo_peca *p, FILE *f);
void ler_peca(FILE *f);
void ler_pecas(void);
void destruir_pecas(void);
void iniciar_jogo(void);
void encerrar_jogo(void);
void jogar(void);
void mudar_pause(void);
void alterar_modo_tela(void);
void set_nivel(const char nivel);

/* Funcoes do menu */
void desenhar_nivel(void);
void desenhar_pontos(void);
void desenhar_menu(void);
void loop_menu(void);

/* Funcoes do jogo */
tipo_peca get_peca_aleatoria(void);
char tabuleiro_ocupado(const char linha, const char coluna);
char pode_descer(void);
char checar_linha(const char lado, const char linha);
char pode_mover(const char lado);
char pode_girar(void);
void girar_peca(void);
char descer_peca(void);
void fixar_peca(void);
void trocar_peca(void);
void eliminar_linhas(void);
void eliminar_linha(const char linha);
void somar_pontuacao(const unsigned int pontuacao);
void zerar_pontuacao();
void somar_linhas(const int linhas);
void desenhar_unidade(const char linha, const char coluna, const char cor);
void desenhar_unidade_xy(const int x, const int y, const char cor);
void desenhar_peca_caindo(void);
void desenhar_tabuleiro(void);
void desenhar_peca_seguinte(void);
void desenhar_jogo(void);
void loop_jogo(void);

/* Funcoes dos recordes */
recorde *atualizar_recordes(recorde **recordes, const unsigned int pontos, const char nivel);
void get_recordes(recorde **recordes);
void salvar_recorde(recorde **recordes);
void desenhar_recorde(recorde **recordes);
void loop_recorde(void);

#endif
