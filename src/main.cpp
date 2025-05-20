#include <Arduino.h> // Biblioteca principal do framework Arduino para ESP32
#include <WiFi.h> // Biblioteca para conectar o ESP32 a redes Wi-Fi
#include <FirebaseESP32.h> // Biblioteca para comunicação com Firebase Realtime Database
#include <addons/RTDBHelper.h> // Biblioteca auxiliar da FirebaseESP32 para debug/log


// #define WIFI_SSID "VIVOFIBRA-7ABE"
// #define WIFI_SSID "uaifai-brum" // Nome da rede Wi-Fi que será usada

#define WIFI_SSID "Cozinha"
#define WIFI_PASSWORD "feliciefred" // Senha da rede Wi-Fi

// #define WIFI_SSID "HIRATA" // Nome da rede Wi-Fi que será usada
// #define WIFI_PASSWORD "tonico123" // Senha da rede Wi-Fi

#define DATABASE_URL "https://test-75680-default-rtdb.firebaseio.com/" // URL do Realtime Database do Firebase
#define DATABASE_SECRET "SWc4MadC9VEjr2FqUA4nenvtXZDczXkD7Zs3Hq4e" // Token de autenticação legado do Firebase

// Pino conectado ao meio do potenciômetro
#define POT_PIN 34 // ou qualquer pino analógico disponível no seu ESP32

// Pino digital com suporte a PWM
#define BUZZER_PIN 33 

// Pino digital válido para o botão
#define BUTTON_PIN 32 

// Pinos conectados ao LED RGB
#define RED_PIN 25
#define GREEN_PIN 26
#define BLUE_PIN 27

#define TEMP_PIN 35

// Pino digital para o botão verde (adicionar vida)
#define BTN_ADD_PIN 16

// Pino digital para o botão vermelho (remover vida)
#define BTN_SUB_PIN 17

#define RED_HEALTH_PIN 0
#define YELLOW_HEALTH_PIN 2
#define GREEN1_HEALTH_PIN 21
#define GREEN2_HEALTH_PIN 22
#define GREEN3_HEALTH_PIN 23


FirebaseData fbdo; // Objeto para manipulação de dados com o Firebase
FirebaseAuth auth; // Estrutura usada para autenticação (não usada aqui pois estamos usando token legado)
FirebaseConfig config; // Estrutura usada para configuração do Firebase

bool controlModeEnabled = false; // false = modo local (sensores), true = modo remoto (valores do Firebase)


int potValue = 0; // valor compartilhado do potenciômetro
int bzzValue = 0;
bool btnValue = false;
int redValue = 0;   // valor da cor vermelha (0–255)
int greenValue = 0; // valor da cor verde (0–255)
int blueValue = 0;  // valor da cor azul (0–255)
int tmpValue = 0;



int valueToSend = 0; // valor a ser enviado para o Firebase


int healthPoints = 100; // valor da vida do personagem (0-100)
bool lastAddState = false;
bool lastSubState = false;


SemaphoreHandle_t frbMutex; // semáforo para controle de acesso ao firebase
SemaphoreHandle_t conMutex; // semáforo para controle de acesso ao controle_remoto
SemaphoreHandle_t potMutex; // semáforo para controle de acesso ao potenciômetro
SemaphoreHandle_t bzzMutex; // semáforo para controle de acesso ao buzzer
SemaphoreHandle_t btnMutex; // semáforo para controle de acesso ao botão
SemaphoreHandle_t rgbMutex; // semáforo para controle de acesso às cores RGB
SemaphoreHandle_t tmpMutex; // semáforo para controle de acesso à temperatura
SemaphoreHandle_t hpMutex;


// FUNCTIONS======================

// Task que faz a leitura do modo de operação no firebase constantemente
// void checkControlModeTask(void *parameter) {
//   bool mode;
//   bool lastMode;

//   while (true) {
//     if (Firebase.getBool(fbdo, "/control_mode")) {

