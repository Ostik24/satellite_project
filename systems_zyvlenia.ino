const int pinVoltageBattery1 = A2; // Напруга батареї 1
const int pinVoltageBattery2 = A1; // Напруга батареї 2
const int pinVoltageSolarPanel = A4; // Напруга сонячної панелі
const int pinCurrentSensor = A3; // Датчик струму
const int pinOutputVoltage = A5; // Вихідна напруга перед DC-DC

// Піни для керування реле або MOSFET
const int pinRelaySolar = 12; // Управління реле сонячної панелі PB4
const int pinRelayBattery1 = 11; // Управління реле батареї 1 PB3
const int pinRelayBattery2 = 10; // Управління реле батареї 2 PB2
const int pinRelayMain = 9;

// Порогові значення
const float minBatteryVoltage = 7.2; // Мінімальна допустима напруга батареї
const float maxBatteryVoltage = 8.4; // Максимальна напруга заряду батареї
const float minSolarVoltage = 7; // Мінімальна напруга сонячної панелі для використання
const float maxSolarVoltage = 9;
const float outputVoltageThresholdLow = 4.8; // Мінімальний рівень вихідної напруги
const float outputVoltageThresholdHigh = 5.2; // Максимальний рівень вихідної напруги

int charge = 0;
const int ON = 0;
const int OFF = 1;

void setup() {
  // Ініціалізація серійного порту для моніторингу
  Serial.begin(9600);

  Serial.print("RAW ADC1:"); Serial.println(analogRead(pinVoltageBattery1));
  Serial.print("RAW ADC2:"); Serial.println(analogRead(pinVoltageBattery2));
  Serial.print("RAW ADC3:"); Serial.println(analogRead(pinVoltageSolarPanel));
  Serial.print("RAW ADC4:"); Serial.println(analogRead(pinCurrentSensor));
  Serial.print("RAW ADC5:"); Serial.println(analogRead(pinOutputVoltage));
  delay(100);


  // Налаштування пінів реле як виходів
  pinMode(pinRelaySolar, OUTPUT);
  pinMode(pinRelayBattery1, OUTPUT);
  pinMode(pinRelayBattery2, OUTPUT);
  pinMode(pinRelayMain, OUTPUT);

  // Відключаємо все реле на старті
  digitalWrite(pinRelaySolar, OFF);
  digitalWrite(pinRelayBattery1, OFF);
  digitalWrite(pinRelayBattery2, OFF);
  digitalWrite(pinRelayMain, OFF);
  delay(1);
}

void loop() {
  // Зчитування значень напруги та струму
  float voltageBattery1 = readVoltage(pinVoltageBattery1);
  float voltageBattery2 = readVoltage(pinVoltageBattery2);
  float solarPanelVoltage = readVoltage(pinVoltageSolarPanel);
  float outputVoltage = readVoltage(pinOutputVoltage);
  float current = readCurrent(pinCurrentSensor);

  // Управління основним реле
  if (voltageBattery1 <= minBatteryVoltage and voltageBattery2 <= minBatteryVoltage and solarPanelVoltage <= minSolarVoltage) {
    digitalWrite(pinRelayMain, OFF);
  } else {
    digitalWrite(pinRelayMain, ON);
  }

  // Виведення значень на серійний монітор
  Serial.print("Battery 1 Voltage:"); Serial.println(voltageBattery1);
  Serial.print("Battery 2 Voltage:"); Serial.println(voltageBattery2);
  Serial.print("Solar Panel Voltage:"); Serial.println(solarPanelVoltage);
  Serial.print("Output Voltage:"); Serial.println(outputVoltage);
  Serial.print("Current:"); Serial.println(current);


  // Заряджання батарей за наявності сонячної енергії
  if (solarPanelVoltage > maxBatteryVoltage) {
    if (voltageBattery1 < maxBatteryVoltage) {
      chargeBattery(1);
      charge = 1;
    } else if (voltageBattery2 < maxBatteryVoltage) {
      chargeBattery(2);
      charge = 1;
    } else if (voltageBattery2 < maxBatteryVoltage and voltageBattery1 < maxBatteryVoltage and solarPanelVoltage > minSolarVoltage) {
      chargeBattery(1);
      charge = 1;
    } else {
      charge = 0;
    }
  }

  // Вибір джерела живлення
  if (solarPanelVoltage >= maxSolarVoltage and charge == 0) {
    // Використовуємо сонячну панель для живлення
    switchToSolarPanel();
  } else {
    offSolarPanel();
    // Перемикаємося на батареї
    if (voltageBattery1 > minBatteryVoltage) {
      switchToBattery(1);
    } else if (voltageBattery2 > minBatteryVoltage) {
      switchToBattery(2);
    } else {
      if (solarPanelVoltage <= minSolarVoltage) {
        // Критичний рівень заряду батарей, відключаємо систему
        Serial.println("Critical battery level! Shutting down.");
        shutdownSystem();
      }
      else {
        Serial.println("Critical battery level! try charge.");
      }
    }
  }

  // Перевірка вихідної напруги
  if (outputVoltage < outputVoltageThresholdLow || outputVoltage > outputVoltageThresholdHigh) {
    Serial.println("Output voltage out of range! Check power source.");
  }

  delay(1000); // Затримка 1 секунда перед наступним циклом
}

// Функції для зчитування даних із датчиків
float readVoltage(int pin) {
  return analogRead(pin) * (5.0 / 1023.0) * 2; // Передбачається дільник напруги 1:2
}

float readCurrent(int pin) {
  return analogRead(pin) * (5.0 / 1023.0); // Конвертація аналогового значення
}

void switchToSolarPanel() {
  digitalWrite(pinRelaySolar, ON);
  digitalWrite(pinRelayBattery1, OFF);
  digitalWrite(pinRelayBattery2, OFF);
  Serial.println("Switched to Solar Panel.");
}
void offSolarPanel() {
  digitalWrite(pinRelaySolar, OFF);
  Serial.println("Solar Panel low voltage switch off.");
}


void switchToBattery(int battery) {
  if (battery == 1) {
    digitalWrite(pinRelayBattery1, OFF);
    digitalWrite(pinRelayBattery2, ON);
    Serial.println("Switched to Battery 1.");
  } else if (battery == 2) {
    digitalWrite(pinRelayBattery2, OFF);
    digitalWrite(pinRelayBattery1, ON);
    Serial.println("Switched to Battery 2.");
  }
}

void chargeBattery(int battery) {
  Serial.print("Charging Battery");
  Serial.print(battery);
  Serial.println(".");
  digitalWrite(pinRelaySolar, ON);

  if (battery == 1) {
    digitalWrite(pinRelayBattery1, ON); // Включаємо реле для зарядки батареї 1
    digitalWrite(pinRelayBattery2, OFF);
  } else if (battery == 2) {
    digitalWrite(pinRelayBattery2, ON); // Включаємо реле для зарядки батареї 2
    digitalWrite(pinRelayBattery1, OFF);
  }
}

// Функція для відключення системи
void shutdownSystem() {
  // Вимикаємо всі реле
  digitalWrite(pinRelaySolar, OFF);
  digitalWrite(pinRelayBattery1, OFF);
  digitalWrite(pinRelayBattery2, OFF);
  digitalWrite(pinRelayMain, OFF);

  Serial.println("System is shutting down due to low battery levels.");
  delay(2000);
}
