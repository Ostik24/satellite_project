const int pinVoltageBattery1 = A0; // Напруга батареї 1
const int pinVoltageBattery2 = A1; // Напруга батареї 2
const int pinVoltageSolarPanel = A2; // Напруга сонячної панелі
const int pinCurrentSensor = A3; // Датчик струму
const int pinOutputVoltage = A4; // Вихідна напруга перед DC-DC

// Піни для керування реле або MOSFET
const int pinRelaySolar = 7; // Управління реле сонячної панелі
const int pinRelayBattery1 = 8; // Управління реле батареї 1
const int pinRelayBattery2 = 9; // Управління реле батареї 2

// Порогові значення
const float minBatteryVoltage = 6.5; // Мінімальна допустима напруга батареї
const float maxBatteryVoltage = 8.4; // Максимальна напруга заряду батареї
const float minSolarVoltage = 7.5; // Мінімальна напруга сонячної панелі для використання
const float outputVoltageThresholdLow = 4.8; // Мінімальний рівень вихідної напруги
const float outputVoltageThresholdHigh = 5.2; // Максимальний рівень вихідної напруги

void setup() {
 // Ініціалізація серійного порту для моніторингу
 Serial.begin(9600);

 // Налаштування пінів реле як виходів
 pinMode(pinRelaySolar, OUTPUT);
 pinMode(pinRelayBattery1, OUTPUT);
 pinMode(pinRelayBattery2, OUTPUT);

 // Відключаємо все реле на старті
 digitalWrite(pinRelaySolar, LOW);
 digitalWrite(pinRelayBattery1, LOW);
 digitalWrite(pinRelayBattery2, LOW);
}

void loop() {
 // Зчитування значень напруги та струму
 float voltageBattery1 = readVoltage(pinVoltageBattery1);
 float voltageBattery2 = readVoltage(pinVoltageBattery2);
 float solarPanelVoltage = readVoltage(pinVoltageSolarPanel);
 float outputVoltage = readVoltage(pinOutputVoltage);
 float current = readCurrent(pinCurrentSensor);

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
 } else if (voltageBattery2 < maxBatteryVoltage) {
 chargeBattery(2);
 }
 }

 // Вибір джерела живлення
 if (solarPanelVoltage > minSolarVoltage) {
 // Використовуємо сонячну панель для живлення
 switchToSolarPanel();
 } else {
 // Перемикаємося на батареї
 if (voltageBattery1 > minBatteryVoltage) {
 switchToBattery(1);
 } else if (voltageBattery2 > minBatteryVoltage) {
 switchToBattery(2);
 } else {
 // Критичний рівень заряду батарей, відключаємо систему
 Serial.println("Critical battery level! Shutting down.");
 shutdownSystem();
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
 return analogRead(pin) * (5.0/1023.0); // Конвертація аналогового значення
}

void switchToSolarPanel() {
 digitalWrite(pinRelaySolar, HIGH);
 digitalWrite(pinRelayBattery1, LOW);
 digitalWrite(pinRelayBattery2, LOW);
 Serial.println("Switched to Solar Panel.");
}

void switchToBattery(int battery) {
 if (battery == 1) {
 digitalWrite(pinRelayBattery1, HIGH);
 digitalWrite(pinRelaySolar, LOW);
 digitalWrite(pinRelayBattery2, LOW);
 Serial.println("Switched to Battery 1.");
 } else if (battery == 2) {
 digitalWrite(pinRelayBattery2, HIGH);
 digitalWrite(pinRelaySolar, LOW);
 digitalWrite(pinRelayBattery1, LOW);
 Serial.println("Switched to Battery 2.");
 }
}

void chargeBattery(int battery) {
 Serial.print("Charging Battery");
 Serial.print(battery);
 Serial.println(".");

 if (battery == 1) {
   digitalWrite(pinRelayBattery1, HIGH); // Включаємо реле для зарядки батареї 1
 } else if (battery == 2) {
   digitalWrite(pinRelayBattery2, HIGH); // Включаємо реле для зарядки батареї 2
 }
}

// Функція для відключення системи
void shutdownSystem() {
 // Вимикаємо всі реле
 digitalWrite(pinRelaySolar, LOW);
 digitalWrite(pinRelayBattery1, LOW);
 digitalWrite(pinRelayBattery2, LOW);
 
 Serial.println("System is shutting down due to low battery levels.");
 while (true) {
   // Зациклюємось для зупинки роботи програми
   delay(1000);
 }
}
