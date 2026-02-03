#include <VarSpeedServo.h>

#define QTDE_LDR 4
#define QTDE_SERVOS 4
#define VALOR_LDR 200
#define VELOC_PADRAO 30
#define VELOC_TRAV 80

#define PIN_CABECA 2
#define PIN_ASA_ESQ 3
#define PIN_ASA_DIR 4
#define PIN_TRONCO 5

// ordem: cabeça, asa_esq, asa_dir, tronco
const int ldrs_pin[] = {A0, A1, A2, A3};

// false = não detectou laser; true = detectou
bool detectou[QTDE_LDR] = {false, false, false, false};
bool travado[QTDE_SERVOS] = {false, false, false, false};

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

    delay(300);
}

void setup(){

    // configura os LDRs
    for(int i = 0; i <  QTDE_LDR; i++){
        pinMode(ldrs_pin[i], INPUT);
    }

    // configura os servos
    setupServo(cabeca, PIN_CABECA, 0, 90, 0, 35);
    setupServo(asa_esq, PIN_ASA_ESQ, 0, 45, 0, 30);
    setupServo(asa_dir, PIN_ASA_DIR, 0, 45, 0, 30);
    setupServo(tronco, PIN_TRONCO, 0, 90, 0, 35);

    Serial.begin(9600);
}

void loop(){

    lerLdr();
    imprimeLdr(); //para debug
    verificaLdr();

    for(int i = 0; i < QTDE_LDR; i ++){
        if(detectou[i]){
            travarServo(i);
            detectou[i] = false;    // modifica somente para não ficar executando travarServo todo loop
        }
    }



    if(!travado[0]) moverServo(cabeca);
    if(!travado[1]) moverServo(asa_esq);
    if(!travado[2]) moverServo(asa_dir);
    if(!travado[3]) moverServo(tronco);   

    delay(1000);
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

void moverServo(ServoConfig &servo){

    int incremento = 5;

    if(servo.ang_atual >= servo.ang_max){
        //servo.ang_atual = servo.ang_max - 1;
        servo.servo.write(servo.ang_min, servo.velocidade, false);
        servo.ang_atual = servo.ang_min;

    } else if(servo.ang_atual <= servo.ang_min){
        //servo.ang_atual = servo.ang_min + 1;
        servo.servo.write(servo.ang_max, servo.velocidade, false);
        servo.ang_atual = servo.ang_max;

    } else{
        int prox_ang;

        if(servo.ang_atual - servo.ang_min < servo.ang_max - servo.ang_atual){
            prox_ang = servo.ang_atual + incremento;

        } else{
            prox_ang = servo.ang_atual - incremento;
        }

        servo.servo.write(prox_ang, servo.velocidade, false);
        servo.ang_atual = prox_ang;

    }
}

void travarServo(int index){
    
    if(index >= 0 && index < QTDE_SERVOS && !travado[index]){
        travado[index] = true;

        switch(index){
            case 0:
                cabeca.servo.write(cabeca.ang_inicial, VELOC_TRAV, false);
                cabeca.ang_atual = cabeca.ang_inicial;
                break;

            case 1:
                asa_esq.servo.write(asa_esq.ang_inicial, VELOC_TRAV, false);
                asa_esq.ang_atual = asa_esq.ang_inicial;
                break;

            case 2:
                asa_dir.servo.write(asa_dir.ang_inicial, VELOC_TRAV, false);
                asa_dir.ang_atual = asa_dir.ang_inicial;
                break;

            case 3:
                tronco.servo.write(tronco.ang_inicial, VELOC_TRAV, false);
                tronco.ang_atual = tronco.ang_inicial;
                break;
        }


    }

}
