/*
Começando desde a parte dos Include. Os 3 primeiros ( <stdio.h>, <stdlib.h> e <string.h> )são bibliotecas usadas em praticamente todos os códigos.

A biblioteca <ctype.h> é usada para fornecer funções de classificação de caracteres,
sendo essencial para a lógica do scanner, enquanto <stdbool.h> fornece true, false e bool, ajudando na legibilidade de flags.

Ademais <locale.h> serve para mostrar os ascentos de forma correta.

Por fim, o uso de #ifdef _Win32 e strcasecmp_stricmp serve para comparar strings sem diferenciação de letras minúsculas e maiúsculas.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <locale.h> 


#ifdef _WIN32
#define strcasecmp _stricmp
#endif

/*
~ Typedef enum e Tipotoken ~
Enum contém todos os tipos de token, padronizando a identificação dos tokens.

Const char* tipo_token_para_string[], é um array paralelo ao enum, que serve para impressão textual do token, importante para sincronizar a ordem com o enum.

Typedef struct { tipotoken tipo; char lexema[256]; int linha; int coluna; } token;, mostra um token reconhecido. Posição incial ( Coluna / Linha ) e texto ( Lexama ).

Entradatabelasimbolos, serve para a entrada da tabela de símbolos, salvando o token associado e o lexema.

*/

typedef enum {
    // Palavras-chave
    TOKEN_KEY_PROGRAM, TOKEN_KEY_VAR, TOKEN_KEY_INTEGER, TOKEN_KEY_REAL, TOKEN_KEY_BEGIN, TOKEN_KEY_END,
    TOKEN_KEY_IF, TOKEN_KEY_THEN, TOKEN_KEY_ELSE, TOKEN_KEY_WHILE, TOKEN_KEY_DO,
    // Identificador
    TOKEN_ID,
    // Literais
    TOKEN_LIT_INT, TOKEN_LIT_REAL, TOKEN_LIT_STRING,
    // Operadores
    TOKEN_OP_EQ, TOKEN_OP_GE, TOKEN_OP_MUL, TOKEN_OP_NE, TOKEN_OP_LE,
    TOKEN_OP_DIV, TOKEN_OP_GT, TOKEN_OP_AD, TOKEN_OP_LT, TOKEN_OP_MIN, TOKEN_OP_POW,
    TOKEN_OP_ASS, // Operador de atribuição :=
    // Símbolos
    TOKEN_SMB_COM, TOKEN_SMB_SEM, TOKEN_SMB_OPA, TOKEN_SMB_CPA, TOKEN_SMB_COLON,
    TOKEN_SMB_DOT, // Símbolo de ponto final
    // Token especial para a expressão binomial
    TOKEN_EXP_BINOMIAL,
    // Controle
    TOKEN_FIM_DE_ARQUIVO, TOKEN_ERRO
} TipoToken;

const char* tipo_token_para_string[] = {
    "program", "var", "integer", "real", "begin", "end",
    "if", "then", "else", "while", "do",
    "ID",
    "LIT_INT", "LIT_REAL", "LIT_STRING",
    "OP_EQ", "OP_GE", "OP_MUL", "OP_NE", "OP_LE",
    "OP_DIV", "OP_GT", "OP_AD", "OP_LT", "OP_MIN", "OP_POW",
    "OP_ASS",
    "SMB_COM", "SMB_SEM", "SMB_OPA", "SMB_CPA", "SMB_COLON",
    "SMB_DOT",
    "EXP_BINOMIAL",
    "FIM_DE_ARQUIVO", "ERRO"
};

typedef struct {
    TipoToken tipo;
    char lexema[256];
    int linha;
    int coluna;
} Token;

typedef struct {
    char lexema[100];
    TipoToken tipo;
} EntradaTabelaSimbolos;

/*
Tabela_de_simbolos[tamanho_tabela_simbolos], setamos um vetor estático com o limite de 200.

Inicializartabeladesimbolos, serve para quando o scanner encontrar um identificador keyword, devolvendo o token correto como Token_Id.

Consultarouinserirsimbolo(const char* lexema), vai andar a TS fazendo busca, com o strcasemp.
Caso encontre, vai retornar o token. Se não existia e tem espaço, vai ser inserido como Token_Id e retornar Token_Id.
Caso esteja cheio, vai imprimir  “Erro Fatal”.

Imprimirtabeladesimbolos(), vai servir para imprimir todas as entradas da TS no fim.

*/