//       lastMode = mode;
//       mode = fbdo.boolData();

//       if (xSemaphoreTake(conMutex, portMAX_DELAY)) {
//         controlModeEnabled = mode; // atualiza valor global
//         xSemaphoreGive(conMutex);
//       }

//       if (mode != lastMode){
//         Serial.printf("Modo de controle atualizado: %s\n", mode ? "REMOTO" : "LOCAL");
//       }
      
      
//     } else {
//         Serial.printf("Erro ao ler control_mode: %s\n", fbdo.errorReason().c_str());
//     }

//     vTaskDelay(2000 / portTICK_PERIOD_MS); // Checa a cada 2 segundos
//   }
// }

// void controlModeTask(void *parameter) {
//   FirebaseJson json;
//   bool mode;
//   bool lastMode;

//   while (true) {

//     // if (xSemaphoreTake(frbMutex, portMAX_DELAY)) {
//       Firebase.getInt(fbdo, "/sensor/botao");
//       lastMode = mode;
//       // mode = fbdo.boolData();
//       mode = false;

//       if (xSemaphoreTake(conMutex, portMAX_DELAY)) {
//         controlModeEnabled = mode;
//         xSemaphoreGive(conMutex);
//       }

//       if (mode != lastMode) {
//         Serial.printf("Modo de controle atualizado: %s\n", mode ? "REMOTO" : "LOCAL");
//       }

//       // // SE ESTIVER EM MODO REMOTO, ATUALIZA TODAS AS VARIÁVEIS GLOBAIS
//       // if (mode) {
//       //   int tempInt;
//       //   float tempFloat;

//       //   // Potenciômetro
//       //   if (Firebase.getInt(fbdo, "/sensor/potenciometro")) {
//       //     tempInt = fbdo.intData();
//       //     if (xSemaphoreTake(potMutex, portMAX_DELAY)) {
//       //       potValue = tempInt;
//       //       xSemaphoreGive(potMutex);
//       //     }
//       //   }

//       //   // Buzzer
//       //   if (Firebase.getInt(fbdo, "/sensor/buzzer")) {
//       //     tempInt = fbdo.intData();
//       //     if (xSemaphoreTake(bzzMutex, portMAX_DELAY)) {
//       //       bzzValue = tempInt;
//       //       xSemaphoreGive(bzzMutex);
//       //     }
//       //   }

//       //   // Botão
//       //   if (Firebase.getBool(fbdo, "/sensor/botao")) {
//       //     bool tempBool = fbdo.boolData();
//       //     if (xSemaphoreTake(btnMutex, portMAX_DELAY)) {
//       //       btnValue = tempBool;
//       //       xSemaphoreGive(btnMutex);
//       //     }
//       //   }

//       //   // LED RGB
//       //   if (Firebase.getInt(fbdo, "/sensor/led/red")) {
//       //     tempInt = fbdo.intData();
//       //     if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
//       //       redValue = tempInt;
//       //       xSemaphoreGive(rgbMutex);
//       //     }
//       //   }
//       //   if (Firebase.getInt(fbdo, "/sensor/led/green")) {
//       //     tempInt = fbdo.intData();
//       //     if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
//       //       greenValue = tempInt;
//       //       xSemaphoreGive(rgbMutex);
//       //     }
//       //   }
//       //   if (Firebase.getInt(fbdo, "/sensor/led/blue")) {
//       //     tempInt = fbdo.intData();
//       //     if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
//       //       blueValue = tempInt;
//       //       xSemaphoreGive(rgbMutex);
//       //     }
//       //   }

//       //   // Temperatura
//       //   if (Firebase.getFloat(fbdo, "/sensor/temperatura")) {
//       //     tempFloat = fbdo.floatData();
//       //     if (xSemaphoreTake(tmpMutex, portMAX_DELAY)) {
//       //       tmpValue = (int)tempFloat;
//       //       xSemaphoreGive(tmpMutex);
//       //     }
//       //   }
//       // }
//       // xSemaphoreGive(frbMutex);

