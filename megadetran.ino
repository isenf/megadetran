#include <VarSpeedServo.h>

#define QTDE_LDR 4
#define QTDE_SERVOS 4
#define VALOR_LDR 200
#define VELOC_PADRAO 30

#define PIN_CABECA 2
#define PIN_ASA_ESQ 3
#define PIN_ASA_DIR 4
#define PIN_TRONCO 5

// ordem: cabeça, asa_esq, asa_dir, tronco
const int ldrs_pin[] = {A0, A1, A2, A3};

// false = não detectou laser; true = detectou
bool detectou[QTDE_LDR] = {false, false, false, false};

int valores_ldr[QTDE_LDR];

struct ServoConfig{
    VarSpeedServo servo;
    int pin;
    int ang_min;
    int ang_max;
    int ang_inicial;
    int ang_atual;
    int velocidade;
};

ServoConfig cabeca, asa_esq, asa_dir, tronco;

void setupServo(ServoConfig &servoConfig, int pin, int ang_min, int ang_max, 
                int ang_inicial, int velocidade = VELOC_PADRAO){
    // função criada para configurar os servos e inicializar

    servoConfig.pin = pin;
    servoConfig.ang_min = ang_min;
    servoConfig.ang_max = ang_max;
    servoConfig.ang_inicial = ang_inicial;
    servoConfig.ang_atual = ang_inicial;
    servoConfig.velocidade = velocidade;

    servoConfig.servo.attach(pin);
    servoConfig.servo.write(ang_inicial, velocidade);

}

void setup(){

    // configura os LDRs
    for(int i = 0; i <  QTDE_LDR; i++){
        pinMode(ldrs_pin[0], INPUT);
    }

    // configura os servos
    setupServo(cabeca, PIN_CABECA, 0, 90, 45, 25);
    setupServo(asa_esq, PIN_ASA_ESQ, 0, 45, 0);
    setupServo(asa_dir, PIN_ASA_DIR, 0, 45, 0);
    setupServo(tronco, PIN_TRONCO, 0, 180, 90, 15);

    Serial.begin(9600);
}

void loop(){

    lerLdr();
    imprimeLdr();
    verificaLdr();

   

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
