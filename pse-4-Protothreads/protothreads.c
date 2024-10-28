#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pt.h"

#define STX 0x02
#define ETX 0x03
#define TAMANHO_BUFFER 256
#define MAX_ITERACOES 1

typedef struct {
    char buffer[TAMANHO_BUFFER];
    int inicio;
    int fim;
    int cheio;
} BufferCircular;

void inicializar(BufferCircular* buf) {
    buf->inicio = 0;
    buf->fim = 0;
    buf->cheio = 0;
}

int estaVazio(BufferCircular* buf) {
    return (!buf->cheio && (buf->inicio == buf->fim));
}

int estaCheio(BufferCircular* buf) {
    return buf->cheio;
}

void inserir(BufferCircular* buf, char valor) {
    if (estaCheio(buf)) {
        buf->inicio = (buf->inicio + 1) % TAMANHO_BUFFER;
    }
    buf->buffer[buf->fim] = valor;
    buf->fim = (buf->fim + 1) % TAMANHO_BUFFER;
    buf->cheio = (buf->fim == buf->inicio);
}

char remover(BufferCircular* buf) {
    assert(!estaVazio(buf));

    char valor = buf->buffer[buf->inicio];
    buf->inicio = (buf->inicio + 1) % TAMANHO_BUFFER;
    buf->cheio = 0;

    return valor;
}

unsigned char calcularChecksum(char* dados, int tamanho) {
    unsigned char chk = 0;
    for (int i = 0; i < tamanho; i++) {
        chk ^= dados[i];
    }
    return chk;
}

int codificar(char* destino, char* dados, int tamanho) {
    destino[0] = STX;
    destino[1] = tamanho;
    memcpy(&destino[2], dados, tamanho);
    destino[2 + tamanho] = calcularChecksum(dados, tamanho);
    destino[3 + tamanho] = ETX;
    return 4 + tamanho;
}

int decodificar(char* dados, int tamanho, char* destino) {
    if (dados[0] != STX || dados[tamanho - 1] != ETX) {
        return -1;  // erro formato
    }
    int qtd = dados[1];
    if (qtd + 4 != tamanho) {
        return -1;  // erro tamanho
    }
    if (dados[2 + qtd] != calcularChecksum(&dados[2], qtd)) {
        return -1;  // erro checksum
    }
    memcpy(destino, &dados[2], qtd);
    return qtd;
}

void testarProtocolo() {
    // Dados de teste
    char dadosOriginais[] = "ABCD";
    char bufferCodificado[256];
    char bufferDecodificado[256];

    // Teste de codificação
    int tamanhoCodificado = codificar(bufferCodificado, dadosOriginais, sizeof(dadosOriginais) - 1);
    assert(tamanhoCodificado == 8);
    assert(bufferCodificado[0] == STX);
    assert(bufferCodificado[1] == 4);
    assert(bufferCodificado[6] == calcularChecksum(dadosOriginais, 4));
    assert(bufferCodificado[7] == ETX);

    // Teste de decodificação
    int tamanhoDecodificado = decodificar(bufferCodificado, tamanhoCodificado, bufferDecodificado);
    assert(tamanhoDecodificado == 4);
    assert(memcmp(bufferDecodificado, dadosOriginais, 4) == 0);

    printf("Todos os testes do protocolo passaram!\n");
}

static struct pt pt_transmissora, pt_receptora;
BufferCircular bufferTransmissora, bufferReceptora;

static PT_THREAD(thread_transmissora(struct pt* pt)) {
    PT_BEGIN(pt);

    static char dados[] = "ABCD";
    static char bufferCodificado[256];
    static int tamanhoCodificado;

    while(1) {
        tamanhoCodificado = codificar(bufferCodificado, dados, sizeof(dados) - 1);
        for (int i = 0; i < tamanhoCodificado; i++) {
            inserir(&bufferTransmissora, bufferCodificado[i]);
        }
        PT_YIELD(pt);
    }

    PT_END(pt);
}

static PT_THREAD(thread_receptora(struct pt* pt)) {
    PT_BEGIN(pt);

    static char bufferDecodificado[256];
    static char bufferRecebido[256];
    static int tamanhoRecebido;
    static int tamanhoDecodificado;

    while(1) {
        tamanhoRecebido = 0;
        while (!estaVazio(&bufferTransmissora)) {
            bufferRecebido[tamanhoRecebido++] = remover(&bufferTransmissora);
            if (bufferRecebido[tamanhoRecebido - 1] == ETX) {
                break;
            }
        }

        tamanhoDecodificado = decodificar(bufferRecebido, tamanhoRecebido, bufferDecodificado);
        if (tamanhoDecodificado > 0) {
            printf("Dados recebidos corretamente: %s\n", bufferDecodificado);
            inserir(&bufferReceptora, 'A'); // ACK
        } else {
            printf("Erro na recepção dos dados\n");
        }
        PT_YIELD(pt);
    }

    PT_END(pt);
}

int main() {
    PT_INIT(&pt_transmissora);
    PT_INIT(&pt_receptora);
    inicializar(&bufferTransmissora);
    inicializar(&bufferReceptora);

    testarProtocolo();

    int iteracoes = 0;

    while (iteracoes < MAX_ITERACOES) {
        thread_transmissora(&pt_transmissora);
        thread_receptora(&pt_receptora);
        iteracoes++;
    }

    printf("Simulação finalizada após %d iterações.\n", MAX_ITERACOES);

    return 0;
}