//     // } else {
//     //   Serial.printf("Erro ao ler control_mode: %s\n", fbdo.errorReason().c_str());
//     // }

//     vTaskDelay(2000 / portTICK_PERIOD_MS); // A cada 2s
//   }
// }


void controlModeTask(void *parameter) {
  bool mode = false;
  bool lastMode = false;

  int localPot, localBzz, localBtn, localRed, localGreen, localBlue, localTmp;
  FirebaseJson json;
  FirebaseJsonData result;


  while (true) {
    // Ler o modo de controle
    if (Firebase.getBool(fbdo, "/control_mode")) {
      lastMode = mode;
      mode = fbdo.boolData();

      // atualizando flag de controle
      if (xSemaphoreTake(conMutex, portMAX_DELAY)) {
        controlModeEnabled = mode;
        xSemaphoreGive(conMutex);
      }

      Serial.printf("Modo de controle: %s\n", mode ? "REMOTO" : "LOCAL");

      if (mode != lastMode) {
        Serial.printf("Modo de controle atualizado: %s\n", mode ? "REMOTO" : "LOCAL");
      }
    } else {
      Serial.printf("Erro ao ler control_mode: %s\n", fbdo.errorReason().c_str());
    }



    if (!mode) {
      // MODO LOCAL: envia os valores para o Firebase

      // Lê o valor do potenciômetro com mutex
      if (xSemaphoreTake(potMutex, portMAX_DELAY)) {
        localPot = potValue;
        xSemaphoreGive(potMutex);
      }

      // Lê o valor do buzzer com mutex
      if (xSemaphoreTake(bzzMutex, portMAX_DELAY)) {
        localBzz = bzzValue;
        xSemaphoreGive(bzzMutex);
      }

      // Lê o valor do botão com mutex
      if (xSemaphoreTake(btnMutex, portMAX_DELAY)) {
        localBtn = btnValue;
        xSemaphoreGive(btnMutex);
      }

      // Lê os valores RGB com mutex
      if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
        localRed = redValue;
        localGreen = greenValue;
        localBlue = blueValue;
        xSemaphoreGive(rgbMutex);
      }

      // Lê o valor do sensor de temperatura com mutex
      if (xSemaphoreTake(tmpMutex, portMAX_DELAY)) {
        localTmp = tmpValue;
        xSemaphoreGive(tmpMutex);
      }

      Serial.printf("Enviando -> Pot: %d | Tmp: %d | Bzz: %d | Btn: %d | R: %d G: %d B: %d\n", localPot, localTmp, localBzz, localBtn, localRed, localGreen, localBlue);

      // Envio para o Firebase
      bool ok = true;
      ok &= Firebase.setInt(fbdo, "/sensor/potenciometro", localPot);
      ok &= Firebase.setInt(fbdo, "/sensor/temperatura", localTmp);
      ok &= Firebase.setInt(fbdo, "/sensor/buzzer", localBzz);
      ok &= Firebase.setInt(fbdo, "/sensor/botao", localBtn);
      ok &= Firebase.setInt(fbdo, "/sensor/led/red", localRed);
      ok &= Firebase.setInt(fbdo, "/sensor/led/green", localGreen);
      ok &= Firebase.setInt(fbdo, "/sensor/led/blue", localBlue);

      if (ok) {
        Serial.println("Dados enviados com sucesso!");
      } else {
        Serial.printf("Erro ao enviar: %s\n", fbdo.errorReason().c_str());
      }



    } else {
      // MODO REMOTO: atualiza as variáveis locais com os valores do Firebase
      if (Firebase.getJSON(fbdo, "/sensor")) {
        json = fbdo.jsonObject();

        // Potenciômetro
        if (json.get(result, "potenciometro") && xSemaphoreTake(potMutex, portMAX_DELAY)) {
          potValue = result.intValue;
          Serial.printf("Pot: %d\n", potValue);
          xSemaphoreGive(potMutex);
        }

        // Buzzer
        if (json.get(result, "buzzer") && xSemaphoreTake(bzzMutex, portMAX_DELAY)) {
          bzzValue = result.intValue;
          xSemaphoreGive(bzzMutex);
        }

        // Botão
        if (json.get(result, "botao") && xSemaphoreTake(btnMutex, portMAX_DELAY)) {
          btnValue = result.boolValue;
          xSemaphoreGive(btnMutex);
        }

        // LED RGB
        if (json.get(result, "led/red") && xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
          redValue = result.intValue;
          xSemaphoreGive(rgbMutex);
        }
        if (json.get(result, "led/green") && xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
          greenValue = result.intValue;
          xSemaphoreGive(rgbMutex);
        }
        if (json.get(result, "led/blue") && xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
          blueValue = result.intValue;
          xSemaphoreGive(rgbMutex);
        }

        // Temperatura
        if (json.get(result, "temperatura") && xSemaphoreTake(tmpMutex, portMAX_DELAY)) {
          tmpValue = result.floatValue;
          xSemaphoreGive(tmpMutex);
        }

      } else {
        Serial.printf("Erro ao ler JSON: %s\n", fbdo.errorReason().c_str());
      }
      Serial.println("Dados atualizados a partir do Firebase.");
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS); // Aguarda 2 segundos
  }
}


