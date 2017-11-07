/**
 * TetrubS
 * Descricao: Implementacao das funcoes globais
 * Autor: Rubens Takiguti Ribeiro
 * E-mail: rubs33@gmail.com
 * Versao: 1.0.0.0
 * Data Criacao: 16/01/2008
 * Modificacao: 01/03/2008
 * Licenca: GNU/GPL 3 (LICENSE.TXT)
 * Copyright (C) 2008  Rubens Takiguti Ribeiro
 */
#include <allegro.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constantes.h"
#include "funcoes.h"


/** Variaveis Globais */
volatile int game_speed = 0;
config_jogo  *config    = NULL;
dados_jogo   *jogo      = NULL;
lista_botoes *botoes    = NULL;
dados_aux    *aux       = NULL;


/**
 * Incrementa o game_speed
 */
void inc_game_speed(void) {
    game_speed++;
}
END_OF_FUNCTION(inc_game_speed);


/**
 * Aborta o jogo com um codigo de erro
 * @param msg mensagem de erro a ser exibida
 * @param cod_erro codigo de retorno por exit
 */
void abortar(const char *msg, const unsigned int cod_erro) {
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    allegro_message(msg);
    encerrar_jogo();
    exit(cod_erro);
}


/**
 * Inicializa a biblioteca allegro
 */
void iniciar_allegro(void) {

    /* Instalar recursos necessarios */
    set_uformat(U_ASCII);
    allegro_init();
    install_timer();
    install_mouse();
    install_keyboard();

    /* Definir Titulo da janela */
    set_window_title("TetrubS");

    /* Travando as variaveis e funcoes dos temporizadores */
    LOCK_VARIABLE(game_speed);
    LOCK_FUNCTION(inc_game_speed);
    install_int_ex(inc_game_speed, BPS_TO_TIMER(100));
}


/**
 * Define um modo de tela
 * @param modo codigo do modo
 */
void definir_modo_tela(int modo) {
    if (set_gfx_mode(modo, W, H, 0, 0) == 0) {
        config->modo_video = modo;
    } else if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, W, H, 0, 0) == 0) {
        config->modo_video = GFX_AUTODETECT_FULLSCREEN;
    } else if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, W, H, 0, 0) == 0) {
        config->modo_video = GFX_AUTODETECT_WINDOWED;
    } else {
        abortar("[definir_modo_tela] " ST_ERRO_MODO_GRAFICO, EXIT_ERRO_VIDEO);
    }
}


/*
 * Toca um som Wave
 * @param cod_som codigo do som a ser tocado
 */
void tocar(const unsigned int cod_som) {
    if (cod_som < SOM_COUNT) {
        play_sample((SAMPLE *)config->sons[cod_som].dat, 255, 127, 1000, 0);
    }
}


/**
 * Toca uma musica Midi
 */
void tocar_musica() {
    char arq[128];
    sprintf(arq, DIR_MUSICAS "%d.mid", config->arq_musica);
    config->musica = load_midi(arq);
    if (config->musica == NULL) {
        config->arq_musica = 1;
        sprintf(arq, DIR_MUSICAS "%d.mid", config->arq_musica);
        config->musica = load_midi(arq);
    }
    if (config->musica != NULL) {
        config->tam_musica = get_midi_length(config->musica);
        play_midi(config->musica, 0);
    }
}


/**
 * Troca a musica atual caso tenha acabado
 */
void trocar_musica() {
    if (midi_time >= config->tam_musica) {
        config->arq_musica += 1;
        tocar_musica();
    }
}


/**
 * Define os atributos de um botao
 * @param img imagem do botao
 * @param x coordenada X do botao na tela
 * @param y coordenada Y do botao na tela
 * @param formato formato do botao
 */
void criar_botao(botao *bt, BITMAP *img, const int x, const int y,
                 const char formato) {
    bt->img     = img;
    bt->formato = formato;
    bt->x       = x;
    bt->y       = y;
}


/**
 * Prepara o buffer para receber uma imagem
 */
void preparar_buffer(void) {
    show_mouse(NULL);
    clear_bitmap(jogo->buffer);
    acquire_screen();
}


/**
 * Transfere o buffer para a tela
 */
void exibir_buffer(void) {
    show_mouse(jogo->buffer);
    blit(jogo->buffer, screen, 0, 0, 0, 0, W, H);
    release_screen();
}


/**
 * Obtem o valor de um digito do numero
 * @param n valor do numero
 * @param pos posicao do digito a ser obtido
 * @return o digito de uma posicao do numero
 */
char get_digito(const int n, const unsigned int pos) {
    int p = 1;
    int i = pos - 1;
    while (i-- > 0 && (p *= 10));
    return ((n - (n % p)) / p) % 10;
}


/**
 * Retorna o clock de acordo com o SO (Windows ou Linux)
 * @return o clock de acordo com o SO
 */
unsigned int wlclock(void) {
    #ifndef WINDOWS
    return clock() / 400;
    #else
    return clock();
    #endif
}


/**
 * Retorna um valor entre 0 e 1000
 * @return um valor entre 0 e 1000
 */
unsigned int wlrand(void) {
    return rand() % 1000;
}


/**
 * Retorna se ja se passaram N milisegundos desde T
 * @param t tempo a ser comparado
 * @param n tempo passado desde t
 * @return se ja passaram N milisegundos desde T
 */
char passou(const unsigned int t, const unsigned int n) {
    return wlclock() > t + n;
}


/**
 * Checa se o mouse esta sobre um botao
 * @param b botao a ser testado
 * @return se o mouse esta sobre o botao ou nao
 */
char mouse_sobre(const botao *b) {
    switch (b->formato) {
    case AREA_RETANGULAR:
        return mouse_sobre_area(b->x, b->y, b->img->w, b->img->h, b->formato);
    case AREA_CIRCULAR:
        return mouse_sobre_area(b->x, b->y, b->img->w, b->img->h, AREA_RETANGULAR);
//TODO
        return mouse_sobre_area(b->x + b->img->w / 2, /* Centro X da elipse */
                                b->y + b->img->h / 2, /* Centro Y da elipse */
                                b->img->w, b->img->h, b->formato);
    }
    return 0;
}


/**
 * Checa se o mouse esta sobre uma area
 * @param x coordenada X da area (superior esquerdo para retangulo e centro para elipse)
 * @param y coordenada Y da area (superior esquerdo para retangulo e centro para elipse)
 * @param w largura da area retangular ou raio horizontal da elipse
 * @param h altura da area retangular ou raio vertical da elipse
 * @param tipo codigo do tipo de area a ser avaliada
 * @return se o mouse esta sobre a regiao definida ou nao
 */
