/************************************************************/
/* DCC004 - ALGORITMOS E ESTRUTURAS DE DADOS II             */
/* Prof.: Cristiano Arbex Valle e Gisele L. Pappa           */
/* TP1 - Segmentacao de Imagens                             */
/* Autor: Mike Dornas Oliveira                              */
/* Matricula: 2017002199                                    */
/* Data: 06/05/2018                                         */
/************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tp1.h"

int main(int argc, char **argv)
{
    if(argc < 2)  //Testa se foi passado o nome da imagem
    {
        printf("\nErro! Passe o nome da imagem\n");
        return 1;
    }

    FILE *arquivo_imagem = NULL;
    TipoImagem *imagem = NULL;

    arquivo_imagem = abir_arquivo(argv[1], ".pgm");   // Abre o arquivo da imagem .pgm
    imagem = salvar_imagem_entrada(arquivo_imagem);   // Salva os dados da imagem em uma matriz dinamica

    int automatico = 1, recursivo = 0;
    definir_metodo_segmentacao(&automatico, &recursivo);  // O metodo default é automatico e usando FILA

    if(automatico)
    {
        int LIMIAR = 10;
        segmentar_imagem_usando_sementes_aleatorias(imagem, LIMIAR, recursivo);

        //segmentar_imagem_detectando_regioes(imagem, LIMIAR, recursivo);

        /* Coloquei essa funcao comentada apenas para comparacao com a funcao anterior.
        Para utiliza-la, comente a funcao "segmentar_imagem_usando_sementes_aleatorias(imagem, LIMIAR, recursivo);"
        e descomente essa.
        Olhar Documentacao e tp1.h para mais detalhes */
    }
    else
    {
        int Num_sementes = 0, Limiar = 0;
        FILE *arquivo_auxilar = NULL;
        TipoSemente *semente = NULL;

        arquivo_auxilar = abir_arquivo(argv[1], ".txt");
        semente = salvar_arquivo_auxiliar(arquivo_auxilar, &Num_sementes, &Limiar); // Armazena as sementes do arquivo auxiliar

        segmentar_imagem_usando_arquivo_auxiliar(imagem, semente, Num_sementes, Limiar, recursivo);

        liberar_sementes(semente);
    }

    criar_imagem_PPM(imagem, argv[1]);  // Cria o arquivo para a nova imagem com as regioes ja segmentadas
    liberar_imagem_entrada(imagem);   // Desaloca a matriz dinamica

    return 0;
}
