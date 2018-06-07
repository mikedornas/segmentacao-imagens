#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "tp1.h"

#define RECURSIVO 1

FILE *abir_arquivo(char *nome, char *extensao)
{
    char nome_arquivo[40];
    // Primeiramente verifica se o usuario possui a pasta com as imagens
    sprintf(nome_arquivo, "imagens_v2//%s%s", nome, extensao);
    FILE *arquivo = fopen(nome_arquivo, "r");

    if(arquivo != NULL) return arquivo;
    else
    {
        // Caso contrario, abre a imagem caso ela esteja no mesmo diretorio do executavel
        sprintf(nome_arquivo, "%s%s", nome, extensao);
        arquivo = fopen(nome_arquivo, "r");
    }
    if(arquivo != NULL) return arquivo;
    else
    {
        // Se nao encontar a imagem termina o programa
        printf("\nERRO! Arquivo %s%s nao encontrado.\n", nome, extensao);
        exit(1);
    }
}

TipoImagem *salvar_imagem_entrada(FILE *arquivo_imagem)
{
    TipoImagem *imagem = (TipoImagem*) malloc(sizeof(TipoImagem));

    fscanf(arquivo_imagem, "P2\n%d %d\n%d\n", &imagem->num_colunas, &imagem->num_linhas, &imagem->nivel_intensidade);

    imagem->matriz_pixels = (TipoPixel**) calloc(imagem->num_linhas, sizeof(TipoPixel*));
    for(int i = 0; i < imagem->num_linhas; i++)
    {
        imagem->matriz_pixels[i] = (TipoPixel*) calloc(imagem->num_colunas, sizeof(TipoPixel));
        for(int j = 0; j < imagem->num_colunas; j++)
        {
            // Define os valores de RGB de cada pixel da iamgem de saida iguais aos valores de intensidade da imagem .pgm
            fscanf(arquivo_imagem, "%d", &imagem->matriz_pixels[i][j].intensidade);
            imagem->matriz_pixels[i][j].R = imagem->matriz_pixels[i][j].intensidade * (255 / imagem->nivel_intensidade);
            imagem->matriz_pixels[i][j].G = imagem->matriz_pixels[i][j].intensidade * (255 / imagem->nivel_intensidade);
            imagem->matriz_pixels[i][j].B = imagem->matriz_pixels[i][j].intensidade * (255 / imagem->nivel_intensidade);
        }
    }
    fclose(arquivo_imagem);
    return imagem;
}

void definir_metodo_segmentacao(int *automatico, int *metodo)
{
    /* Caso o usuario pressione uma tecla diferente das definidas, o modo padrão
    é escolhido: metodo automatico e usando Fila */
    char ch;
    printf("\nPara utilizar sementes aleatorias [PRESSIONE A] ou usar arquivo auxiliar [PRESSIONE M]: ");
    ch = getchar();
    if(ch == 'm' || ch == 'M')
        *automatico = 0;

    printf("\nPara realizar segmentacao de forma recursiva [PRESSIONE R] ou iterativa [PRESSIONE I]: ");
    scanf("\n%c", &ch);
    if(ch == 'r' || ch == 'R')
        *metodo = RECURSIVO;
}

TipoSemente *salvar_arquivo_auxiliar(FILE *arquivo_auxilar, int *Num_sementes, int *Limiar)
{
    fscanf(arquivo_auxilar, "%d\n%d\n", Num_sementes, Limiar);
    TipoSemente *semente = (TipoSemente*) calloc(*Num_sementes, sizeof(TipoSemente));
    for(int i = 0; i < *Num_sementes; i++)
    {
        fscanf(arquivo_auxilar, "%d, %d <%d, %d, %d>\n", &semente[i].x, &semente[i].y, &semente[i].R, &semente[i].G, &semente[i].B);
    }
    fclose(arquivo_auxilar);
    return semente;
}

void liberar_imagem_entrada(TipoImagem *imagem)
{
    for(int i = 0; i < imagem->num_linhas; i++)
        free(imagem->matriz_pixels[i]);

    free(imagem->matriz_pixels);
    free(imagem);
}

void liberar_sementes(TipoSemente *semente)
{
    free(semente);
}