char mouse_sobre_area(const int x, const int y,
                      const int w, const int h,
                      const unsigned char tipo) {
    int d = 0;
    int a = 0;
    
    switch (tipo) {
    case AREA_RETANGULAR:
        return (mouse_x > x) && (mouse_x < x + w) &&
               (mouse_y > y) && (mouse_y < y + h);
    case AREA_CIRCULAR:
        /* TODO: obter o angulo formado entre os pontos:
         * [mouse_x, mouse_y] e [x, y] sobre as coordenadas e absissas
         */
        d = (int)(sqrt(quadrado(mouse_x - x) + quadrado(mouse_y - y)));
        a = 1;
        return (d * cos(a) < w) && (d * sin(a) < h);
    }
    return 0;
}


/**
 * Desenha um botao
 * @param bt botao a ser desenhado
 */
void desenhar_botao(const botao *b) {
    masked_blit(b->img, jogo->buffer, 0, 0,
        b->x, b->y, b->img->w, b->img->h);
}


/**
 * Desenha um numero em uma posicao do buffer
 * @param n numero a ser desenhado
 * @param x coordenada X do numero na tela
 * @param y coordenada Y do numero na tela
 */
void desenhar_numero(const int n, const unsigned int x, const unsigned int y) {
    BITMAP *img = NULL;
    int cod_img = 0;
    
    /* Obter codigo da imagem do numero */
    switch (n) {
    case 0: cod_img = IMG_0; break;
    case 1: cod_img = IMG_1; break;
    case 2: cod_img = IMG_2; break;
    case 3: cod_img = IMG_3; break;
    case 4: cod_img = IMG_4; break;
    case 5: cod_img = IMG_5; break;
    case 6: cod_img = IMG_6; break;
    case 7: cod_img = IMG_7; break;
    case 8: cod_img = IMG_8; break;
    case 9: cod_img = IMG_9; break;
    }
    
    /* Desenhar numero */
    img = (BITMAP *)config->imgs[cod_img].dat;
    masked_blit(img, jogo->buffer, 0, 0, x, y, img->w, img->h);
}


/**
 * Le uma disposicao de peca e adiciona na peca
 * @param p peca analisada
 * @param f arquivo de pecas
 */
void ler_disposicao(tipo_peca *p, FILE *f) {
    int c = 0;
    int achou = 0;
    int linha, coluna;
    disposicao_peca *d = NULL;
    
    /* Alocar uma nova disposicao */
    p->num_disposicoes += 1;
    p->disposicoes = (disposicao_peca *)realloc(p->disposicoes, (int)(p->num_disposicoes * sizeof(disposicao_peca)));
    d = &(p->disposicoes[p->num_disposicoes - 1]);

    /* Percorrer unidades da peca */
    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < 4; coluna++) {
            achou = 0;
            do {
                c = fgetc(f);
                
                if (c == EOF) {
                    fclose(f);
                    abortar("[ler_disposicao] " ST_ERRO_PECA, EXIT_ERRO_PECA);
                }
                
                switch ((char)c) {
                case '@':
                case '1':
                    d->matriz[linha][coluna] = 1;
                    achou = 1;
                    break;

                case '.':
                case '0':
                    d->matriz[linha][coluna] = 0;
                    achou = 1;
                    break;

                /* Ignorar quebras de linha e espacos */
                case '\n':
                case '\r':
                case '\t':
                case ' ':
                    break;

                /* Caracter invalido */
                default:
                    fclose(f);
                    abortar("[ler_disposicao] " ST_ERRO_PECA, EXIT_ERRO_PECA);
                }
            } while (!achou);
        }
    }
}


/**
 * Le uma peca do arquivo e armazena na memoria
 * @param f ponteiro para posicao do arquivo com uma peca
 */
void ler_peca(FILE *f) {
    tipo_peca *p = NULL;
    int c = 0;
    static unsigned char cor = 0;

    /* Alocar espaco para uma peca */
    jogo->num_pecas += 1;
    jogo->pecas = (tipo_peca *)realloc(jogo->pecas, jogo->num_pecas * sizeof(tipo_peca));
    p = &(jogo->pecas[jogo->num_pecas - 1]);
    
    p->disposicoes = NULL;
    p->num_disposicoes = 0;
    p->disposicao = 0;
    p->coluna = 0;
    p->linha = 0;
    p->cor = cor++ % 7 + 1;
    
    do {
        c = fgetc(f);
        
        if (c == EOF) {
            break;
        }
        
        switch ((char)c) {
            
        /* Inicio de uma nova peca */
        case '#':
            fseek(f, -1, SEEK_CUR); /* Voltar uma posicao no arquivo */
            return;
        
        /* Ignorar quebras de linha e espacos */
        case '\n':
        case '\r':
        case '\t':
        case ' ':
            break;
            
        /* Inicio de uma disposicao de peca */
        case '@':
        case '1':
        case '.':
        case '0':
            fseek(f, -1, SEEK_CUR);   /* Voltar uma posicao no arquivo */
            ler_disposicao(p, f);     /* Ler a disposicao de peca      */
            break;
            
        /* Caractere invalido */
        default:
            fclose(f);
            abortar("[ler_peca]2 " ST_ERRO_PECA, EXIT_ERRO_PECA);
        }
    } while (c != EOF);
}


/**
 * Le as pecas do arquivo texto
 */
void ler_pecas(void) {
    FILE *f;
    char linha[128];
    int c = 0;
    
    /* Abrir arquivo de pecas para carregar as pecas */
    f = fopen(ARQ_PECAS, "r");
    if (ferror(f) != 0) {
        abortar("[ler_pecas] " ST_ERRO_PECAS, EXIT_ERRO_PECAS);
    }
    
    do {
        c = fgetc(f);
        
        /* Chegou no final do arquivo */
        if (c == EOF) {
            break;
        }
        
        switch ((char)c) {
            
        /* Inicio de uma nova peca */
        case '#':
            fgets(linha, 128, f);   /* Ignorar a linha inteira */
            ler_peca(f);            /* Comecar a ler uma peca  */
            break;
        
        /* Ignorar quebras de linha e espacos */
        case '\n':
        case '\r':
        case '\t':
        case ' ':
            break;

        /* Caractere invalido */
        default:
            fclose(f);
            abortar("[ler_pecas] " ST_ERRO_PECA, EXIT_ERRO_PECA);
        }
    } while (c != EOF);
    
    fclose(f);
}


/**
 * Desaloca as pecas carregadas na memoria
 */
void destruir_pecas(void) {
    char i;
    tipo_peca *p = NULL;
    
    /* Percorrer as pecas alocadas */
    for (i = 0; i < jogo->num_pecas; i++) {
        p = &(jogo->pecas[(int)i]);
        
        /* Desalocar disposicoes */
        free(p->disposicoes);
    }
    
    /* Desalocar pecas */
    free(jogo->pecas);
}


/**
 * Inicializa os dados do jogo
 */
