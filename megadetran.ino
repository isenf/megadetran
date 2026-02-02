#include <Servo.h>

#define QTDE_LDR 4
#define QTDE_SERVOS 4

#define VALOR_LDR 200

// referencial dos arrays
#define CABECA 0
#define ASA_ESQ 1
#define ASA_DIR 2
#define TRONCO 3

// ordem: cabeça, asa_esq, asa_dir, tronco
const int ldrs_pin[] = {A0, A1, A2, A3};
const int servos_pin[] = {2, 3, 4, 5};

int pos_iniciais[] = {45, 0, 0, 90}; // pode ser mudado depois
int valores_ldr[QTDE_LDR];

Servo servos[QTDE_SERVOS];

// false = não detectou laser; true = detectou
bool detectou[QTDE_LDR] = {false, false, false, false};

void setup(){

    for(int i = 0; i <  QTDE_LDR; i++){
        pinMode(ldrs_pin[0], INPUT);
    }

    
    for(int i = 0; i <  QTDE_SERVOS; i++){
        servos[i].attach(servos_pin[i]);
        servos[i].write(pos_iniciais[i]);
    }

    Serial.begin(9600);
  

}

void loop(){

    lerLdr();
    imprimeLdr();
    verificaLdr();

    for(int i = 0; i < QTDE_SERVOS; i++){ // será necessário modificar no futuro
      if(detectou[i]){

      }
    }

}

void lerLdr(){
    for(int i = 0; i < QTDE_LDR; i++){
        valores_ldr[i] = analogRead(ldrs_pin[i]);
    }
}

void verificaLdr(){
    for(int i = 0; i < QTDE_LDR; i++){
        if(valores_ldr[i] <= VALOR_LDR)
            detectou[i] = true;
    }
}

void imprimeLdr(){
    for(int i = 0; i < QTDE_LDR; i++){
        Serial.print("LDR");
        Serial.print(i);
        Serial.print(":");
        Serial.println(valores_ldr[i]);
    }

    Serial.println();
}

void movimentaServo(int servo_index, int angulo){
    if(servo_index < 0 || servo_index >= QTDE_SERVOS){
        Serial.println("erro: indice inválido");
    } 
     
    int angulo = constraint(angulo, 0, 180);
    servos[servo_index].write(angulo);

}