void segmentar_imagem_usando_arquivo_auxiliar(TipoImagem *imagem, TipoSemente *semente, int Num_sementes, int Limiar, int metodo)
{
    if(metodo == RECURSIVO)
    {
        //Segementa usando a funcao recursiva
        for(int i = 0; i < Num_sementes; i++)
        {
            int tamanho_regiao = 0;
            colorir_regiao_recursivamente(imagem, &semente[i], semente[i].x, semente[i].y, Limiar, &tamanho_regiao);
        }
        printf("\nImagem criada recursivamente\n");
    }
    else
    {
        // Segmenta usando o conceito de uma Fila
        TipoFila *Fila = (TipoFila*) malloc(sizeof(TipoFila));
        FFVazia(Fila);
        for(int i = 0; i < Num_sementes; i++)
        {
            int tamanho_regiao = 0;
            colorir_regiao_usando_fila(Fila, imagem, &semente[i], Limiar, &tamanho_regiao);
        }
        free(Fila->Inicio);
        free(Fila);
        printf("\nImagem criada usando fila.\n");
    }
}

void gera_semente_aleatoria(TipoImagem *imagem, TipoSemente *semente_aleatoria)
{
    semente_aleatoria->R = rand() % 255;
    semente_aleatoria->G = rand() % 255;
    semente_aleatoria->B = rand() % 255;
    semente_aleatoria->x = rand() % imagem->num_colunas;
    semente_aleatoria->y = rand() % imagem->num_linhas;
}

void segmentar_imagem_usando_sementes_aleatorias(TipoImagem *imagem, int Limiar, int metodo)
{
    srand(time(NULL));

    TipoSemente *semente_aleatoria;
    semente_aleatoria = (TipoSemente*) calloc(1, sizeof(TipoSemente));

    for(int i = 0; i < (imagem->num_linhas + imagem->num_colunas)/2; i++)
    {
        gera_semente_aleatoria(imagem, semente_aleatoria);

        if(!imagem->matriz_pixels[semente_aleatoria->y][semente_aleatoria->x].pixel_alterado)
        {
            int tamanho_regiao = 0;
            if(metodo == RECURSIVO)
                colorir_regiao_recursivamente(imagem, semente_aleatoria, semente_aleatoria->x, semente_aleatoria->y, Limiar, &tamanho_regiao);
            else
            {
                TipoFila *Fila = (TipoFila*) malloc(sizeof(TipoFila));
                FFVazia(Fila);
                colorir_regiao_usando_fila(Fila, imagem, semente_aleatoria, Limiar, &tamanho_regiao);
                free(Fila->Inicio);
                free(Fila);
            }
        }
    }
    free(semente_aleatoria);
    if(metodo == RECURSIVO)
      printf("\nImagem criada recursivamente\n");
    else
      printf("\nImagem criada usando fila.\n");
}

