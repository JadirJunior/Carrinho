//Bibliotecas utilizadas
#include <DabbleESP32.h>

//Definições de biblioteca
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE

//PARÂMETROS
//Parâmetro para definir a distância do sensor frontal 
#define P_SENSOR_FRONTAL 20

//Parâmetro para definir a distância do sensor traseiro
#define P_SENSOR_TRASEIRO 20

//Parâmetro para definir o tempo de ré (segundos)
#define P_T_RE 1

//Parâmetro para definir o tempo que o carrinho irá girar após a ré (segundos)
#define P_T_ROTA_1 1

//Parâmetro para definir o tempo que o carrinho irá girar para ajustar a lateral (segundos)
#define P_T_ROTA_2 1

//Parâmetro para definir o percentual de velocidade da ré
#define P_V_RE 90

//Parâmetro para definir o percentual de velocidade do ajuste
#define P_V_AJUSTE 90

//Parâmetro para definir a velocidade frontal máxima
#define P_V_FRENTE 100

//Parâmetro para definir a velocidade frontal mínima
#define P_V_MIN 90

/////////////////////////////////////////////////////

//PINAGEM
#define SF_TRIG 15
#define SF_ECHO 4
#define ST_TRIG 22
#define ST_ECHO 25
 
#define MTD 32
#define RD 27
#define MTE 14  
#define RE 13
#define VE 18
#define VD 19

#define SLD 2
#define SLE 21

#define MC 35

#define ACR 1

/////////////////////////////////////////////////
//Definição dos estados e partes do processamento
enum Estado {
  desligado = 0,
  manual = 1,
  automatico = 2
};

enum Automatico {
  frente = 0,
  ajuste_esq = 1,
  ajuste_dir = 2,
  re = 3,
  ajuste_re = 4,
  paradoA = 5
};

enum Manual {
  frente_m = 0,
  re_m = 1,
  esquerda = 2,
  direita = 3,
  paradoM = 4
};

//VARIÁVEIS
int R = 0;
int t_re = 0;
int sle = -1;
int sld = -1;
int mc = -1;
int frequencia = 5000;
int canal = 0;
int resolucao = 8;
int velocidade = 0;

Estado estado;
Automatico modoA;
Manual modoM;