#define TAMANHO_TABELA_SIMBOLOS 200
EntradaTabelaSimbolos tabela_de_simbolos[TAMANHO_TABELA_SIMBOLOS];
int contador_tabela_simbolos = 0;

void inicializarTabelaDeSimbolos() {
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "program"); tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_PROGRAM;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "var");     tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_VAR;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "integer"); tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_INTEGER;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "real");    tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_REAL;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "begin");   tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_BEGIN;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "end");     tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_END;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "if");      tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_IF;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "then");    tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_THEN;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "else");    tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_ELSE;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "while");   tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_WHILE;
    strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, "do");      tabela_de_simbolos[contador_tabela_simbolos++].tipo = TOKEN_KEY_DO;
}

TipoToken consultarOuInserirSimbolo(const char* lexema) {
    for (int i = 0; i < contador_tabela_simbolos; i++) {
        if (strcasecmp(tabela_de_simbolos[i].lexema, lexema) == 0) {
            return tabela_de_simbolos[i].tipo;
        }
    }
    if (contador_tabela_simbolos < TAMANHO_TABELA_SIMBOLOS) {
        strcpy(tabela_de_simbolos[contador_tabela_simbolos].lexema, lexema);
        tabela_de_simbolos[contador_tabela_simbolos].tipo = TOKEN_ID;
        contador_tabela_simbolos++;
        return TOKEN_ID;
    }
    fprintf(stderr, "ERRO FATAL: Tabela de Símbolos cheia.\n");
    return TOKEN_ERRO;
}

void imprimirTabelaDeSimbolos() {
    printf("\n--- Tabela de Símbolos Final ---\n");
    for (int i = 0; i < contador_tabela_simbolos; i++) {
        printf("  [%03d] Lexema: %-20s | Token: %s\n", i, tabela_de_simbolos[i].lexema, tipo_token_para_string[tabela_de_simbolos[i].tipo]);
    }
    printf("----------------------------------\n");
}

/* 

File* arquivo_fonte;, é o arquivo aberto para leitura.(Aqui, o micro pascal)

Int linha_atual = 1; int coluna_atual 1;, detém a posição atual no arquivo.
Usado também para atribuir a posição de começo dos tokens e erros léxicos também.

*/ 

// --- ANALISADOR LÉXICO (AFD) ---

FILE* arquivo_fonte;
int linha_atual = 1;
int coluna_atual = 1;

// --- FUNÕES UTILITÁRIAS E DE LEITURA DO SCANNER ---

/*
Char proximocaractere, lê um caractere do arquivo fgetc e atualiza linha_atual / coluna_atual.
Se “\n” incrementa a linha_atual e zera a coluna_atual, senão vai incrementar coluna_atual.

Charprevercaractere, faz um fgetc e logo após o ungetc para “adivinhar” o próximo caractere sem consumir, depois retorno esse valor.

Token criartoken, vai montar e retornar um token preenchido, depois centraliza a construção do token.

Void pularespacos, vai conseumir os espaços que estão em branco, até encontrar algo não especial, usando prevercaractere e proximoCaractere.

*/
char proximoCaractere() {
    char c = fgetc(arquivo_fonte);
    if (c == '\n') {
        linha_atual++;
        coluna_atual = 1;
    } else if (c != EOF) {
        coluna_atual++;
    }
    return c;
}

char preverCaractere() {
    char c = fgetc(arquivo_fonte);
    ungetc(c, arquivo_fonte);
    return c;
}

Token criarToken(TipoToken tipo, const char* lexema, int linha, int coluna) {
    Token token;
    token.tipo = tipo;
    strcpy(token.lexema, lexema);
    token.linha = linha;
    token.coluna = coluna;
    return token;
}

void pularEspacos() {
    while (isspace(preverCaractere()) || (unsigned char)preverCaractere() == 160 || (unsigned char)preverCaractere() == 194 || (unsigned char)preverCaractere() == 195) {
        proximoCaractere();
    }
}

// --- SCANNER  ---

