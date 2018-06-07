/* Armazena para cada pixel os valores de intensidade da imagem .pgm,
os valores de RGB para a imagem .ppm
e se o pixel ja foi analisado */
typedef struct TipoPixel {
    int intensidade;
    int R, G, B;
    int pixel_alterado;
} TipoPixel;

/* Armazena os dados da imagem a ser segmentada */
typedef struct TipoImagem {
    int num_linhas;
    int num_colunas;
    int nivel_intensidade;
    TipoPixel **matriz_pixels;
} TipoImagem;

/* Armazena os valores de RGB para o pixel da posicao (x,y) */
typedef struct TipoSemente {
    int x, y;
    int R, G, B;
} TipoSemente;

typedef struct Celula *Apontador;

/* Define uma celula da Fila */
typedef struct Celula {
    TipoSemente Item;
    Apontador Prox;
} Celula;

/* Define um tipo Fila */
typedef struct TipoFila {
    Apontador Inicio, Fim;
} TipoFila;

/* Retorna um ponteiro para o arquivo aberto conforme o nome e a extensao passados */
FILE *abir_arquivo(char *nome, char *extensao);

/* A funcao recebe um ponteiro para um arquivo de imagem .pgm, le e armazenar
seus dados em um TipoImagem e o retorna */
TipoImagem *salvar_imagem_entrada(FILE *arquivo_imagem);

/* Desaloca o espaco utilizado para salvar os dados da imagem */
void liberar_imagem_entrada(TipoImagem *imagem);

/* A funcao pergunta ao usuario e seleciona qual metodo de segmentacao sera
utilizado: recursivo ou iterativo, automatico ou usando sementes predefinidas */
void definir_metodo_segmentacao(int *automatico, int *recursivo);

/* A funcao recebe um ponteiro para um arquivo de sementes ja aberto
e armazena as sementes em um vetor de TipoSemente */
TipoSemente *salvar_arquivo_auxiliar(FILE *arquivo_auxilar, int *Num_sementes, int *Limiar);

/* Desaloca o espaco utilizado para salvar as sementes */
void liberar_sementes(TipoSemente *semente);

/* A funcao recebe um TipoImagem contendo os dados da imagem e seguimenta a regiao
utilizando as sementes armazanadas no TipoSemente */
void segmentar_imagem_usando_arquivo_auxiliar(TipoImagem *imagem, TipoSemente *semente, int Num_sementes, int Limiar, int metodo);

/* A funcao gera valores aleatorios para RGB e para a posicao (x,y) da semente
e armazena no TipoSemente recebido como parametro. */
void gera_semente_aleatoria(TipoImagem *imagem, TipoSemente *semente_aleatoria);

 /* A funcao recebe um TipoImagem contendo os dados da imagem, chama a funcao "gera_semente_aleatoria(  )"
 para gerar uma semente e segmenta a regiao a partir dessa semente */
void segmentar_imagem_usando_sementes_aleatorias(TipoImagem *imagem, int Limiar, int metodo);

/* A funcao Utiliza o conceito de uma Fila para expandir e segmentar as regioes */
void colorir_regiao_usando_fila(TipoFila *Fila, TipoImagem *imagem, TipoSemente *semente, int Limiar, int *tamanho_regiao);

/* A funcao expande e segmenta as regioes usando recursao */
void colorir_regiao_recursivamente(TipoImagem *imagem, TipoSemente *semente, int x, int y, int Limiar, int *tamanho_regiao);

/* A funcao recebe os dados da imagem apos ser segmentada e cria um arquivo de
imagem no formato .ppm com a nova imagem */
void criar_imagem_PPM(TipoImagem *imagem, char *nome);

/* Cria uma Fila vazia */
void FFVazia(TipoFila *Fila);

/* Analisa se a Fila esta vazia, se sim retorna 1 caso contrario retorna 0 */
int Vazia(const TipoFila *Fila);

/* Acrecenta mais um pixel do TipoSemente a Fila */
void Enfileira(TipoFila *Fila, TipoSemente pixel, TipoImagem *imagem);

/* Retira um item (pixel) da Fila e altera as cores desse pixel */
void Desenfileira(TipoFila *Fila, TipoImagem *imagem, int *tamanho_regiao);

/* Funcao extra, porem similar a "segmentar_imagem_usando_sementes_aleatorias(  )".
A sua diferenca é que ela identifica todas as regioes existentes na imagem para o valor
de limiar recebido, dessa forma todos os pixels do arquivo imagem sao analisados e segmentados */
void segmentar_imagem_detectando_regioes(TipoImagem *imagem, int Limiar, int metodo);
