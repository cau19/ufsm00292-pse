#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define STX 0x02
#define ETX 0x03

// estados
#define ESP 0
#define QTD 1
#define DADOS 2
#define CHK 3
#define ETX_ESPERANDO 4
#define COMPLETO 5
#define ERRO 6

// estrutura FSM
typedef struct {
    uint8_t estado;
    uint8_t qtd;
    uint8_t dados[256];
    uint8_t chk;
    uint8_t idx;
} FSM;

// inicializa FSM
void fsm_init(FSM *fsm) {
    fsm->estado = ESP;
    fsm->qtd = 0;
    fsm->chk = 0;
    fsm->idx = 0;
}

// processa byte
bool fsm_process(FSM *fsm, uint8_t byte) {
    switch (fsm->estado) {
        case ESP:
            if (byte == STX) {
                fsm->estado = QTD;
            }
            break;

        case QTD:
            fsm->qtd = byte;
            fsm->estado = DADOS;
            break;

        case DADOS:
            fsm->dados[fsm->idx++] = byte;
            if (fsm->idx == fsm->qtd) {
                fsm->estado = CHK;
            }
            break;

        case CHK:
            fsm->chk = byte;
            fsm->estado = ETX_ESPERANDO;
            break;

        case ETX_ESPERANDO:
            if (byte == ETX) {
                fsm->estado = COMPLETO;
                return true;
            } else {
                fsm->estado = ERRO;
            }
            break;

        case COMPLETO:
        case ERRO:
            // Manter os estados finais não processáveis
            break;
    }
    return false;
}

// testar FSM
bool testepassou() {
    FSM fsm;
    fsm_init(&fsm);

    uint8_t msg[] = {STX, 0x03, 'A', 'B', 'C', 0x05, ETX};

    for (int i = 0; i < sizeof(msg); i++) {
        if (fsm_process(&fsm, msg[i]) && (i != sizeof(msg) - 1)) {
            return false; // Não deve completar antes do final da mensagem
        }
    }

    return fsm.estado == COMPLETO;
}

int main() {
    bool todos_passaram = true;

    // teste
    if (!testepassou()) {
        todos_passaram = false;
    }

    // Imprimir resultado dos testes
    if (todos_passaram) {
        printf("Todos os testes passaram.\n");
    } else {
        printf("Um ou mais testes falharam.\n");
    }

    return 0;
}