/*
Token obterProximotoken é uma das nossas funções importantíssima, pois tem o autômato/loop que produz o token em cada chamada.

Variáveis int vai auxiliar com o Buffer ( Indice_lexema )

Vai chamar o proximocaractere pra que consuma o caractere C, calcula a linha_inicio e coluna_inicio,
caso isspace(c), vai ignorar e pular.

Já o identificador vai ler as letras / dígitos e underscore usando prevercaractere e proximoCaractere,
terminando o lexema.

Caso seja keyword, vai chamar consultarOuinserirsimbolo e retorna o Token correspondente, caso contrário insere e retorna Token_Id.

Strings entre aspas simples, vai ler até achar ´ fechando, se \n ou EOF antes do fechamento, vai gerar um erro léxico.

<, >, = são vistos como lookhead para <=, >= e <> + - / * ^ ; ) e são mapeados diretamente para os tokens.
Por fim, : tem lookhead para :=

Está função é a responsável pela lógica toda de reconhecimento lexical.
Ele que vai determinar os limites dos tokens, atualizando a tabela de símbolos e sinalizando os erros léxicos,
impedindo análise posterior indevida.


Também temos o uso do EOF(O EOF significa END OF FILE), que é uma constante simbólica do STDIO.H, ele desempenha dois papéis, dependendo do contexto em que é encontrado,
ele atua como um sinal de erro quando surge dentro de uma estrutura sintática inacabada (como um comentário ou string),
e como o sinal de término bem-sucedido da análise quando aparece entre tokens válidos

*/