void iniciar_jogo(void) {
    BITMAP *img = NULL;
    int depth = 0;
    
    /* Alocando memoria */
    config = (config_jogo *)malloc(sizeof(config_jogo));
    jogo   = (dados_jogo *)malloc(sizeof(dados_jogo));
    botoes = (lista_botoes *)malloc(sizeof(lista_botoes));
    aux    = (dados_aux *)malloc(sizeof(dados_aux));
    
    if (config == NULL || jogo == NULL || botoes == NULL) {
        abortar("[iniciar_jogo] " ST_ERRO_MEMORIA, EXIT_ERRO_MEMORIA);
    }

    /* Definindo modo grafico */
    set_color_depth(((depth = desktop_color_depth()) >= 16) ? depth : 16);
    definir_modo_tela(GFX_AUTODETECT_FULLSCREEN);
    
    /* Carregando o arquivo de Imagens */
    config->imgs = load_datafile(ARQ_IMAGENS);
    if (config->imgs == NULL) {
        abortar("[iniciar_jogo] " ST_ERRO_IMAGENS, EXIT_ERRO_IMAGENS);
    }
    
    /* Carregando o arquivo de Sons */
    config->sons = load_datafile(ARQ_SONS);
    if (config->sons == NULL) {
        abortar("[iniciar_jogo] " ST_ERRO_SONS, EXIT_ERRO_SONS);
    }
    
    /* Definindo a musica corrente */
    config->arq_musica = 0;
    config->tam_musica = 0;
    config->musica     = NULL;
    tocar_musica();
    
    /* Definindo cursor da Allegro */
    select_mouse_cursor(MOUSE_CURSOR_ALLEGRO);

    /* Definindo as configurações de som */
    if (!install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, 0)) {
        set_volume(255, 255);
    }

    /* Preenchendo os dados do jogo */
    jogo->buffer = create_bitmap(W, H);
    drawing_mode(DRAW_MODE_SOLID, jogo->buffer, 0, 0);
    set_nivel(0);                  /* Iniciar no nivel zero       */
    zerar_pontuacao();             /* Zerar a pontuacao           */
    jogo->opcao        = OP_MENU;  /* Iniciar o jogo no menu      */
    jogo->encerrado    = 0;        /* Iniciar sem estar encerrado */
    jogo->pausado      = 0;        /* Pause desativado            */
    jogo->pecas        = NULL;     /* Lista de pecas vazia        */
    jogo->num_pecas    = 0;        /* Numero de pecas             */
    
    /* Ler as pecas do arquivo */
    ler_pecas();

    /* Preenchendo os dados auxiliares */
    memset(&(aux->nivel), -1, 3);
    memset(&(aux->pontos), -1, 7);
    
    /* Cores claras */
    aux->cores[0] = makecol(255, 255,   0);
    aux->cores[1] = makecol( 65, 160, 240);
    aux->cores[2] = makecol(220,   0,   0);
    aux->cores[3] = makecol(255, 132,   0);
    aux->cores[4] = makecol(  0,   0, 200);
    aux->cores[5] = makecol(  0, 240,   0);
    aux->cores[6] = makecol(255,  20, 255);
    
    /* Cores escuras */
    aux->cores_e[0] = makecol(128, 128,   0);
    aux->cores_e[1] = makecol( 11,  77, 130);
    aux->cores_e[2] = makecol(100,   0,   0);
    aux->cores_e[3] = makecol(150,  70,   0);
    aux->cores_e[4] = makecol(  0,   0, 100);
    aux->cores_e[5] = makecol(  0, 110,   0);
    aux->cores_e[6] = makecol(150,   0, 150);
    
    /* Criar os botoes */
    
    /* Botao para decrementar nivel */
    img = (BITMAP *)config->imgs[IMG_BT_ESQUERDA].dat;
    criar_botao(&botoes->bt_menos, img,
                POS_NIVEL_X - img->w, POS_NIVEL_Y,
                AREA_RETANGULAR);
    
    /* Botao para incrementar nivel */
    img = (BITMAP *)config->imgs[IMG_BT_DIREITA].dat;
    criar_botao(&botoes->bt_mais, img,
                POS_NIVEL_X + (LW * 3), POS_NIVEL_Y,
                AREA_RETANGULAR);
    
    /* Botao para iniciar partida */
    img = (BITMAP *)config->imgs[IMG_BT_JOGAR].dat;
    criar_botao(&botoes->bt_jogar, img,
                POS_JOGAR_X, POS_JOGAR_Y, AREA_CIRCULAR);

    /* Botao para pausar a partida */
    img = (BITMAP *)config->imgs[IMG_BT_PAUSAR].dat;
    criar_botao(&botoes->bt_pausar, img,
                POS_JOGAR_X, POS_JOGAR_Y, AREA_CIRCULAR);

    /* Botao para sair da partida ou do jogo */
    img = (BITMAP *)config->imgs[IMG_BT_SAIR].dat;
    criar_botao(&botoes->bt_sair, img,
                POS_SAIR_X, POS_SAIR_Y, AREA_CIRCULAR);
}


/**
 * Encerra o jogo e a biblioteca allegro
 */
void encerrar_jogo(void) {
    
    /* Encerrando a Allegro */
    stop_midi();
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    unload_datafile(config->imgs);
    unload_datafile(config->sons);
    destroy_bitmap(jogo->buffer);
    allegro_exit();
    
    /* Desalocando memoria */
    destruir_pecas();
    free(config);
    free(jogo);
    free(botoes);
    free(aux);
}


/**
 * Funcao para iniciar o jogo
 */
void jogar(void) {

    /* Inicializar a Allegro */
    iniciar_allegro();

    /* Inicia os dados do jogo */
    iniciar_jogo();
    
    /* Escolher opcao do jogo */
    do {
        switch (jogo->opcao) {
        case OP_MENU:
            loop_menu();
            break;
        case OP_JOGAR:
            loop_jogo();
            break;
        case OP_RECORDE:
            loop_recorde();
            break;
        case OP_SAIR:
            break;
        }
    } while (jogo->opcao != OP_SAIR);

    /* Encerrar o jogo */
    encerrar_jogo();
}


/**
 * Alterna entre o modo pausado e modo ativo
 */
void mudar_pause(void) {
    jogo->pausado = 1 - jogo->pausado;
    tocar(SOM_BOTAO);
}


/**
 * Alterna entre tela cheia e janela
 */
void alterar_modo_tela(void) {
    
    /* Alterar o modo de video */
    if (config->modo_video == GFX_AUTODETECT_FULLSCREEN) {
        config->modo_video = GFX_AUTODETECT_WINDOWED;
    } else {
        config->modo_video = GFX_AUTODETECT_FULLSCREEN;
    }
    
    /* Utilizar o modo de video alterado */
    definir_modo_tela(config->modo_video);
}


/**
 * Define o nivel do jogo
 * @param nivel nivel a ser setado (entre 0 e 100)
 */
