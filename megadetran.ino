#include <VarSpeedServo.h>

#define CABECA 0
#define ASA_ESQ 1
#define ASA_DIR 2
#define TRONCO 3

#define QTDE_LDR 4
#define QTDE_SERVOS 4
#define VALOR_LDR 200
#define VELOC_PADRAO 30
#define VELOC_TRAV 80
#define INTERV_PADRAO 200
#define INTERV_LDR 100 

#define PIN_CABECA 2
#define PIN_ASA_ESQ 3
#define PIN_ASA_DIR 4
#define PIN_TRONCO 5
#define PIN_RESET 25
#define PIN_OLHOD 7
#define PIN_OLHOE 6

// ordem: cabeça, asa_esq, asa_dir, tronco
const int ldrs_pin[] = {A0, A1, A2, A3};
unsigned long tempo_anterior_ldr = 0;

// false = não detectou laser; true = detectou
bool detectou[QTDE_LDR] = {false, false, false, false};
bool travado[QTDE_SERVOS] = {false, false, false, false};

int valores_ldr[QTDE_LDR];
int val_leds = 255;
int count = 4;

// ordem: vermelho, verde, azul
const int led_rgb[] = {8, 9, 10};
int valores_rgb[] = {255, 0, 0};

int fase_rgb = 0;
int passo_rgb = 0;
unsigned long tempo_ant_rgb = 0;

struct ServoConfig{
    VarSpeedServo servo;
    int pin;
    int ang_min;
    int ang_max;
    int ang_inicial;
    int ang_atual;
    int velocidade;

    unsigned long tempo_anterior;
    unsigned long intervalo;
};

ServoConfig cabeca, asa_esq, asa_dir, tronco;

void setupServo(ServoConfig &servoConfig, int pin, int ang_min, int ang_max, 
                int ang_inicial, int velocidade = VELOC_PADRAO, int intervalo = INTERV_PADRAO){
    // função criada para configurar os servos e inicializar

    servoConfig.pin = pin;
    servoConfig.ang_min = ang_min;
    servoConfig.ang_max = ang_max;
    servoConfig.ang_inicial = ang_inicial;
    servoConfig.ang_atual = ang_inicial;
    servoConfig.velocidade = velocidade;

    servoConfig.tempo_anterior = 0;
    servoConfig.intervalo = intervalo;

    servoConfig.servo.attach(pin);
}

void posInicial(){
    cabeca.servo.write(cabeca.ang_inicial, cabeca.velocidade, false);
    asa_esq.servo.write(asa_esq.ang_inicial, asa_esq.velocidade, false);
    asa_dir.servo.write(asa_dir.ang_inicial, asa_dir.velocidade, false);
    tronco.servo.write(tronco.ang_inicial, tronco.velocidade, false);
}

void setup(){

    // configura os LDRs
    for(int i = 0; i <  QTDE_LDR; i++){
        pinMode(ldrs_pin[i], INPUT);
    }

    // configura os servos
    setupServo(cabeca, PIN_CABECA, 0, 90, 0, 35, 1600);
    setupServo(asa_esq, PIN_ASA_ESQ, 0, 45, 0, 30, 800);
    setupServo(asa_dir, PIN_ASA_DIR, 0, 45, 0, 30, 800);
    setupServo(tronco, PIN_TRONCO, 0, 90, 0, 35, 1600);
    posInicial();

    pinMode(PIN_RESET, INPUT_PULLUP);
    pinMode(PIN_OLHOE, OUTPUT);
    pinMode(PIN_OLHOD, OUTPUT);

    acendeLeds(count);

    Serial.begin(9600);
}

void loop(){

    if(digitalRead(PIN_RESET) == 0) reset();

    lerLdr();
    imprimeLdr(); //para debug
    verificaLdr();

    for(int i = 0; i < QTDE_LDR; i ++){
        if(detectou[i]){
            if(!travado[i]){ 
                count--;
                acendeLeds(count);
            }
            
            travarServo(i);
            detectou[i] = false;    // modifica somente para não ficar executando travarServo todo loop
        }
    }

    if(!travado[CABECA]) moverServo(cabeca);
    if(!travado[ASA_ESQ]) moverServo(asa_esq);
    if(!travado[ASA_DIR]) moverServo(asa_dir);
    if(!travado[TRONCO]) moverServo(tronco); 

    animaRgb();

}

void lerLdr(){

    if(millis() - tempo_anterior_ldr < INTERV_LDR) return;
    tempo_anterior_ldr = millis();

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

    if(millis() - servo.tempo_anterior < servo.intervalo) return;

    servo.tempo_anterior = millis();

    int incremento = 5;

    if(servo.ang_atual >= servo.ang_max){
        servo.servo.write(servo.ang_min, servo.velocidade, false);
        servo.ang_atual = servo.ang_min;

    } else if(servo.ang_atual <= servo.ang_min){
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
            case CABECA:
                cabeca.servo.write(cabeca.ang_inicial, VELOC_TRAV, false);
                cabeca.ang_atual = cabeca.ang_inicial;
                break;

            case ASA_ESQ:
                asa_esq.servo.write(asa_esq.ang_inicial, VELOC_TRAV, false);
                asa_esq.ang_atual = asa_esq.ang_inicial;
                break;

            case ASA_DIR:
                asa_dir.servo.write(asa_dir.ang_inicial, VELOC_TRAV, false);
                asa_dir.ang_atual = asa_dir.ang_inicial;
                break;

            case TRONCO:
                tronco.servo.write(tronco.ang_inicial, VELOC_TRAV, false);
                tronco.ang_atual = tronco.ang_inicial;
                break;
        }
    }
}

void acendeLeds(int count){
    val_leds = map(count, 0, 4, 0, 255);

    analogWrite(PIN_OLHOE, val_leds);
    analogWrite(PIN_OLHOD, val_leds);
}

void acendeRgb(int verm, int verd, int azul){
    analogWrite(led_rgb[0], verm);
    analogWrite(led_rgb[1], verd);
    analogWrite(led_rgb[2], azul);

}

void animaRgb(){

    if(count == 0){
        acendeRgb(0, 0, 0);
        return;
    }
    
    if(millis() - tempo_ant_rgb < 50) return;
    tempo_ant_rgb = millis();

    int verm = 255, verd = 0, azul = 0;

    switch(fase_rgb){

        case 0:
            passo_rgb += 5;

            if(passo_rgb >= 50){
                passo_rgb = 50;
                fase_rgb = 1;
            }

            verm = 255;
            verd = passo_rgb;
            break;
        
        case 1:

            passo_rgb += 5;

            if(passo_rgb >= 255){
                passo_rgb = 255;
                fase_rgb = 2;
            }

            verm = 255;
            verd = passo_rgb;
            break;

        case 2:
            passo_rgb -= 5;

            if(passo_rgb <= 50){
                passo_rgb = 50;
                fase_rgb = 3;
            }

            verm = 255;
            verd = passo_rgb;
            break;

        case 3:
            passo_rgb -= 5;

            if(passo_rgb  <= 0){
                passo_rgb = 0;
                fase_rgb = 0;
            }

            verm = 255;
            verd = passo_rgb;
            break;

    }

    acendeRgb(verm, verd, azul);

}


void reset(){
    for(int i = 0; i < QTDE_LDR; i++){
        travado[i] = false;
    }

    posInicial();

    count = 4;
    acendeLeds(count);

    fase_rgb = 0;
    passo_rgb = 0;
    acendeRgb(255, 0, 0);

    delay(2000);    // delay para simulação
}