// Task que faz a leitura do potenciômetro constantemente
void readPotTask(void *parameter) {
  int value;

  while (true) {

    if (!controlModeEnabled) {
      value = analogRead(POT_PIN); // lê valor analógico (0 a 4095)
    

      if (xSemaphoreTake(potMutex, portMAX_DELAY)) {
        potValue = value; // atualiza valor global
        xSemaphoreGive(potMutex);
      }
      
    }
      
    vTaskDelay(100 / portTICK_PERIOD_MS); // espera 100ms
  }
}

// Task que atualiza frequencia do buzzer constantemente
void buzzerTask(void *parameter) {
  int localBzz;

  while (true) { 

    if (xSemaphoreTake(bzzMutex, portMAX_DELAY)) {
      localBzz = bzzValue;
      xSemaphoreGive(bzzMutex);
    }

    ledcWriteTone(3, localBzz); // atualiza frequencia do buzzer

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Task que faz a leitura do botão constantemente
void readButtonTask(void *parameter) {
  bool localBtn = false;

  while (true) {

    if (!controlModeEnabled) {

      localBtn = !digitalRead(BUTTON_PIN); // inverte o valor (pressionado = 1)
      // Serial.printf("Botão lido: %d\n", rawBtn);
      

      if (xSemaphoreTake(btnMutex, portMAX_DELAY)) {
        btnValue = localBtn; 
        xSemaphoreGive(btnMutex);
      }

    }
      
    vTaskDelay(100 / portTICK_PERIOD_MS); // debounce simples (100 ms)
  }
}


// Task que atualiza o LED RGB constantemente
void rgbLedTask(void *parameter) {
  int r, g, b;

  while (true) {

    if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
      r = redValue;
      g = greenValue;
      b = blueValue;
      xSemaphoreGive(rgbMutex);
    }
    

    // Aplica PWM com os valores lidos
    ledcWrite(0, r); // canal 0 -> vermelho
    ledcWrite(1, g); // canal 1 -> verde
    ledcWrite(2, b); // canal 2 -> azul

    vTaskDelay(100 / portTICK_PERIOD_MS); // atualiza a cada 100ms
  }
}


// Task que faz a leitura da temperatura constantemente
void readTempTask(void *parameter) {
  int tmpRaw = 0;
  float voltage = 0.0;
  float temperatureC = 0.0;

  while (true) {

    if (!controlModeEnabled) {

      tmpRaw = analogRead(TEMP_PIN);
      voltage = (tmpRaw / 4095.0) * 3.3; // converte leitura para volts

      // Serial.printf("volts: %f\n", voltage);

      // Estimativa empírica da temperatura com base no módulo LM393 + NTC
      temperatureC = 77.5 - (voltage * 100.0);
      
      if (xSemaphoreTake(tmpMutex, portMAX_DELAY)) {
        tmpValue = (int)temperatureC; // arredonda para int
        xSemaphoreGive(tmpMutex);
      }

    }

    vTaskDelay(1000 / portTICK_PERIOD_MS); // leitura a cada 1s
  }
}


void updateHealthLEDs(int hp) {
  digitalWrite(RED_HEALTH_PIN, LOW);
  digitalWrite(YELLOW_HEALTH_PIN, LOW);
  digitalWrite(GREEN1_HEALTH_PIN, LOW);
  digitalWrite(GREEN2_HEALTH_PIN, LOW);
  digitalWrite(GREEN3_HEALTH_PIN, LOW);

  if (hp >= 0 && hp <= 20) {
    digitalWrite(RED_HEALTH_PIN, HIGH);
  }
  else if (hp >= 30 && hp <= 40) {
    digitalWrite(RED_HEALTH_PIN, HIGH);
    digitalWrite(YELLOW_HEALTH_PIN, HIGH);
  }
  else if (hp >= 50 && hp <= 70) {
    digitalWrite(RED_HEALTH_PIN, HIGH);
    digitalWrite(YELLOW_HEALTH_PIN, HIGH);
    digitalWrite(GREEN1_HEALTH_PIN, HIGH);
  }
  else if (hp >= 80 && hp <= 90) {
    digitalWrite(GREEN1_HEALTH_PIN, HIGH);
    digitalWrite(GREEN2_HEALTH_PIN, HIGH);
  }else if (hp == 100) {
    digitalWrite(GREEN1_HEALTH_PIN, HIGH);
    digitalWrite(GREEN2_HEALTH_PIN, HIGH);
    digitalWrite(GREEN3_HEALTH_PIN, HIGH);
  }
}

void healthControlTask(void *parameter) {
  bool currentAddState, currentSubState;

  while (true) {
    currentAddState = !digitalRead(BTN_ADD_PIN); // botão pressionado = true
    currentSubState = !digitalRead(BTN_SUB_PIN);

    // Aumenta vida
    if (currentAddState && !lastAddState) {
      if (xSemaphoreTake(hpMutex, portMAX_DELAY)) {
        healthPoints = min(100, healthPoints + 10);
        Serial.printf("Vida aumentada: %d\n", healthPoints);
        updateHealthLEDs(healthPoints); // atualiza LEDs
        xSemaphoreGive(hpMutex);
      }
    }

    // Diminui vida
    if (currentSubState && !lastSubState) {
      if (xSemaphoreTake(hpMutex, portMAX_DELAY)) {
        healthPoints = max(0, healthPoints - 10);
        Serial.printf("Vida diminuída: %d\n", healthPoints);
        updateHealthLEDs(healthPoints); // atualiza LEDs
        xSemaphoreGive(hpMutex);
      }
    }

    lastAddState = currentAddState;
    lastSubState = currentSubState;

    vTaskDelay(100 / portTICK_PERIOD_MS); // debounce simples
  }
}



// // Task que envia os dados para o Firebase a cada 1s
// void sendToFirebaseTask(void *parameter) {
//   int localPot;
//   // int localPot;
//   int localBtn;
//   int localRed, localGreen, localBlue;
//   int localTmp;

//   while (true) {
//     // if (xSemaphoreTake(frbMutex, portMAX_DELAY)) {
//       // Lê o valor do potenciômetro com mutex
//       if (xSemaphoreTake(potMutex, portMAX_DELAY)) {
//         localPot = potValue;
//         xSemaphoreGive(potMutex);
//       }

//       // Lê o valor do buzzer com mutex
//       if (xSemaphoreTake(bzzMutex, portMAX_DELAY)) {
//         localPot = bzzValue;
//         xSemaphoreGive(bzzMutex);
//       }

//       // Lê o valor do botão com mutex
//       if (xSemaphoreTake(btnMutex, portMAX_DELAY)) {
//         localBtn = btnValue;
//         xSemaphoreGive(btnMutex);
//       }

//       // Lê os valores RGB com mutex
//       if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
//         localRed = redValue;
//         localGreen = greenValue;
//         localBlue = blueValue;
//         xSemaphoreGive(rgbMutex);
//       }

//       // Lê o valor do sensor de temperatura com mutex
//       if (xSemaphoreTake(tmpMutex, portMAX_DELAY)) {
//         localTmp = tmpValue;
//         xSemaphoreGive(tmpMutex);
//       }

//       Serial.printf("Enviando -> Pot: %d | Tmp: %d | Bzz: %d | Btn: %d | R: %d G: %d B: %d\n", localPot, localTmp, localPot, localBtn, localRed, localGreen, localBlue);

//       // Envio para o Firebase
//       bool ok = true;
//       ok &= Firebase.setInt(fbdo, "/sensor/potenciometro", localPot);
//       ok &= Firebase.setInt(fbdo, "/sensor/temperatura", localTmp);
//       ok &= Firebase.setInt(fbdo, "/sensor/buzzer", localPot);
//       ok &= Firebase.setInt(fbdo, "/sensor/botao", localBtn);
//       ok &= Firebase.setInt(fbdo, "/sensor/led/red", localRed);
//       ok &= Firebase.setInt(fbdo, "/sensor/led/green", localGreen);
//       ok &= Firebase.setInt(fbdo, "/sensor/led/blue", localBlue);

//       if (ok) {
//         Serial.println("Dados enviados com sucesso!");
//       } else {
//         Serial.printf("Erro ao enviar: %s\n", fbdo.errorReason().c_str());
//       }
      
//       // xSemaphoreGive(frbMutex);
//     // }
//       vTaskDelay(1000 / portTICK_PERIOD_MS); // espera 1 segundo
//   }
// }




// SETUP==============

void setup() {
  Serial.begin(115200); // Inicializa comunicação serial

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Inicia conexão Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.reconnectNetwork(true); // reconecta automaticamente
  fbdo.setBSSLBufferSize(4096, 1024); // buffer de comunicação segura
  Firebase.begin(&config, &auth); // inicializa Firebase

  pinMode(POT_PIN, INPUT); // define pino como entrada

  // PWM para o buzzer 
  ledcSetup(3, 2000, 8); // 2kHz, 8 bits
  ledcAttachPin(BUZZER_PIN, 3);

  pinMode(BUTTON_PIN, INPUT_PULLUP); // botão com resistor de pull-up interno

  // PWM nos pinos RGB (resolução 8 bits, freq 5kHz)
  ledcSetup(0, 5000, 8);
  ledcAttachPin(RED_PIN, 0);

  ledcSetup(1, 5000, 8);
  ledcAttachPin(GREEN_PIN, 1);

  ledcSetup(2, 5000, 8);
  ledcAttachPin(BLUE_PIN, 2);

  pinMode(BTN_ADD_PIN, INPUT_PULLUP);
  pinMode(BTN_SUB_PIN, INPUT_PULLUP);

  pinMode(RED_HEALTH_PIN, OUTPUT);
  pinMode(YELLOW_HEALTH_PIN, OUTPUT);
  pinMode(GREEN1_HEALTH_PIN, OUTPUT);
  pinMode(GREEN2_HEALTH_PIN, OUTPUT);
  pinMode(GREEN3_HEALTH_PIN, OUTPUT);

  // Criação dos mutexes
  frbMutex = xSemaphoreCreateMutex();
  conMutex = xSemaphoreCreateMutex();
  potMutex = xSemaphoreCreateMutex();
  bzzMutex = xSemaphoreCreateMutex();
  btnMutex = xSemaphoreCreateMutex();
  rgbMutex = xSemaphoreCreateMutex();
  tmpMutex = xSemaphoreCreateMutex();
  hpMutex = xSemaphoreCreateMutex();


  // Cria a task que lê o controle_mode no core 0
  xTaskCreatePinnedToCore(
    controlModeTask,
    "ControlModeTask",
    16384,   // 4 KB de stack (mais seguro para uso de Firebase)
    NULL,
    1,
    NULL,
    0
  );

  // Cria a thread da leitura do potenciômetro no core 1
  xTaskCreatePinnedToCore(
    readPotTask,      // função
    "ReadPotTask",    // nome
    4096,             // stack size
    NULL,             // parâmetro
    1,                // prioridade
    NULL,             // handle
    1                 // core 1
  );


  // Cria a thread para o buzzer no core 1
  xTaskCreatePinnedToCore(
    buzzerTask,      // função
    "BuzzerTask",    // nome
    4096,             // stack size
    NULL,             // parâmetro
    1,                // prioridade
    NULL,             // handle
    1                 // core 1
  );


  // Cria a task que lê o botão no core 0
  xTaskCreatePinnedToCore(
    readButtonTask,
    "ReadButtonTask",
    4096,
    NULL,
    1,
    NULL,
    0
  );


  // Cria a thread para o LED RGB no core 0
  xTaskCreatePinnedToCore(
    rgbLedTask,       // função
    "RgbLedTask",     // nome
    4096,             // stack size
    NULL,             // parâmetro
    1,                // prioridade
    NULL,             // handle
    0                 // core 0
  );

  // Cria a thread para o sensor de temperatura no core 0
  xTaskCreatePinnedToCore(
    readTempTask,       // função
    "ReadTempTask",     // nome
    4096,             // stack size
    NULL,             // parâmetro
    1,                // prioridade
    NULL,             // handle
    0                 // core 0
  );

  xTaskCreatePinnedToCore(
    healthControlTask,
    "HealthControlTask",
    4096,
    NULL,
    1,
    NULL,
    1
  );

  updateHealthLEDs(healthPoints);
  
  // // Cria a task que envia os dados para o Firebase no core 1
  // xTaskCreatePinnedToCore(
  //   sendToFirebaseTask,
  //   "SendToFirebaseTask",
  //   8182,
  //   NULL,
  //   1,
  //   NULL,
  //   1
  // );
}

void loop() {
  static unsigned long lastSend = 0;

  int localPot = potValue;
  if (!controlModeEnabled) {
    if (xSemaphoreTake(potMutex, portMAX_DELAY)) {
      localPot = potValue;

      // Serial.printf("Pot lido: %d\n", localPot);
      xSemaphoreGive(potMutex);
    }
  
    if (xSemaphoreTake(hpMutex, portMAX_DELAY)) {
      Serial.printf("Vida atual: %d\n", healthPoints);
      xSemaphoreGive(hpMutex);
    }
    
    // Atualiza valores RGB com base em alguma lógica ou Firebase (exemplo: fixo)
    if (xSemaphoreTake(rgbMutex, portMAX_DELAY)) {
      redValue = (localPot / 16) % 256;   // converte 0–4095 para 0–255
      greenValue = 255 - redValue;          // valor inverso
      blueValue = (redValue + 100) % 256;   // efeito visual diferente
      xSemaphoreGive(rgbMutex);
    }
    
    if (xSemaphoreTake(bzzMutex, portMAX_DELAY)) {
      bzzValue = localPot;
      xSemaphoreGive(bzzMutex);
    }
  

  delay(1000); // printa a cada 1 segundo

  }
  

  
}