void set_nivel(const char nivel) {
    if (jogo->nivel == nivel) {
        return;
    }
    jogo->nivel = (nivel < 0) ? 0 : (nivel > 100) ? 100 : nivel;

    /* Atualizar os digitos auxiliares */    
    aux->nivel[0] = get_digito(jogo->nivel, 3);
    aux->nivel[1] = get_digito(jogo->nivel, 2);
    aux->nivel[2] = get_digito(jogo->nivel, 1);
    
    /* Atualizar velocidade das pecas */
    
    /* 1000 -> 88 {1000 - [(24 - 00) * 38]} */
    if (jogo->nivel < 25) {
        jogo->velocidade = 1000 - ((jogo->nivel - 0) * 38);

    /* 0950 -> 86 {0900 - [(49 - 25) * 36]} */
    } else if (jogo->nivel < 50) {
        jogo->velocidade = 950 - ((jogo->nivel - 25) * 36);
        
    /* 0900 -> 84 {0900 - [(74 - 50) * 34]} */
    } else if (jogo->nivel < 75) {
        jogo->velocidade = 900 - ((jogo->nivel - 50) * 34);
        
    /* 0882 -> 82 {0850 - [(100 - 75) * 32]} */
    } else {
        jogo->velocidade = 882 - ((jogo->nivel - 75) * 32);
    }
}

/**
 * Desenha o nivel em que o jogo esta
 */
void desenhar_nivel(void) {
    desenhar_numero((int)(aux->nivel[0]), POS_NIVEL_X + LW * 0, POS_NIVEL_Y);
    desenhar_numero((int)(aux->nivel[1]), POS_NIVEL_X + LW * 1, POS_NIVEL_Y);
    desenhar_numero((int)(aux->nivel[2]), POS_NIVEL_X + LW * 2, POS_NIVEL_Y);
}


/**
 * Desenha os pontos do usuario
 */
void desenhar_pontos(void) {
    desenhar_numero((int)(aux->pontos[0]), POS_PONTOS_X + LW * 0, POS_PONTOS_Y);
    desenhar_numero((int)(aux->pontos[1]), POS_PONTOS_X + LW * 1, POS_PONTOS_Y);
    desenhar_numero((int)(aux->pontos[2]), POS_PONTOS_X + LW * 2, POS_PONTOS_Y);
    desenhar_numero((int)(aux->pontos[3]), POS_PONTOS_X + LW * 3, POS_PONTOS_Y);
    desenhar_numero((int)(aux->pontos[4]), POS_PONTOS_X + LW * 4, POS_PONTOS_Y);
    desenhar_numero((int)(aux->pontos[5]), POS_PONTOS_X + LW * 5, POS_PONTOS_Y);
    desenhar_numero((int)(aux->pontos[6]), POS_PONTOS_X + LW * 6, POS_PONTOS_Y);
}


/**
 * Desenha o menu 
 */
void desenhar_menu(void) {
    BITMAP *img = NULL;

    /* Fundo */
    img = (BITMAP *)config->imgs[IMG_FUNDO].dat;
    blit(img, jogo->buffer, 0, 0, 0, 0, img->w, img->h);
    
    /* Botoes */
    desenhar_botao(&botoes->bt_menos);
    desenhar_botao(&botoes->bt_mais);
    desenhar_botao(&botoes->bt_jogar);
    desenhar_botao(&botoes->bt_sair);

    /* Nivel */
    desenhar_nivel();

    /* Pontuacao */
    desenhar_pontos();
    
    /* Textos para debug */
    #if DEBUG == 1
    textprintf_ex(jogo->buffer, font, 5, 5, makecol(255,255,255), 0,
        " Debug ");
    textprintf_ex(jogo->buffer, font, 5, 15, makecol(255,255,255), 0,
        " Mouse: (%d, %d) ", mouse_x, mouse_y);
    textprintf_ex(jogo->buffer, font, 5, 25, makecol(255,255,255), 0,
        " Area: %d x %d ", LARGURA_AREA, ALTURA_AREA);
    textprintf_ex(jogo->buffer, font, 5, 35, makecol(255,255,255), 0,
        " Tijolinho: %d x %d ", LARGURA_UNIDADE, ALTURA_UNIDADE);
    #endif
}


/**
 * Tela de Menu
 * @return opcao selecionada do menu
 */
void loop_menu(void) {
    unsigned int tempo = wlclock();

    loop_infinito {
        
        /* Desenhar na tela */
        preparar_buffer();
        desenhar_menu();
        exibir_buffer();
        
        /* Tratar eventos  */
        inicio_game_speed

        trocar_musica();

        /* Tratar mouse */
        if ((mouse_b & 1) && passou(tempo, MENU_TEMPO)) {
            tempo = wlclock();
            if (mouse_sobre(&(botoes->bt_mais))) {
                tocar(SOM_BOTAO);
                set_nivel(jogo->nivel + 1);
            } else if (mouse_sobre(&(botoes->bt_menos))) {
                tocar(SOM_BOTAO);
                set_nivel(jogo->nivel - 1);
            } else if (mouse_sobre(&(botoes->bt_jogar))) {
                tocar(SOM_BOTAO);
                jogo->opcao = OP_JOGAR;
                sair_loop;
            } else if (mouse_sobre(&(botoes->bt_sair))) {
                jogo->opcao = OP_SAIR;
                sair_loop;
            }
        }

        /* Tratar teclado */
        if (keypressed()) {
            switch (readkey() >> 8) {
                    
            /* Opcoes gerais */
            case KEY_UP:
            case KEY_RIGHT:
                tocar(SOM_BOTAO);
                set_nivel(jogo->nivel + 1);
                break;

            case KEY_DOWN:
            case KEY_LEFT:
                tocar(SOM_BOTAO);
                set_nivel(jogo->nivel - 1);
                break;

            case TECLA_MUDAR_TELA:
                alterar_modo_tela();
                break;

            /* Opcoes que saem do loop */
            case KEY_ENTER:
                tocar(SOM_BOTAO);
                jogo->opcao = OP_JOGAR;
                sair_loop;

            case TECLA_SAIR:
                jogo->opcao = OP_SAIR;
                sair_loop;
            }
        }
        fim_game_speed
    }
}


/**
 * Retorna um ponteiro para uma peca aleatoria
 * @return uma peca aleatoria
 */
tipo_peca get_peca_aleatoria(void) {
    tipo_peca p;
    unsigned int sorteio = 0;
    
    /* Sortear uma peca */
    sorteio = wlrand() % jogo->num_pecas;
    p = jogo->pecas[sorteio];
    
    /* Sortear uma disposicao */
    sorteio = wlrand() % p.num_disposicoes;
    p.disposicao = sorteio;
    
    return p;
}


/**
 * Checa se o tabuleiro esta ocupado em uma posicao
 */
char tabuleiro_ocupado(const char linha, const char coluna) {
    
    /* Posicao valida do Tabuleiro */
    if (linha >= 0 && linha < NUM_LINHAS &&
        coluna >= 0 && coluna < NUM_COLUNAS) {
        return jogo->tabuleiro[(int)linha][(int)coluna];
    }
    
    /* Posicao acima do tabuleiro */
    if (linha < 0) {
        return 0;
    }
    
    /* Posicao invalida */
    return 1;
}


/**
 * Indica se a peca que esta descendo pode descer ainda mais
 * @return flag indicando se a peca pode descer mais ou nao
 */
