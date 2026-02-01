#include <Servo.h>

#define QTDE_LDR 4
#define QTDE_SERVOS 4

// referencial dos arrays
#define CABECA 0
#define ASA_ESQ 1
#define ASA_DIR 2
#define TRONCO 3

// ordem: cabeça, asa_esq, asa_dir, tronco
const int ldrs_pin[] = {A0, A1, A2, A3};
const int servos_pin[] = {2, 3, 4, 5};
int pos_iniciais[] = {45, 0, 0, 90}; // pode ser mudado depois

Servo servos[QTDE_SERVOS];

void setup(){

  for(int i = 0; i <  QTDE_LDR; i++){
    pinMode(ldrs_pin[0], INPUT);
  }

  for(int i = 0; i <  QTDE_SERVOS; i++){
    servos[i].attach(servos_pin[i]);
    servos[i].write(pos_iniciais[i]);
  }

}

void loop(){

}