void colorir_regiao_usando_fila(TipoFila *Fila, TipoImagem *imagem, TipoSemente *semente, int Limiar, int *tamanho_regiao)
{
    if(!imagem->matriz_pixels[semente->y][semente->x].pixel_alterado)
    {
        (*tamanho_regiao)++;
        Enfileira(Fila, *semente, imagem);
    }

    while(!Vazia(Fila))
    {
        int linha_atual = Fila->Inicio->Prox->Item.y;
        int coluna_atual = Fila->Inicio->Prox->Item.x;

/* As condicoes para enfileirar um pixel (ou seja, agregar esse pixel a regiao) sao: suas cordenadas (x,y)
entenjam dentro da limite da imagem; o pixel nao tiver sido analisado e a diferenca entre sua intensidade
de cor e a da regiao for menor ou igual ao limiar predefinido */
        //Pixel DIREITA
        if(coluna_atual < (imagem->num_colunas - 1) && !imagem->matriz_pixels[linha_atual][coluna_atual + 1].pixel_alterado &&
           abs(imagem->matriz_pixels[linha_atual][coluna_atual].intensidade - imagem->matriz_pixels[linha_atual][coluna_atual + 1].intensidade) <= Limiar)
        {
            (*tamanho_regiao)++;
            TipoSemente pixel_direita = Fila->Inicio->Prox->Item;
            pixel_direita.x++;
            Enfileira(Fila, pixel_direita, imagem);
        }
        //Pixel ESQUERDA
        if(coluna_atual > 0 && !imagem->matriz_pixels[linha_atual][coluna_atual - 1].pixel_alterado &&
           abs(imagem->matriz_pixels[linha_atual][coluna_atual].intensidade - imagem->matriz_pixels[linha_atual][coluna_atual - 1].intensidade) <= Limiar)
        {
            (*tamanho_regiao)++;
            TipoSemente pixel_esquerda = Fila->Inicio->Prox->Item;
            pixel_esquerda.x--;
            Enfileira(Fila, pixel_esquerda, imagem);
        }
        //Pixel BAIXO
        if(linha_atual < (imagem->num_linhas - 1) && !imagem->matriz_pixels[linha_atual + 1][coluna_atual].pixel_alterado &&
           abs(imagem->matriz_pixels[linha_atual][coluna_atual].intensidade - imagem->matriz_pixels[linha_atual + 1][coluna_atual].intensidade) <= Limiar)
        {
            (*tamanho_regiao)++;
            TipoSemente pixel_baixo = Fila->Inicio->Prox->Item;
            pixel_baixo.y++;
            Enfileira(Fila, pixel_baixo, imagem);
        }
        //Pixel CIMA
        if(linha_atual > 0 && !imagem->matriz_pixels[linha_atual - 1][coluna_atual].pixel_alterado &&
           abs(imagem->matriz_pixels[linha_atual][coluna_atual].intensidade - imagem->matriz_pixels[linha_atual - 1][coluna_atual].intensidade) <= Limiar)
        {
            (*tamanho_regiao)++;
            TipoSemente pixel_cima = Fila->Inicio->Prox->Item;
            pixel_cima.y--;
            Enfileira(Fila, pixel_cima, imagem);
        }
/* Ao desenfileirar um pixel, sua cor RGB é alterada */
        Desenfileira(Fila, imagem, tamanho_regiao);
    }
}

void colorir_regiao_recursivamente(TipoImagem *imagem, TipoSemente *semente, int x, int y, int Limiar, int *tamanho_regiao)
{
/* Para agregar um novo pixel a regiao, suas cordenadas (x,y) devem estar dentro da limite da imagem; o pixel nao tiver sido analisado e a diferenca entre sua intensidade
de cor e a da regiao for menor ou igual ao limiar predefinido */
    //Pixel ESQUERDA
    if(x > 0 && !imagem->matriz_pixels[y][x - 1].pixel_alterado &&
       abs(imagem->matriz_pixels[y][x].intensidade - imagem->matriz_pixels[y][x - 1].intensidade) <= Limiar)
    {
        (*tamanho_regiao)++;
        imagem->matriz_pixels[y][x - 1].pixel_alterado = 1;
        colorir_regiao_recursivamente(imagem, semente, x-1, y, Limiar, tamanho_regiao);
    }
    //Pixel DIREITA
    if(x < (imagem->num_colunas - 1) && !imagem->matriz_pixels[y][x + 1].pixel_alterado &&
       abs(imagem->matriz_pixels[y][x].intensidade - imagem->matriz_pixels[y][x + 1].intensidade) <= Limiar)
    {
        (*tamanho_regiao)++;
        imagem->matriz_pixels[y][x + 1].pixel_alterado = 1;
        colorir_regiao_recursivamente(imagem, semente, x+1, y, Limiar, tamanho_regiao);
    }
    //Pixel CIMA
    if(y > 0 && !imagem->matriz_pixels[y - 1][x].pixel_alterado &&
       abs(imagem->matriz_pixels[y][x].intensidade - imagem->matriz_pixels[y - 1][x].intensidade) <= Limiar)
    {
        (*tamanho_regiao)++;
        imagem->matriz_pixels[y - 1][x].pixel_alterado = 1;
        colorir_regiao_recursivamente(imagem, semente, x, y-1, Limiar, tamanho_regiao);
    }
    //Pixel BAIXO
    if(y < (imagem->num_linhas - 1) && !imagem->matriz_pixels[y + 1][x].pixel_alterado &&
       abs(imagem->matriz_pixels[y][x].intensidade - imagem->matriz_pixels[y + 1][x].intensidade) <= Limiar)
    {
        (*tamanho_regiao)++;
        imagem->matriz_pixels[y + 1][x].pixel_alterado = 1;
        colorir_regiao_recursivamente(imagem, semente, x, y+1, Limiar, tamanho_regiao);
    }

    if(*tamanho_regiao > 0.00008*(imagem->num_linhas * imagem->num_colunas))
    {
        imagem->matriz_pixels[y][x].R = semente->R;
        imagem->matriz_pixels[y][x].G = semente->G;
        imagem->matriz_pixels[y][x].B = semente->B;
    }
}