char pode_descer(void) {
    disposicao_peca *d;
    char linha, coluna, l_peca, c_peca;

    /* Obter posicao da peca */
    l_peca = jogo->peca_caindo.linha;
    c_peca = jogo->peca_caindo.coluna;

    d = &(jogo->peca_caindo.disposicoes[(int)jogo->peca_caindo.disposicao]);
    
    /* Obter unidade mais baixa da peca */
    /* TODO: otimizar este loop */
    for (linha = 3; linha >= 0; linha--) {
        for (coluna = 0; coluna < 4; coluna++) {
            
            /* Se chegou ao final do tabuleiro OU
             * Se existe unidade na posicao linha x coluna da peca
             * E a posicao abaixo desta no tabuleiro esta preenchida
             */
            if (d->matriz[(int)linha][(int)coluna] && 
                ((l_peca + linha >= NUM_LINHAS) ||
                 tabuleiro_ocupado(l_peca + linha + 1, c_peca + coluna))
                ) {
                return 0;
            }
        }
    }
    return 1;
}


/**
 * Checa se uma linha de uma disposicao pode ir para um lado
 * @param lado lado a ser checado
 * @param linha linha a ser checada
 * @return se a linha pode ir para um lado ou nao
 */
char checar_linha(const char lado, const char linha) {
    char coluna;
    char l_peca, c_peca;
    disposicao_peca *d;
    
    d = &(jogo->peca_caindo.disposicoes[(int)jogo->peca_caindo.disposicao]);

    switch (lado) {
    case ESQUERDA:
        
        /* TODO: otimizar este loop */
        for (coluna = 0; coluna < 4; coluna++) {
            if (d->matriz[(int)linha][(int)coluna]) {
                
                /* Obter posicao da peca */
                l_peca = jogo->peca_caindo.linha;
                c_peca = jogo->peca_caindo.coluna;
                
                if (c_peca + coluna - 1 < 0) {
                    return 0;
                }
                
                return !tabuleiro_ocupado(l_peca + linha, c_peca + coluna - 1);
            }
        }
        break;
        
    case DIREITA:
        
        /* TODO: otimizar este loop */
        for (coluna = 3; coluna >= 0; coluna--) {
            if (d->matriz[(int)linha][(int)coluna]) {
                
                /* Obter posicao da peca */
                l_peca = jogo->peca_caindo.linha;
                c_peca = jogo->peca_caindo.coluna;
                
                if (c_peca + coluna + 1 >= NUM_COLUNAS) {
                    return 0;
                }
                
                return !tabuleiro_ocupado(l_peca + linha, c_peca + coluna + 1);
            }
        }
        break;
    }
    return 1;
}


/**
 * Checa se a peca pode ir para direita ou esquerda
 * @param lado codigo do lado a se checado
 * @return se a peca pode ir para o lado desejado ou nao
 */
char pode_mover(const char lado) {

    /* Checar se cada linha pode ir para o lado desejado */
    return checar_linha(lado, 0) &&
           checar_linha(lado, 1) &&
           checar_linha(lado, 2) &&
           checar_linha(lado, 3);
}


/**
 * Checa se a peca pode girar ou nao
 * @return se a peca pode girar ou nao
 */
char pode_girar(void) {
    disposicao_peca *d;
    char linha, coluna, l_peca, c_peca;
    char proxima_disposicao = (jogo->peca_caindo.disposicao + 1) % 
                              jogo->peca_caindo.num_disposicoes;
    d = &(jogo->peca_caindo.disposicoes[(int)proxima_disposicao]);
    
    /* Obter posicao da peca */
    l_peca = jogo->peca_caindo.linha;
    c_peca = jogo->peca_caindo.coluna;
                    
    /* Checar se a proxima disposicao e' aceitavel */
    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < 4; coluna++) {
            
            /* Se existe unidade na posicao linha x coluna da peca */
            if (d->matriz[(int)linha][(int)coluna]) {
                
                /* Se a posicao esta' fora do tabuleiro */
                if (linha < 0 || linha >= NUM_LINHAS ||
                    coluna < 0 || coluna >= NUM_COLUNAS ||
                    tabuleiro_ocupado(l_peca + linha, c_peca + coluna)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}


/**
 * Gira a peca corrente para a proxima disposicao
 */
void girar_peca(void) {
    jogo->peca_caindo.disposicao = (jogo->peca_caindo.disposicao + 1) % 
                                   jogo->peca_caindo.num_disposicoes;
    tocar(SOM_GIRAR);
}


/**
 * Desce a peca que esta caindo
 * @return se a peca desceu ou nao
 */
char descer_peca(void) {
    static unsigned int t = 0;
    static unsigned char pode_tocar = 1;

    if (passou(t, jogo->velocidade)) {
        if (pode_descer()) {
            jogo->peca_caindo.linha += 1;
            t = wlclock();
            return 1;
        } else {
            pode_tocar = 1;
            return 0;
        }
    } else if (!pode_descer() && pode_tocar) {
        tocar(SOM_CAIR);
        pode_tocar = 0;
    }
    return 1;
}


/**
 * Fixa a peca que esta caindo no tabuleiro
 */
void fixar_peca(void) {
    int linha, coluna, l_peca, c_peca;
    disposicao_peca *d;

    d = &(jogo->peca_caindo.disposicoes[(int)jogo->peca_caindo.disposicao]);
    
    /* Obter posicao da peca */
    l_peca = jogo->peca_caindo.linha;
    c_peca = jogo->peca_caindo.coluna;
    
    /* Fixar peca atual no tabuleiro */
    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < 4; coluna++) {
            if (d->matriz[linha][coluna]) {
                if (l_peca + linha >= 0) {
                    jogo->tabuleiro[l_peca + linha][c_peca + coluna] = jogo->peca_caindo.cor;
                } else {
                    jogo->encerrado = 1;
                }
            }
        }
    }
}


/**
 * Troca a peca atual pela peca seguinte e sorteia uma nova peca seguinte
 */
void trocar_peca(void) {
    
    /* Trocar peca atual pela peca seguinte */
    jogo->peca_caindo = jogo->peca_seguinte;
    jogo->peca_caindo.linha = -3;
    jogo->peca_caindo.coluna = MEIO_AREA;
    
    /* Sortear nova peca seguinte */
    jogo->peca_seguinte = get_peca_aleatoria();
}


/**
 * Elimina as linhas completadas e atualiza a pontuacao e nivel
 */
void eliminar_linhas(void) {
    char linha, coluna;
    char completa;
    char eliminadas = 0;
    int pontuacao = 0;
    
    /* Checar apenas as linhas onde a peca caiu */
    for (linha = jogo->peca_caindo.linha; linha < jogo->peca_caindo.linha + 4; linha++) {
        completa = 1;
        for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
            if (!jogo->tabuleiro[(int)linha][(int)coluna]) {
                completa = 0;
                break;
            }
        }
        
        /* Eliminar a linha */
        if (completa) {
            eliminar_linha(linha);
            linha--;
            eliminadas++;
        }
    }
    
    /* Se achou linhas a serem eliminadas */
    if (eliminadas) {
        
        /* Calcular a pontuacao */
        switch (eliminadas) {
        case 1: pontuacao = PONTUACAO_1; break;
        case 2: pontuacao = PONTUACAO_2; break;
        case 3: pontuacao = PONTUACAO_3; break;
        case 4: pontuacao = PONTUACAO_4; break;
        }
        
        /* Incrementar pontuacao e numero de linhas */
        somar_pontuacao(pontuacao);
        somar_linhas(eliminadas);
        tocar(SOM_LINHA);
    }
    
}