Token obterProximoToken() {
    char lexema[256];
    int indice_lexema = 0;
    char c;

    while (true) {
        c = proximoCaractere();
        int linha_inicio = linha_atual;
        int coluna_inicio = coluna_atual - 1;

        if (c == EOF) {
            return criarToken(TOKEN_FIM_DE_ARQUIVO, "EOF", linha_atual, coluna_atual);
        }

        if (isspace(c) || (unsigned char)c == 160 || (unsigned char)c == 194 || (unsigned char)c == 195) { 
            continue; 
        }

        if (c == '{') {
            int linha_inicio_comentario = linha_inicio;
            int coluna_inicio_comentario = coluna_inicio;
            do {
                c = proximoCaractere();
                if (c == EOF) {
                    fprintf(stderr, "ERRO LÉXICO: Comentário iniciado na linha %d, coluna %d, não foi fechado antes do fim do arquivo.\n", linha_inicio_comentario, coluna_inicio_comentario);
                    return criarToken(TOKEN_ERRO, "{...", linha_inicio_comentario, coluna_inicio_comentario);
                }
            } while (c != '}');
            continue;
        }

        if (isalpha(c) || c == '_') {
            lexema[indice_lexema++] = c;
            while (isalnum(preverCaractere()) || preverCaractere() == '_') {
                lexema[indice_lexema++] = proximoCaractere();
            }
            lexema[indice_lexema] = '\0';
            TipoToken tipo = consultarOuInserirSimbolo(lexema);
            return criarToken(tipo, lexema, linha_inicio, coluna_inicio);
        }

        if (isdigit(c)) {
            lexema[indice_lexema++] = c;
            bool eh_real = false;
            while (isdigit(preverCaractere())) {
                lexema[indice_lexema++] = proximoCaractere();
            }
            if (preverCaractere() == '.') {
                char proximo = proximoCaractere(); 
                if (!isdigit(preverCaractere())) {
                    ungetc(proximo, arquivo_fonte); 
                    lexema[indice_lexema] = '\0';
                    return criarToken(TOKEN_LIT_INT, lexema, linha_inicio, coluna_inicio);
                }
                ungetc(proximo, arquivo_fonte); 
                eh_real = true;
                lexema[indice_lexema++] = proximoCaractere();
                while (isdigit(preverCaractere())) {
                    lexema[indice_lexema++] = proximoCaractere();
                }
            }
            lexema[indice_lexema] = '\0';
            return criarToken(eh_real ? TOKEN_LIT_REAL : TOKEN_LIT_INT, lexema, linha_inicio, coluna_inicio);
        }

        if (c == '\'') {
            indice_lexema = 0;
            while ((c = proximoCaractere()) != '\'' && c != '\n' && c != EOF) {
                if (indice_lexema < 255) lexema[indice_lexema++] = c;
            }
            lexema[indice_lexema] = '\0';
            if (c == '\'') {
                return criarToken(TOKEN_LIT_STRING, lexema, linha_inicio, coluna_inicio);
            } else {
                fprintf(stderr, "ERRO LÉXICO: String iniciada na linha %d, coluna %d, não foi fechada na mesma linha.\n", linha_inicio, coluna_inicio);
                return criarToken(TOKEN_ERRO, lexema, linha_inicio, coluna_inicio);
            }
        }

        if (c == '<') {
            if (preverCaractere() == '>') { proximoCaractere(); return criarToken(TOKEN_OP_NE, "<>", linha_inicio, coluna_inicio); }
            if (preverCaractere() == '=') { proximoCaractere(); return criarToken(TOKEN_OP_LE, "<=", linha_inicio, coluna_inicio); }
            return criarToken(TOKEN_OP_LT, "<", linha_inicio, coluna_inicio);
        }

        if (c == '>') {
            if (preverCaractere() == '=') { proximoCaractere(); return criarToken(TOKEN_OP_GE, ">=", linha_inicio, coluna_inicio); }
            return criarToken(TOKEN_OP_GT, ">", linha_inicio, coluna_inicio);
        }

        if (c == '=') return criarToken(TOKEN_OP_EQ, "=", linha_inicio, coluna_inicio);

        switch(c) {
            case '+': return criarToken(TOKEN_OP_AD, "+", linha_inicio, coluna_inicio);
            case '-': return criarToken(TOKEN_OP_MIN, "-", linha_inicio, coluna_inicio);
            case '*': return criarToken(TOKEN_OP_MUL, "*", linha_inicio, coluna_inicio);
            case '/': return criarToken(TOKEN_OP_DIV, "/", linha_inicio, coluna_inicio);
            case ',': return criarToken(TOKEN_SMB_COM, ",", linha_inicio, coluna_inicio);
            case ';': return criarToken(TOKEN_SMB_SEM, ";", linha_inicio, coluna_inicio);
            case ')': return criarToken(TOKEN_SMB_CPA, ")", linha_inicio, coluna_inicio);
            case '^': return criarToken(TOKEN_OP_POW, "^", linha_inicio, coluna_inicio);
            case '.': return criarToken(TOKEN_SMB_DOT, ".", linha_inicio, coluna_inicio);
            
            case ':':
                if (preverCaractere() == '=') {
                    proximoCaractere();
                    return criarToken(TOKEN_OP_ASS, ":=", linha_inicio, coluna_inicio);
                } else {
                    return criarToken(TOKEN_SMB_COLON, ":", linha_inicio, coluna_inicio);
                }

            case '(': {
                long pos_inicial = ftell(arquivo_fonte);
                int linha_bkp = linha_atual;
                int coluna_bkp = coluna_atual;

                char buffer_lexema[256];
                int buffer_idx = 0;
                buffer_lexema[buffer_idx++] = '(';

                pularEspacos();
                if (!isalnum(preverCaractere())) goto falha_binomial;
                while (isalnum(preverCaractere())) { buffer_lexema[buffer_idx++] = proximoCaractere(); }
                
                pularEspacos();
                char op = preverCaractere();
                if (op != '+' && op != '-') goto falha_binomial;
                buffer_lexema[buffer_idx++] = proximoCaractere();

                pularEspacos();
                if (!isalnum(preverCaractere()) && preverCaractere() != '.') goto falha_binomial;
                while (isalnum(preverCaractere()) || preverCaractere() == '.') { 
                    buffer_lexema[buffer_idx++] = proximoCaractere(); 
                }

                pularEspacos();
                if (preverCaractere() != ')') goto falha_binomial;
                buffer_lexema[buffer_idx++] = proximoCaractere();

                pularEspacos();
                if (preverCaractere() != '^') goto falha_binomial;
                buffer_lexema[buffer_idx++] = proximoCaractere();

                pularEspacos();
                if (!isdigit(preverCaractere())) goto falha_binomial;
                while (isdigit(preverCaractere())) { buffer_lexema[buffer_idx++] = proximoCaractere(); }

                buffer_lexema[buffer_idx] = '\0';
                return criarToken(TOKEN_EXP_BINOMIAL, buffer_lexema, linha_inicio, coluna_inicio);

            falha_binomial:
                fseek(arquivo_fonte, pos_inicial, SEEK_SET);
                linha_atual = linha_bkp;
                coluna_atual = coluna_bkp;
                return criarToken(TOKEN_SMB_OPA, "(", linha_inicio, coluna_inicio);
            }
            
            default:
                if (isprint(c)) {
                    sprintf(lexema, "%c", c);
                    fprintf(stderr, "ERRO LÉXICO: Caractere desconhecido '%c' na linha %d, coluna %d.\n", c, linha_inicio, coluna_inicio);
                } else {
                    sprintf(lexema, "\\x%02X", (unsigned char)c);
                    fprintf(stderr, "ERRO LÉXICO: Caractere não imprimível (código %d) na linha %d, coluna %d.\n", (unsigned char)c, linha_inicio, coluna_inicio);
                }
                return criarToken(TOKEN_ERRO, lexema, linha_inicio, coluna_inicio);
        }
    }
}