//MÉTODOS
bool verificaFrente() {
  digitalWrite(SF_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(SF_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SF_TRIG, LOW);

  float duration = pulseIn(SF_ECHO, HIGH);

  float distance = 0.017 * duration;
  
  if (distance < P_SENSOR_FRONTAL) {

    return true;

  } else {
    return false;
  }

}

void reset() {
  t_re = 0;
}

bool verificaTras() {
  digitalWrite(ST_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(ST_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ST_TRIG, LOW);

  float duration = pulseIn(ST_ECHO, HIGH);

  float distance = 0.017 * duration;
  
  if (distance < P_SENSOR_TRASEIRO) {

    return true;

  } else {
    return false;
  }

}


int calcularVelocidade(float percentual) {
  float val = percentual/100;
  int ret = val*255;
  return ret;
}

void parar() {
  
  velocidade = 0;
  digitalWrite(MTD, LOW);
  digitalWrite(MTE, LOW);
  digitalWrite(RD, LOW);
  digitalWrite(RE, LOW);
  ledcWrite(canal, calcularVelocidade(0));
  ledcWrite(canal, calcularVelocidade(0));
}


//////////////////////////////////////////////////////////
//Métodos dos estados e processamento:
void ParadoManual() {
  parar();

  if (GamePad.isTrianglePressed())
  {
    modoM = frente_m;
  } else if (GamePad.isCrossPressed()) {
    modoM = re_m;
  } else if (GamePad.isSquarePressed()) {
    modoM = esquerda;
  } else if (GamePad.isCirclePressed()) {
    modoM = direita;
  }

}

void ParadoAuto() {

  parar();



  if (R == 1) {
    modoA = re;
  } else {
    modoA = frente;
  }

}

void Frente() {

  if (verificaFrente()) {

      //Parar o motor por um tempinho e ativar a Ré
      parar();
      R = 1;
      delay(1000);
      modoA = paradoA;
      Serial.println("Parando e ativando a ré");
      
    } else {
      //Motor para frente

      if (digitalRead(SLD) == 1) {
        
        modoA = ajuste_esq;
        
      } else {

        if (digitalRead(SLE) == 1) {  
          modoA = ajuste_dir;
        } else {
          
          digitalWrite(RD, LOW);
          digitalWrite(RE, LOW);
          
          digitalWrite(MTE, HIGH);
          digitalWrite(MTD, HIGH);

          if (velocidade == 0) {
            velocidade = P_V_MIN;
          }

          velocidade += ACR;
          
          if (velocidade > P_V_FRENTE) {
            velocidade = P_V_FRENTE;
          }

          ledcWrite(canal, calcularVelocidade(velocidade));
        }

      }

      
      
      Serial.println("Frente");
    }

 

}


void Desligado() {

  switch (mc) {
    case 1:
      estado = automatico;
      break;
    case 0:
      estado = manual;
      break;
  }

}

void Ajuste_esq() {
  //Caso o sensor detecte algo na direita dele.
  
  parar();
  
  delay(800);

  digitalWrite(MTD, HIGH);
  digitalWrite(MTE, LOW);

  ledcWrite(canal, calcularVelocidade(P_V_AJUSTE));

  delay(P_T_ROTA_2*1000);

  modoA = frente;

}

void Ajuste_dir() {

  parar();
  
  delay(800);

  digitalWrite(MTD, LOW);
  digitalWrite(MTE, HIGH);

  ledcWrite(canal, calcularVelocidade(P_V_AJUSTE));

  delay(P_T_ROTA_2*1000);

  modoA = frente;

}

void Re() {

  digitalWrite(MTD, LOW);
  digitalWrite(RD, HIGH);
  

  digitalWrite(MTE, LOW);
  digitalWrite(RE, HIGH);

  ledcWrite(canal, calcularVelocidade(P_V_RE));

  if (verificaTras() || t_re > P_T_RE) {

      //Feito para caso o tempo da ré não finalize e algo surja atrás e não termine o movimento.
      //Nesse caso, é necessário garantir que ele inicie o ajuste.
      t_re = 0;
      Serial.println("Ajuste_Ré");

      modoA = ajuste_re;


    } else {

      t_re++;
      Serial.print("Contando tempo da ré: ");
      Serial.println(t_re);
      delay(1000);

    }



}

void Ajuste_re() {
  //Verifica SLD e SLE e dá preferência para a direita.

  
  int sle = digitalRead(SLE);
  int sld = digitalRead(SLD);
  
  if (sld && !sle) {
    //Ajustar para a esquerda

    parar();

    delay(500);

    digitalWrite(MTD, HIGH);
    digitalWrite(RD, LOW);
    
    digitalWrite(MTE, LOW);
    digitalWrite(RE, LOW);
    
    ledcWrite(canal, calcularVelocidade(P_V_AJUSTE));
    
    Serial.println("Ajustando a ré");

    delay(P_T_ROTA_1*1000);

    Serial.println("Carrinho ajustado");

    R = 0;
    
    modoA = paradoA;

  } else {

    
    if ( (sle && !sld) || (!sle && !sld)) {
      //Ajustar para a direita

        
      parar();

      delay(500);
      
      digitalWrite(MTD, LOW);
      digitalWrite(RD, LOW);
    
      digitalWrite(MTE, HIGH);
      digitalWrite(RE, LOW);

      
      ledcWrite(canal, calcularVelocidade(P_V_AJUSTE));

      Serial.println("Ajustando a ré");

      delay(P_T_ROTA_1*1000);

      Serial.println("Carrinho ajustado");

      R = 0;  
      modoA = paradoA;

    } else {
      
      reset();
      parar();

    }


  }

}

void modoAutomatico() {
  
  if (mc == 1) {

    switch (modoA) {
      case paradoA:
        ParadoAuto();
        break;
      case frente:
        Frente();
        break;
      case ajuste_esq:
        Ajuste_esq();
        break;
      case ajuste_dir:
        Ajuste_dir();
        break;
      case re:
        Re();
        break;
      case ajuste_re:
        Ajuste_re();
        break;
    }

  } else {
    parar();
    estado = manual;
  }
  
  

}

void Frente_M() {

  if (GamePad.isTrianglePressed())
  {

    if (velocidade == 0) {
      velocidade = P_V_MIN;
    }

    velocidade += ACR;
    if (velocidade > P_V_FRENTE) {
      velocidade = 100;
    }

    digitalWrite(RD, LOW);
    digitalWrite(RE, LOW);
    digitalWrite(MTD, HIGH);
    digitalWrite(MTE, HIGH);

    ledcWrite(canal, calcularVelocidade(velocidade));
    ledcWrite(canal, calcularVelocidade(velocidade));

  } else {
    modoM = paradoM;
  }
  
}

void Re_M() {
  
  if (GamePad.isCrossPressed())
  {
    digitalWrite(MTD, LOW);
    digitalWrite(RD, HIGH);
    digitalWrite(MTE, LOW);
    digitalWrite(RE, HIGH);
    ledcWrite(canal, calcularVelocidade(P_V_RE));


  } else {
    modoM = paradoM;
  }

}

void Direita() {
  if (GamePad.isCirclePressed())
  { 
    digitalWrite(MTD, LOW);
    digitalWrite(RD, LOW);
    digitalWrite(MTE, HIGH);
    digitalWrite(RE, LOW);
    ledcWrite(canal, calcularVelocidade(P_V_AJUSTE));

  } else {
    modoM = paradoM;
  }
}

void Esquerda() {
  if (GamePad.isSquarePressed())
  {
    digitalWrite(MTD, HIGH);
    digitalWrite(RD, LOW);
    digitalWrite(MTE, LOW);
    digitalWrite(RE, LOW);

    ledcWrite(canal, calcularVelocidade(P_V_AJUSTE));
  } else {
    modoM = paradoM;
  }
}


void modoManual() {

  if (mc == 0) {


    Dabble.processInput();

    switch (modoM) {
        case paradoM:
          ParadoManual();
          break;
        case frente_m:
          Frente_M();
          break;
        case re_m:
          Re_M();
          break;
        case direita:
          Direita();
          break;
        case esquerda:
          Esquerda();
          break;
    }

  } else {
    estado = automatico;
  }
  
  
}

///////////////////////////////////////////////


void setup() {

  Serial.begin(115200);

  pinMode(SF_TRIG, OUTPUT);
  pinMode(SF_ECHO, INPUT);

  pinMode(ST_TRIG, OUTPUT);
  pinMode(ST_ECHO, INPUT);


  pinMode(MTE, OUTPUT);
  pinMode(MTD, OUTPUT);
  pinMode(VE, OUTPUT);
  pinMode(VD, OUTPUT);
  ledcSetup(canal, frequencia, resolucao);  
  ledcAttachPin(VE, canal);  
  ledcAttachPin(VD, canal);
  
  
  pinMode(RE, OUTPUT);
  pinMode(RD, OUTPUT);
  pinMode(MC, INPUT_PULLDOWN);

 
  pinMode(SLD, INPUT_PULLDOWN);
  pinMode(SLE, INPUT_PULLDOWN);

  Dabble.begin("Seninha");

  
}


void loop() {
  mc = digitalRead(MC);

  
  switch (estado) {
    case desligado:
      Desligado();
      break;
    case manual:
      modoManual();
      break;
    case automatico:
      modoAutomatico();
      break;
  }
  
}