/**
 * Elimina uma linha do tabuleiro
 * @param linha linha a ser eliminada
 */
void eliminar_linha(const char linha) {
    char linha_aux, coluna_aux;
    
    /* Para cada linha acima da linha eliminada */
    for (linha_aux = linha; linha_aux > 0; linha_aux--) {
        
        /* Copiar os valores para linha de baixo */
        for (coluna_aux = 0; coluna_aux < NUM_COLUNAS; coluna_aux++) {
            jogo->tabuleiro[(int)linha_aux][(int)coluna_aux] = 
                jogo->tabuleiro[(int)linha_aux - 1][(int)coluna_aux];
            jogo->tabuleiro[(int)linha_aux - 1][(int)coluna_aux] = 0;
        }
    }
}


/**
 * Soma um valor a pontuacao total
 * @param pontuacao valor a ser somado
 */
void somar_pontuacao(const unsigned int pontuacao) {
    jogo->pontos += pontuacao;
    
    /* Atualizar os digitos auxiliares */    
    aux->pontos[0] = get_digito(jogo->pontos, 7);
    aux->pontos[1] = get_digito(jogo->pontos, 6);
    aux->pontos[2] = get_digito(jogo->pontos, 5);
    aux->pontos[3] = get_digito(jogo->pontos, 4);
    aux->pontos[4] = get_digito(jogo->pontos, 3);
    aux->pontos[5] = get_digito(jogo->pontos, 2);
    aux->pontos[6] = get_digito(jogo->pontos, 1);
}


/**
 * Zera a pontuacao do jogo
 */
void zerar_pontuacao() {
    jogo->pontos = 0;
    jogo->linhas = 0;
    memset(&(aux->pontos), 0, 7);
}


/**
 * Soma um valor ao numero de linhas total
 * @param linhas numero de linhas eliminadas
 */
void somar_linhas(const int linhas) {
    char novo_nivel = 0;
    jogo->linhas += linhas;
    novo_nivel = ((jogo->linhas - (jogo->linhas % 10)) / 10) +
                  jogo->nivel_inicio;
    if (jogo->nivel != novo_nivel) {
        tocar(SOM_NIVEL);
    }
    set_nivel(novo_nivel);
}


/**
 * Desenha uma unidade de peca (tijolinho)
 * @param linha linha da unidade no tabuleiro
 * @param coluna coluna da unidade no tabuleiro
 * @param cor cor da unidade
 */
void desenhar_unidade(const char linha, const char coluna, const char cor) {
    const int x = POS_AREA_X1 + (coluna * LARGURA_UNIDADE);
    const int y = POS_AREA_Y1 + (linha * ALTURA_UNIDADE);
    desenhar_unidade_xy(x, y, cor);
}


/**
 * Desenha uma unidade de peca (tijolinho) em uma posicao da tela
 * @param x coordenada x na tela
 * @param y coordenada y na tela
 * @param cor cor da unidade
 */
void desenhar_unidade_xy(const int x, const int y, const char cor) {
    int c = aux->cores_e[cor - 1];
    
    /* Desenha o raio X do jogo */
    if (key[KEY_X] && aux->flag_peca) {
        vline(jogo->buffer, x, POS_AREA_Y1, POS_AREA_Y3, makecol(255, 0, 0));
        vline(jogo->buffer, x + LARGURA_UNIDADE, POS_AREA_Y1, POS_AREA_Y3, makecol(255, 0, 0));
        rectfill(jogo->buffer, x, y, x + LARGURA_UNIDADE, y + ALTURA_UNIDADE, 0);
        return;
    }
    
    /* Desenha a peca normalmente */
    rectfill(jogo->buffer, x, y, x + LARGURA_UNIDADE, y + ALTURA_UNIDADE, aux->cores[cor - 1]);
    rect(jogo->buffer, x, y, x + LARGURA_UNIDADE, y + ALTURA_UNIDADE, c);
    
    int linha, coluna;
    for (linha = ALTURA_UNIDADE - 1; linha >= 0; linha--) {
        for (coluna = LARGURA_UNIDADE - 1; coluna >= linha; coluna -= 2) {
            putpixel(jogo->buffer, x + coluna, y + linha, c);
        }
    }
}


/**
 * Desenha uma peca caindo
 */
void desenhar_peca_caindo(void) {
    int linha, coluna;
    int x, y;
    tipo_peca p = jogo->peca_caindo;

    x = POS_AREA_X1 + (p.coluna * LARGURA_UNIDADE);
    y = POS_AREA_Y1 + (p.linha * ALTURA_UNIDADE);

    aux->flag_peca = 1;
    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < 4; coluna++) {
            if (p.disposicoes[(int)p.disposicao].matriz[linha][coluna]) {
                desenhar_unidade(p.linha + linha,
                                 p.coluna + coluna,
                                 p.cor);
            }
        }
    }
    aux->flag_peca = 0;
}


/**
 * Desenha o tabuleiro
 */
void desenhar_tabuleiro(void) {
    int linha, coluna;
    char cor;
    for (linha = 0; linha < NUM_LINHAS; linha++) {
        for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
            if ((cor = jogo->tabuleiro[linha][coluna])) {
                desenhar_unidade(linha, coluna, cor);
            }
        }
    }
}


/**
 * Desenha a proxima peca
 */
void desenhar_peca_seguinte(void) {
    int linha, coluna;
    tipo_peca p = jogo->peca_seguinte;

    for (linha = 0; linha < 4; linha++) {
        for (coluna = 0; coluna < 4; coluna++) {
            if (p.disposicoes[(int)p.disposicao].matriz[linha][coluna]) {
                desenhar_unidade_xy(POS_SEGUINTE_X + (coluna * LARGURA_UNIDADE),
                                    POS_SEGUINTE_Y + (linha * ALTURA_UNIDADE),
                                    p.cor);
            }
        }
    }
}


/**
 * Desenha o jogo
 */