// --- FUNÇÃO MAIN --- 

/*
Fluxo principal do analisador léxico:

1. Abre o arquivo fonte passado por parâmetro. Se não conseguir abrir, exibe mensagem de erro e encerra.

2. Gera o nome do arquivo de saída, trocando a extensão para ".lex".

3. Abre o arquivo de saída e escreve o cabeçalho da tabela de tokens.

4. Inicializa a tabela de símbolos com as palavras-chave da linguagem.

5. Executa o loop principal do scanner:
   - Para cada token lido, se for erro, marca que houve erro.
   - Se for um token válido (exceto EOF), grava no arquivo de saída o lexema, tipo, linha e coluna.

6. Fecha os arquivos de entrada e saída.

7. Imprime a tabela de símbolos final no console.

8. Exibe mensagem de status informando se a análise léxica teve sucesso ou encontrou erros, e o nome do arquivo gerado.

*/

int main(int argc, char* argv[]) {
    // Adiciona o setlocale para corrigir a exibição de acentos no console Windows
    setlocale(LC_ALL, ".UTF-8");

    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte.pas>\n", argv[0]);
        return 1;
    }

    arquivo_fonte = fopen(argv[1], "r");
    if (!arquivo_fonte) {
        perror("Erro ao abrir arquivo fonte");
        return 1;
    }

    char nome_arquivo_saida[100];
    strcpy(nome_arquivo_saida, argv[1]);
    char *ponto = strrchr(nome_arquivo_saida, '.');
    if (ponto != NULL && ponto != nome_arquivo_saida) {
        strcpy(ponto, ".lex");
    } else {
        strcat(nome_arquivo_saida, ".lex");
    }

    FILE* arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (!arquivo_saida) {
        perror("Erro ao criar arquivo de saida");
        fclose(arquivo_fonte);
        return 1;
    }

    fprintf(arquivo_saida, "%-20s | %-17s | %-5s | %s\n", "LEXEMA", "TIPO DE TOKEN", "LINHA", "COLUNA");
    fprintf(arquivo_saida, "------------------------------------------------------------------\n");

    inicializarTabelaDeSimbolos();
    printf("Iniciando análise léxica de '%s'...\n", argv[1]);
    bool contem_erros = false;
    Token token;
    
    do {
        token = obterProximoToken();
        if (token.tipo == TOKEN_ERRO) {
            contem_erros = true;
        } else if (token.tipo != TOKEN_FIM_DE_ARQUIVO) { // O laço para QUANDO recebe o token de EOF.
            fprintf(arquivo_saida, "%-20s | %-17s | %-5d | %d\n", 
                    token.lexema, 
                    tipo_token_para_string[token.tipo], 
                    token.linha, 
                    token.coluna);
        }
    } while (token.tipo != TOKEN_FIM_DE_ARQUIVO);

    fclose(arquivo_fonte);
    fclose(arquivo_saida);

    // --- ORDEM DE EXIBIÇÃO LEXEMA | TOKEN | LINHA | COLUNA --- 

    // 1. Imprime a Tabela de Símbolos PRIMEIRO
    imprimirTabelaDeSimbolos();

    // 2. Imprime a mensagem de status DEPOIS
    printf("\n======================================================\n");
    if (contem_erros) {
        printf("Análise léxica concluída com ERROS.\n");
        printf("O arquivo '%s' contém os tokens válidos formatados em tabela.\n", nome_arquivo_saida);
    } else {
        printf("Análise léxica concluída com SUCESSO!\n");
        printf("Todos os tokens foram reconhecidos e salvos em '%s'.\n", nome_arquivo_saida);
    }
    printf("======================================================\n");

    return 0;
}