void criar_imagem_PPM(TipoImagem *imagem, char *nome)
{
    char nome_imagem[30];
    sprintf(nome_imagem, "%s.ppm", nome);
    FILE *imagem_PPM = fopen(nome_imagem, "w");
    if(imagem_PPM == NULL)
    {
        printf("\nErro ao criar imagem .ppm\n");
        exit(1);
    }

    fprintf(imagem_PPM, "P3\n%d %d\n255\n", imagem->num_colunas, imagem->num_linhas);

    for(int i = 0; i < imagem->num_linhas; i++)
    {
        for(int j = 0; j < imagem->num_colunas; j++)
        {
            fprintf(imagem_PPM, "%d %d %d ", imagem->matriz_pixels[i][j].R, imagem->matriz_pixels[i][j].G, imagem->matriz_pixels[i][j].B);
        }
        fprintf(imagem_PPM, "\n");
    }

    fclose(imagem_PPM);
}

void FFVazia(TipoFila *Fila)
{
    Fila->Inicio = (Apontador) calloc(1, sizeof(Celula));
    Fila->Fim = Fila->Inicio;
    Fila->Inicio->Prox = NULL;
}

int Vazia(const TipoFila *Fila)
{
    return (Fila->Inicio == Fila->Fim);
}

void Enfileira(TipoFila *Fila, TipoSemente pixel, TipoImagem *imagem)
{
    imagem->matriz_pixels[pixel.y][pixel.x].pixel_alterado = 1;
    Fila->Fim->Prox = (Apontador) malloc(sizeof(Celula));
    Fila->Fim = Fila->Fim->Prox;
    Fila->Fim->Item = pixel;
    Fila->Fim->Prox = NULL;
}

void Desenfileira(TipoFila *Fila, TipoImagem *imagem, int *tamanho_regiao)
{
    Apontador q = Fila->Inicio;
    Fila->Inicio = Fila->Inicio->Prox;
    free(q);
    TipoSemente pixel_atual = Fila->Inicio->Item;
    if(*tamanho_regiao > 4)
    {
        imagem->matriz_pixels[pixel_atual.y][pixel_atual.x].R = pixel_atual.R;
        imagem->matriz_pixels[pixel_atual.y][pixel_atual.x].G = pixel_atual.G;
        imagem->matriz_pixels[pixel_atual.y][pixel_atual.x].B = pixel_atual.B;
    }
}

void segmentar_imagem_detectando_regioes(TipoImagem *imagem, int Limiar, int metodo)
{
    srand(time(NULL));

    TipoSemente *semente;
    semente = (TipoSemente*) calloc(1, sizeof(TipoSemente));

/* Percorre a matriz contendo os pixels da imagem e encontra o primeiro pixel NAO ALTERADO,
    esse pixel se torna uma semente para expandir a regiao, em seguida é chamada a funcao
    de segmentacao (seja ela recursiva ou usando Fila) para esse valor de semente */
    for(int i = 0; i < imagem->num_linhas; i++)
    {
        for(int j = 0; j < imagem->num_colunas; j++)
        {
            if(!imagem->matriz_pixels[i][j].pixel_alterado)
            {
                // Correcao para nao utilizar o ponto critico (0,0), no caso para a imagem Split_2
                if(i == 0 && j == 0) {semente->x = (imagem->num_colunas/2); semente->y = (imagem->num_linhas/2);}
                else {
                semente->x = j;
                semente->y = i;
                }
                semente->R = rand() % 255;
                semente->G = rand() % 255;
                semente->B = rand() % 255;
                int tamanho_regiao = 0;

                if(metodo == RECURSIVO)
                    colorir_regiao_recursivamente(imagem, semente, semente->x, semente->y, Limiar, &tamanho_regiao);
                else
                {
                    TipoFila *Fila = (TipoFila*) malloc(sizeof(TipoFila));
                    FFVazia(Fila);
                    colorir_regiao_usando_fila(Fila, imagem, semente, Limiar, &tamanho_regiao);
                    free(Fila->Inicio);
                    free(Fila);
                }
            }
        }
    }
    free(semente);
}