void desenhar_jogo(void) {
    BITMAP *img = NULL;

    /* Fundo */
    img = (BITMAP *)config->imgs[IMG_FUNDO].dat;
    blit(img, jogo->buffer, 0, 0, 0, 0, img->w, img->h);
    
    /* Botoes */
    desenhar_botao(&botoes->bt_pausar);
    desenhar_botao(&botoes->bt_sair);

    /* Nivel e Pontuacao */
    desenhar_nivel();
    desenhar_pontos();
    
    /* Peca caindo e Peca seguinte */
    if (!jogo->pausado) {
        desenhar_peca_caindo();
        desenhar_tabuleiro();
        desenhar_peca_seguinte();
    }
    
    /* Textos para debug */
    #if DEBUG == 1

    /* Area do jogo */
    fastline(jogo->buffer, POS_AREA_X1, POS_AREA_Y1, POS_AREA_X2, POS_AREA_Y2, makecol(255,0,0));
    fastline(jogo->buffer, POS_AREA_X1, POS_AREA_Y1, POS_AREA_X3, POS_AREA_Y3, makecol(255,0,0));
    fastline(jogo->buffer, POS_AREA_X4, POS_AREA_Y4, POS_AREA_X2, POS_AREA_Y2, makecol(255,0,0));
    fastline(jogo->buffer, POS_AREA_X4, POS_AREA_Y4, POS_AREA_X3, POS_AREA_Y3, makecol(255,0,0));

    /* Posicao da peca que esta caindo */
    textprintf_ex(jogo->buffer, font, 5, 5, makecol(255,255,255), 0,
        " Peca caindo: (c:%d, l:%d) ", jogo->peca_caindo.coluna, jogo->peca_caindo.linha);

    /* Pausado? */
    textprintf_ex(jogo->buffer, font, 5, 15, makecol(255,255,255), 0,
        " Pausado: %d ", jogo->pausado);
        
    /* Musica */
    long pos = midi_time;
    textprintf_ex(jogo->buffer, font, 5, 25, makecol(255,255,255), 0,
        " Musica: %02d:%02d / %02d:%02d ", pos / 60, pos % 60,
        config->tam_musica / 60, config->tam_musica % 60);

    /* Tabuleiro */
    int l,c;
    for (l = 0; l < NUM_LINHAS; l++) {
        textprintf_ex(jogo->buffer, font, 575, 100 + l * 10, makecol(255,255,255), 0,
            "%d:", l);
        for (c = 0; c < NUM_COLUNAS; c++) {
            textprintf_ex(jogo->buffer, font, 600 + c * 10, 100 + l * 10, makecol(255,255,255), 0,
                "%d", jogo->tabuleiro[l][c]);
        }
    }
    
    /* Peca caindo */
    for (l = 0; l < 4; l++) {
        for (c = 0; c < 4; c++) {
            textprintf_ex(jogo->buffer, font, 600 + c * 10, 500 + l * 10, makecol(255,255,255), 0,
                "%d", jogo->peca_caindo.disposicoes[(int)jogo->peca_caindo.disposicao].matriz[l][c]);
        }
    }

    /* Numero de pecas alocadas */
    /*
    textprintf_ex(jogo->buffer, font, 5, 5, makecol(255,255,255), 0,
        " Pecas alocadas: %d ", jogo->num_pecas);
    int i;
    for (i = 0; i < jogo->num_pecas; i++) {
        textprintf_ex(jogo->buffer, font, 5, (i + 2) * 10, makecol(255,255,255), 0,
            " Peca %d tem: %d disposicoes ", i, jogo->pecas[i].num_disposicoes);
    }
    */

    #endif
}


/**
 * Tela do Jogo
 */
void loop_jogo(void) {
    unsigned int tempo = wlclock();
    int linha, coluna;
    
    /* Obter nivel em que o jogo foi iniciado */
    jogo->nivel_inicio = jogo->nivel;
    set_nivel(jogo->nivel);
    
    zerar_pontuacao();
    jogo->encerrado = 0;   /* Iniciar sem estar encerrado */
    jogo->pausado   = 0;   /* Pause desativado            */
    
    /* Sortear uma peca inicial */
    srand(wlclock());
    jogo->peca_caindo = get_peca_aleatoria();
    jogo->peca_caindo.coluna = MEIO_AREA;
    jogo->peca_caindo.linha = -3;
    
    /* Sortear a proxima peca */
    jogo->peca_seguinte = get_peca_aleatoria();
    
    /* Limpar o tabuleiro */
    for (linha = 0; linha < NUM_LINHAS; linha++) {
        for (coluna = 0; coluna < NUM_COLUNAS; coluna++) {
            jogo->tabuleiro[linha][coluna] = 0;
        }
    }

    loop_infinito {
        
        trocar_musica();
        
        /* Desenhar na tela */
        preparar_buffer();
        desenhar_jogo();
        exibir_buffer();
        
        /* Tratar eventos  */
        inicio_game_speed
        
        if (!jogo->pausado) {
            if (!descer_peca()) {
                fixar_peca();
                eliminar_linhas();
                if (jogo->encerrado) {
                    jogo->opcao = OP_RECORDE;
                    sair_loop;
                } else {
                    trocar_peca();
                }
            }
        }

        /* Tratar mouse */
        if ((mouse_b & 1) && passou(tempo, MENU_TEMPO)) {
            tempo = wlclock();
            if (mouse_sobre(&(botoes->bt_pausar))) {
                mudar_pause();
            } else if (mouse_sobre(&(botoes->bt_sair))) {
                jogo->opcao = OP_MENU;
                sair_loop;
            }
        }

        /* Tratar teclado */
        if (keypressed()) {
            
            /* Quando pausado */
            if (jogo->pausado) {
                switch (readkey() >> 8) {
                case TECLA_PAUSAR:
                    mudar_pause();
                    break;
                
                /* Opcoes que saem do jogo */
                case TECLA_SAIR:
                    tocar(SOM_BOTAO);
                    jogo->opcao = OP_MENU;
                    sair_loop;
                }
                
            /* Quando despausado */
            } else  {
            
                switch (readkey() >> 8) {

                /* Opcoes gerais */
                case KEY_UP:
                    if (pode_girar()) {
                        girar_peca();
                    }
                    break;

                case KEY_DOWN:
                    if (pode_descer()) {
                        tocar(SOM_BOTAO);
                        jogo->peca_caindo.linha++;
                    }
                    break;

                case KEY_SPACE:
                    tocar(SOM_DESCER);
                    while (pode_descer()) {
                        jogo->peca_caindo.linha++;
                    }
                    tocar(SOM_CAIR);
                    break;

                case KEY_LEFT:
                    if (pode_mover(ESQUERDA)) {
                        tocar(SOM_BOTAO);
                        jogo->peca_caindo.coluna--;
                    }
                    break;

                case KEY_RIGHT:
                    if (pode_mover(DIREITA)) {
                        tocar(SOM_BOTAO);
                        jogo->peca_caindo.coluna++;
                    }
                    break;

                case TECLA_MUDAR_TELA:
                    alterar_modo_tela();
                    break;
                
                case TECLA_PAUSAR:
                    mudar_pause();
                    break;

                /* Opcoes que saem do jogo */
                case TECLA_SAIR:
                    tocar(SOM_BOTAO);
                    jogo->opcao = OP_MENU;
                    sair_loop;
                }
            }
        }
        fim_game_speed
    }
}


/**
 * Obtem os 10 maiores recordes do arquivo
 * @param recordes vetor onde serao armazenados os recordes obtidos
 */
