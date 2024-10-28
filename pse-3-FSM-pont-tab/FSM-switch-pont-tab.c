#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STX 0x02
#define ETX 0x03

// estados
#define AGUARDAR_STX    0
#define LER_QTD         1
#define LER_DADOS       2
#define LER_CHK         3
#define AGUARDAR_ETX    4
#define COMPLETO        5
#define ERRO            6

// estrutura FSM
typedef struct {
    uint8_t state;
    uint8_t qtd;
    uint8_t data[256];
    uint8_t chk;
    uint8_t dataIndex;
    bool (*stateFunc)(struct FSM*, uint8_t);
} FSM;

// funções
bool fsm_aguardar_stx(FSM *fsm, uint8_t byte);
bool fsm_ler_qtd(FSM *fsm, uint8_t byte);
bool fsm_ler_dados(FSM *fsm, uint8_t byte);
bool fsm_ler_chk(FSM *fsm, uint8_t byte);
bool fsm_aguardar_etx(FSM *fsm, uint8_t byte);

// tab estados
bool (*tabelaEstados[])(FSM*, uint8_t) = {
    fsm_aguardar_stx,
    fsm_ler_qtd,
    fsm_ler_dados,
    fsm_ler_chk,
    fsm_aguardar_etx
};

// inicializa FSM
void fsm_inicializar(FSM *fsm) {
    fsm->state = AGUARDAR_STX;
    fsm->qtd = 0;
    fsm->chk = 0;
    fsm->dataIndex = 0;
    fsm->stateFunc = tabelaEstados[fsm->state];
}

// processa byte
bool fsm_processar(FSM *fsm, uint8_t byte) {
    if (fsm->state < sizeof(tabelaEstados) / sizeof(tabelaEstados[0])) {
        bool completou = fsm->stateFunc(fsm, byte);
        if (fsm->state < sizeof(tabelaEstados) / sizeof(tabelaEstados[0])) {
            fsm->stateFunc = tabelaEstados[fsm->state];
        }
        return completou;
    }
    return false;
}

// funções estado

bool fsm_aguardar_stx(FSM *fsm, uint8_t byte) {
    if (byte == STX) {
        fsm->state = LER_QTD;
    }
    return false;
}

bool fsm_ler_qtd(FSM *fsm, uint8_t byte) {
    fsm->qtd = byte;
    fsm->dataIndex = 0;
    fsm->state = LER_DADOS;
    return false;
}

bool fsm_ler_dados(FSM *fsm, uint8_t byte) {
    if (fsm->dataIndex < fsm->qtd) {
        fsm->data[fsm->dataIndex++] = byte;
        if (fsm->dataIndex == fsm->qtd) {
            fsm->state = LER_CHK;
        }
    }
    return false;
}

bool fsm_ler_chk(FSM *fsm, uint8_t byte) {
    fsm->chk = byte;
    fsm->state = AGUARDAR_ETX;
    return false;
}

bool fsm_aguardar_etx(FSM *fsm, uint8_t byte) {
    if (byte == ETX) {
        fsm->state = COMPLETO;
        return true;
    } else {
        fsm->state = ERRO;
    }
    return false;
}

// testes FSM
void executar_teste() {
    FSM fsm;
    fsm_inicializar(&fsm);

    uint8_t mensagem[] = {STX, 0x03, 'A', 'B', 'C', 0x05, ETX};
    bool completou = false;

    for (int i = 0; i < sizeof(mensagem); i++) {
        if (fsm_processar(&fsm, mensagem[i])) {
            completou = true;
            break;
        }
    }

    if (completou && fsm.state == COMPLETO) {
        printf("FSM completou com sucesso.\n");
    } else {
        printf("FSM falhou.\n");
    }
}

int main() {
    executar_teste();
    return 0;
}
