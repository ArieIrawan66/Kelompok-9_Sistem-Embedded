#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Inisialisasi LCD 16x2 dengan alamat I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin sensor ultrasonik
const int trigPin = 8;
const int echoPin = 9;

// Pin relay
const int relayPin = 7;

// Konstanta ukuran tangki (dalam cm)
const float tankHeight = 30; // Tinggi maksimal air dalam tangki
const float tankRadius = 12.5; // Radius tangki (untuk menghitung volume)

// Variabel untuk perhitungan
long duration;
float distance;
float waterVolume;

void setup() {
  // Inisialisasi pin
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);

  // Relay awalnya mati (solenoid valve tertutup)
  digitalWrite(relayPin, LOW);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Volume: ");
  lcd.setCursor(0, 1);
  lcd.print("Sistem Siap...");

  // Inisialisasi Serial Monitor
  Serial.begin(9600);
  Serial.println("Sistem Monitoring Air Aktif...");
  Serial.println("================================");
  delay(2000); // Tunggu 2 detik sebelum mulai
}

void loop() {
  // Mengukur jarak permukaan air
  distance = getWaterLevel();

  // Hitung volume air di tangki (menggunakan rumus volume silinder)
  if (distance < tankHeight) {
    float waterHeight = tankHeight - distance; // Tinggi air (dalam cm)
    waterVolume = 3.14 * tankRadius * tankRadius * (waterHeight / 1000.0); // Volume dalam liter
  } else {
    waterVolume = 0.0; // Tangki kosong
  }

  // Tampilkan volume air di LCD
  lcd.setCursor(0, 0);
  lcd.print("Volume: ");
  lcd.print(waterVolume, 1);
  lcd.print(" L     ");

  // Tampilkan hasil di Serial Monitor
  Serial.print("Volume Air: ");
  Serial.print(waterVolume, 1);
  Serial.println(" L");

  // Kontrol solenoid valve
  if (waterVolume > 12.0) { // Jika volume air lebih dari 10 liter
    digitalWrite(relayPin, HIGH); // Buka solenoid valve (air mengalir ke pembuangan)
    lcd.setCursor(0, 1);
    lcd.print("Pembuangan Aktif");
    Serial.println("Status: Pembuangan Aktif");
  } else {
    digitalWrite(relayPin, LOW); // Tutup solenoid valve (air mengalir ke tangki)
    lcd.setCursor(0, 1);
    lcd.print("Pengisian Aktif ");
    Serial.println("Status: Pengisian Aktif");
  }

  Serial.println("--------------------------------");
  delay(1000); // Update setiap 1 detik
}

// Fungsi untuk mendapatkan jarak permukaan air dari sensor ultrasonik
float getWaterLevel() {
  // Kirim sinyal TRIG
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Baca waktu pantulan (ECHO)
  duration = pulseIn(echoPin, HIGH);

  // Konversi waktu ke jarak (cm)
  float distance = duration * 0.034 / 2;

  // Pastikan jarak dalam batas tangki
  if (distance > tankHeight) {
    return tankHeight;
  } else {
    return distance;
  }
}
