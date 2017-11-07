/**
 * TetrubS
 * Descricao: Arquivo de constantes configuraveis
 * Autor: Rubens Takiguti Ribeiro
 * E-mail: rubs33@gmail.com
 * Versao: 1.0.0.0
 * Data Criacao: 17/01/2008
 * Modificacao: 20/01/2008
 * Licenca: GNU/GPL 3 (LICENSE.TXT)
 * Copyright (C) 2008  Rubens Takiguti Ribeiro
 */
#ifndef CONFIG_H
#define CONFIG_H


/* Constantes Configuraveis  */
#define W            1024     /* Largura da tela         */
#define H             768     /* Altura da tela          */
#define LW             20     /* Largura do texto        */
#define LH             20     /* Altura do texto         */
#define MENU_TEMPO    350     /* Tempo de espera do Menu */

#define NUM_COLUNAS    13     /* Numero de colunas       */
#define NUM_LINHAS     30     /* Numero de linhas        */

#define PONTUACAO_1    40     /* Pontuacao para 1 linha  */
#define PONTUACAO_2   100     /* Pontuacao para 2 linha  */
#define PONTUACAO_3   300     /* Pontuacao para 3 linha  */
#define PONTUACAO_4  1200     /* Pontuacao para 4 linha  */


#define MEIO_AREA (NUM_COLUNAS / 2 - 2)

#define POS_AREA_X1 140
#define POS_AREA_Y1 52
#define POS_AREA_X4 482
#define POS_AREA_Y4 715
#define POS_AREA_X2 POS_AREA_X4
#define POS_AREA_Y2 POS_AREA_Y1
#define POS_AREA_X3 POS_AREA_X1
#define POS_AREA_Y3 POS_AREA_Y4

#define LARGURA_AREA (POS_AREA_X2 - POS_AREA_X1)
#define ALTURA_AREA  (POS_AREA_Y3 - POS_AREA_Y1)

#define LARGURA_UNIDADE (LARGURA_AREA / NUM_COLUNAS)
#define ALTURA_UNIDADE  (ALTURA_AREA / NUM_LINHAS)

#define POS_SEGUINTE_X 728
#define POS_SEGUINTE_Y 186

#define POS_NIVEL_X 685
#define POS_NIVEL_Y 360

#define POS_PONTOS_X 755
#define POS_PONTOS_Y 435

#define POS_JOGAR_X 620
#define POS_JOGAR_Y 600

#define POS_SAIR_X 800
#define POS_SAIR_Y 600

/* Teclas de Atalho */
#define TECLA_SAIR        KEY_ESC   /* Tecla para sair do jogo         */
#define TECLA_MUDAR_TELA  KEY_F2    /* Tecla para mudar o modo de tela */
#define TECLA_PAUSAR      KEY_P     /* Tecla para pausar o jogo        */


#endif