void get_recordes(recorde **recordes) {
    FILE *arquivo;

    /* Zerar o vetor */
    memset(*recordes, 0, 10 * sizeof(recorde));

    /* Obter recordes do arquivo */
    arquivo = fopen(ARQ_RECORDES, "rb");
    fread(*recordes, sizeof(recorde), 10, arquivo);
    fclose(arquivo);
}


/**
 * Salva um recorde na memoria
 */
void salvar_recorde(recorde **recordes) {
    FILE *arquivo;
    arquivo = fopen(ARQ_RECORDES, "wb");
    if (ferror(arquivo)) {
        return;
    }
    fwrite(*recordes, sizeof(recorde), 10, arquivo);
    fclose(arquivo);
}


/**
 * Checa se a pontuacao e' um recorde e retorna um ponteiro para a 
 * posicao do recorde, caso contrario retorna NULL
 * @param recordes recordes anteriores
 * @param pontos pontos obtidos na jogada
 * @param nivel nivel acancado
 * @return um ponteiro para a posicao do vetor onde o recorde parou ou NULL
 */
recorde *atualizar_recordes(recorde **recordes, const unsigned int pontos, const char nivel) {
    char i;
    recorde *retorno = NULL;

    /* Checar se e' um recorde */
    if (((*recordes)[9].pontos > pontos) ||
        (((*recordes)[9].pontos == pontos) && ((*recordes)[9].nivel < nivel))) {
        return NULL;
    }

    /* Buscar pela posicao correta */
    for (i = 8; i >= 0; i--) {
        if (((*recordes)[(int)i].pontos < pontos) ||
            (((*recordes)[(int)i].pontos == pontos) && ((*recordes)[(int)i].nivel) > nivel)) {
            memcpy((*recordes) + (i + 1), (*recordes) + i, sizeof(recorde));
            retorno = (*recordes) + i;
        } else {
            break;
        }
    }
    
    return retorno;
}


/**
 * Desenha os recordes
 * @param recordes os 10 maiores recordes
 */
void desenhar_recorde(recorde **recordes) {
    BITMAP *img = NULL;
    char i;
    static const int w = 300;
    static const int h = 300;
    static const int x = (W / 2) - (w / 2);
    static const int y = (H / 2) - (h / 2);
    
    static const int w2 = w - 20;
    static const int h2 = h - 20;
    static const int x2 = x + ((w / 2) - (w2 / 2));
    static const int y2 = y + ((h / 2) - (h2 / 2));

    /* Fundo */
    img = (BITMAP *)config->imgs[IMG_FUNDO].dat;
    blit(img, jogo->buffer, 0, 0, 0, 0, img->w, img->h);

    /* Retangulo com os recordes no centro da tela */
    rectfill(jogo->buffer, x, y, x + w, y + h, makecol(240, 240, 100));
    fastline(jogo->buffer, x, y, x + w, y, makecol(255, 255, 200));
    fastline(jogo->buffer, x, y, x, y + h, makecol(255, 255, 230));
    fastline(jogo->buffer, x + w, y, x + w, y + h, makecol(140, 140, 0));
    fastline(jogo->buffer, x, y + h, x + w, y + h, makecol(130, 130, 0));

    /* Retangulo preto com os 10 recordes */
    rectfill(jogo->buffer, x2, y2, x2 + w2, y2 + h2, makecol(40, 40, 40));
    fastline(jogo->buffer, x2, y2, x2 + w2, y2, makecol(80, 80, 80));
    fastline(jogo->buffer, x2, y2, x2, y2 + h2, makecol(0, 0, 0));
    fastline(jogo->buffer, x2 + w2, y2, x2 + w2, y2 + h2, makecol(180, 180, 180));
    fastline(jogo->buffer, x2, y2 + h2, x2 + w2, y2 + h2, makecol(200, 200, 200));
    
    textprintf_ex(jogo->buffer, font, x2 + 10, y2 + 10,
                  makecol(255,255,255), -1, "Nome");
    textprintf_ex(jogo->buffer, font, x2 + w2 - 100, y2 + 10,
                  makecol(255,255,255), -1, "Nivel");
    textprintf_ex(jogo->buffer, font, x2 + w2 - 50, y2 + 10,
                  makecol(255,255,255), -1, "Pontos");
    for (i = 0; i < 10; i++) {
        textprintf_ex(jogo->buffer, font, x2 + 10, y2 + 40 + (25 * i),
                      makecol(255,255,255), -1,
                      "%s", (*recordes)[(int)i].nome);
        textprintf_ex(jogo->buffer, font, x2 + w2 - 90, y2 + 40 + (25 * i),
                      makecol(255,255,255), -1,
                      "%d", (*recordes)[(int)i].nivel);
        textprintf_ex(jogo->buffer, font, x2 + w2 - 50, y2 + 40 + (25 * i),
                      makecol(255,255,255), -1,
                      "%d", (*recordes)[(int)i].pontos);
    }
}


/**
 * Tela do Recorde
 */
void loop_recorde(void) {
    recorde *recordes = NULL;
    recorde *atual = NULL;
    int tecla = 0;
    int tamanho = 0;
    
    recordes = (recorde *)malloc(10 * sizeof(recorde));

    /* Obter recordes do arquivo */
    get_recordes(&recordes);
    
    /* Checa se e' um recorde e, caso seja, retorna um ponteiro para o recorde
     * caso contrario, retorna NULL
     */
    atual = atualizar_recordes(&recordes, jogo->pontos, jogo->nivel);

    /* Se nao e' um recorde */
    if (atual == NULL) {
        jogo->opcao = OP_MENU;
        return;
    }
    
    /* Montar recorde atual */
    atual->nome[0] = '\0';
    atual->pontos  = jogo->pontos;
    atual->linhas  = jogo->linhas;
    atual->nivel   = jogo->nivel;

    loop_infinito {
        
        /* Desenhar na tela */
        preparar_buffer();
        desenhar_recorde(&recordes);
        exibir_buffer();
        
        /* Tratar eventos  */
        inicio_game_speed
        
        /* Tratar teclado */
        if (keypressed()) {
            tecla = readkey();
            switch (tecla >> 8) {
                
            /* Teclas para escrever o recorde */
            case KEY_BACKSPACE:
                tamanho = strlen(atual->nome);
                if (tamanho) {
                    atual->nome[tamanho - 1] = 0;
                }
                break;
            default:
                tamanho = strlen(atual->nome);
                if (tamanho < 20) {
                    atual->nome[tamanho] = (char)(tecla & 0xff);
                    atual->nome[tamanho + 1] = 0;
                }
                break;
                
            /* Teclas para sair do jogo */
            case KEY_ENTER:
                tocar(SOM_BOTAO);
                salvar_recorde(&recordes);
                jogo->opcao = OP_MENU;
                sair_loop;
            case TECLA_SAIR:
                tocar(SOM_BOTAO);
                jogo->opcao = OP_MENU;
                sair_loop;
            }
        }
        fim_game_speed
    }
    free(recordes);
}

