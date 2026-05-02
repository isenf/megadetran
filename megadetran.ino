#include <VarSpeedServo.h>

// indices para as partes do dragão
#define CABECA 0
#define ASA_ESQ 1
#define ASA_DIR 2
#define TRONCO 3

// variáveis constantes
#define QTDE_LDR 4
#define QTDE_SERVOS 4
#define VALOR_LDR 200
#define VELOC_PADRAO 30
#define VELOC_TRAV 80
#define INTERV_PADRAO 200
#define INTERV_LDR 100 

// variáveis com os pinos
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

// variáveis para o brilho dos olhos
int val_leds = 255;
int count = 4;

// ordem: vermelho, verde, azul (boca)
const int led_rgb[] = {8, 9, 10};
int valores_rgb[] = {255, 0, 0};

// variáveis de fases e transições dos leds rgb
int fase_rgb = 0;
int passo_rgb = 0;
unsigned long tempo_ant_rgb = 0;

/*===== struct para os servos =====*/
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
    bool travado;
};

ServoConfig servos[4];

/*===== struct para os LDRs =====*/
struct LdrConfig{
    int pin;
    int valor;
    bool detectou;

    unsigned long tempo_anterior;
};

LdrConfig ldrs[4];

 // função criada para configurar os servos e inicializar
void setupServo(ServoConfig &servoConfig, int pin, int ang_min, int ang_max, 
                int ang_inicial, int velocidade = VELOC_PADRAO, int intervalo = INTERV_PADRAO){

    servoConfig.pin = pin;
    servoConfig.ang_min = ang_min;
    servoConfig.ang_max = ang_max;
    servoConfig.ang_inicial = ang_inicial;
    servoConfig.ang_atual = ang_inicial;
    servoConfig.velocidade = velocidade;

    servoConfig.tempo_anterior = 0;
    servoConfig.intervalo = intervalo;

    servoConfig.travado = false;

    servoConfig.servo.attach(pin);
}

// função criada para configurar e inicializar os LDRs
void setupLdr(LdrConfig &ldr, int pin){
    ldr.pin = pin;
    ldr.valor = 0;
    ldr.detectou = false;

    pinMode(pin, INPUT);
}

// configura a posição inicial dos servos
void posInicial(){
    servos[CABECA].servo.write(servos[CABECA].ang_inicial, servos[CABECA].velocidade, false);
    servos[ASA_ESQ].servo.write(servos[ASA_ESQ].ang_inicial, servos[ASA_ESQ].velocidade, false);
    servos[ASA_DIR].servo.write(servos[ASA_DIR].ang_inicial, servos[ASA_DIR].velocidade, false);
    servos[TRONCO].servo.write(servos[TRONCO].ang_inicial, servos[TRONCO].velocidade, false);
}

void setup(){

    // configura os LDRs
    for(int i = 0; i <  QTDE_LDR; i++){
        setupLdr(ldrs[i], ldrs_pin[i]);
    }

    // configura os leds RGBs
    for(int i = 0; i < 3; i++){
        pinMode(led_rgb[i], OUTPUT);
    }

    // configura os servos
    setupServo(servos[CABECA], PIN_CABECA, 0, 90, 0, 35, 1600);
    setupServo(servos[ASA_ESQ], PIN_ASA_ESQ, 0, 45, 0, 30, 800);
    setupServo(servos[ASA_DIR], PIN_ASA_DIR, 0, 45, 0, 30, 800);
    setupServo(servos[TRONCO], PIN_TRONCO, 0, 90, 0, 35, 1600);
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
    //imprimeLdr(); //para debug
    verificaLdr();

    for(int i = 0; i < QTDE_LDR; i ++){
        if(ldrs[i].detectou){
            if(!servos[i].travado){ 
                count--;
                acendeLeds(count);
            }
            
            travarServo(i);
            ldrs[i].detectou = false;    // modifica somente para não ficar executando travarServo todo loop
        }
    }

    if(!servos[CABECA].travado) moverServo(servos[CABECA]);
    if(!servos[ASA_ESQ].travado) moverServo(servos[ASA_ESQ]);
    if(!servos[ASA_DIR].travado) moverServo(servos[ASA_DIR]);
    if(!servos[TRONCO].travado) moverServo(servos[TRONCO]); 

    animaRgb();

}

// le os valores dos LDRs
void lerLdr(){

    if(millis() - tempo_anterior_ldr < INTERV_LDR) return;
    tempo_anterior_ldr = millis();

    for(int i = 0; i < QTDE_LDR; i++){
        ldrs[i].valor = analogRead(ldrs_pin[i]);
    }
}

void verificaLdr(){ // verifica se o LDR detectou

    for(int i = 0; i < QTDE_LDR; i++){
        if(ldrs[i].valor <= VALOR_LDR)
            ldrs[i].detectou = true;
    }
}

// função que imprime os valores dos LDRs
void imprimeLdr(){
    for(int i = 0; i < QTDE_LDR; i++){
        Serial.print("LDR");
        Serial.print(i);
        Serial.print(":");
        Serial.println(ldrs[i].valor);
    }

    Serial.println();
}

// movimenta os servos de forma assincrona
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

// trava um determinado servo motor
void travarServo(int index){
    
    if(index >= 0 && index < QTDE_SERVOS && !servos[index].travado){
        servos[index].travado = true;

        switch(index){
            case CABECA:
                servos[CABECA].servo.write(servos[CABECA].ang_inicial, VELOC_TRAV, false);
                servos[CABECA].ang_atual = servos[CABECA].ang_inicial;
                break;

            case ASA_ESQ:
                servos[ASA_ESQ].servo.write(servos[ASA_ESQ].ang_inicial, VELOC_TRAV, false);
                servos[ASA_ESQ].ang_atual = servos[ASA_ESQ].ang_inicial;
                break;

            case ASA_DIR:
                servos[ASA_DIR].servo.write(servos[ASA_DIR].ang_inicial, VELOC_TRAV, false);
                servos[ASA_DIR].ang_atual = servos[ASA_DIR].ang_inicial;
                break;

            case TRONCO:
                servos[TRONCO].servo.write(servos[TRONCO].ang_inicial, VELOC_TRAV, false);
                servos[TRONCO].ang_atual = servos[TRONCO].ang_inicial;
                break;
        }
    }
}

// acende o led dos olhos usando pwm
void acendeLeds(int count){
    val_leds = map(count, 0, 4, 0, 255);

    analogWrite(PIN_OLHOE, val_leds);
    analogWrite(PIN_OLHOD, val_leds);
}

// função para acender o led RGB
void acendeRgb(int verm, int verd, int azul){
    analogWrite(led_rgb[0], verm);
    analogWrite(led_rgb[1], verd);
    analogWrite(led_rgb[2], azul);

}

// faz o led RGB variar de vermelho -> laranja -> amarelo -> ...
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

// função que reseta as variáveis
void reset(){
    for(int i = 0; i < QTDE_LDR; i++){
        servos[i].travado = false;
    }

    posInicial();

    count = 4;
    acendeLeds(count);

    fase_rgb = 0;
    passo_rgb = 0;
    acendeRgb(255, 0, 0);

    delay(2000);    // delay para simulação
}
