#include "Arduino.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "SevSeg.h" // 7세그먼트 디스플레이 라이브러리

SevSeg sevseg; // 7세그먼트 디스플레이 객체 생성

// 네트워크 인증 정보를 입력하세요.
const char *ssid = "TINYWAVELAB";   // WiFi 이름
const char *password = "tinywave1"; // Wifi 비밀번호

// 라즈베리 파이의 외부 IP 주소 및 Myphpadmin에 게시 지시를 담은 /var/www/html/ 디렉터리의 파일 이름
const char *serverName = "http://223.171.143.206/post-countdb.php";

// php 파일에 있는 키와 일치해야 합니다. 이렇게 하면 외부 사용자가 서버에 데이터를 보내지 못하게 할 수 있습니다.
String apiKeyValue = "tPmAT5Ab3j7F9";

// sensorName과 sensorLocation은 서로 다른 센서를 식별하기 위해 자유롭게 수정할 수 있습니다.
String sensorName = "IR";            // 센서 이름
String sensorLocation = "Machine 1"; // 센서 위치

const int IRsensorPin = 27; // IR 센서 핀 번호

// 함수 선언부
void delaym(int time);           // 디스플레이를 갱신하면서 딜레이하는 함수
void Task1code(void *parameter); // 코어에 할당된 작업을 처리하는 함수
void setupWiFi();                // WiFi 설정 함수
void beginSevenSeg();            // 7세그먼트 디스플레이 초기화 함수
void sendPostRequest(int count); // POST 요청을 전송하는 함수

void setup()
{
  beginSevenSeg();      // 7세그먼트 디스플레이 초기화
  Serial.begin(115200); // 시리얼 통신 시작
  setupWiFi();          // WiFi 설정
  // FreeRTOS 태스크 생성
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, NULL, 0);
}

void loop()
{
  static int objectCount = 0; // 객체 카운트
  static int preMillis = 0;   // 이전 밀리초 시간

  for (;;) // 무한 루프
  {
    if (digitalRead(IRsensorPin) == LOW) // IR 센서가 LOW일 때
    {
      preMillis = millis();            // 현재 밀리초 시간 저장
      while (millis() - preMillis < 5) // 5밀리초 동안
      {
        sevseg.refreshDisplay(); // 디스플레이 갱신
      }
      if (digitalRead(IRsensorPin) == LOW) // 다시 IR 센서가 LOW일 때
      {
        objectCount++;                          // 객체 카운트 증가
        Serial.println(objectCount);            // 카운트 출력
        sevseg.setNumber(objectCount);          // 카운트를 7세그먼트에 출력
        sendPostRequest(objectCount);           // 카운트를 서버에 전송
        preMillis = millis();                   // 현재 밀리초 시간 저장
        while (digitalRead(IRsensorPin) == LOW) // IR 센서가 LOW일 때까지
        {
          sevseg.refreshDisplay(); // 디스플레이 갱신
        }
      }
    }
  }
}

void delaym(int time) // 디스플레이를 갱신하면서 딜레이하는 함수
{
  int prevMillis = millis();
  while (millis() - prevMillis < time) // 지정한 시간 동안
  {
    sevseg.refreshDisplay(); // 디스플레이 갱신
  }
}

void Task1code(void *parameter) // 코어에 할당된 작업을 처리하는 함수
{
  int prevMillis = 0;                     // 이전 밀리초 시간
  Serial.print("Task1 running on core "); // 코어 작업 실행 메시지 출력
  for (;;)
  {
    sevseg.refreshDisplay();          // 디스플레이 갱신
    if (prevMillis < millis() - 1000) // 1초가 지나면
    {
      vTaskDelay(1);         // 태스크 딜레이
      prevMillis = millis(); // 현재 밀리초 시간 저장
    }
  }
}

void setupWiFi() // WiFi 설정 함수
{
  WiFi.begin(ssid, password);           // WiFi 연결
  Serial.println("Connecting");         // 연결 메시지 출력
  while (WiFi.status() != WL_CONNECTED) // WiFi가 연결될 때까지
  {
    delaym(500);       // 500밀리초 딜레이
    Serial.print("."); // 연결 상태 출력
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); // WiFi IP 주소 출력
}

void beginSevenSeg() // 7세그먼트 디스플레이 초기화 함수
{
  byte numDigits = 3;                                  // 디지트 개수
  byte digitPins[] = {25, 33, 32};                     // 디지트 핀
  byte segmentPins[] = {18, 19, 5, 4, 13, 14, 16, 17}; // 세그먼트 핀
  bool resistorsOnSegments = false;                    // 세그먼트에 저항이 있는지 여부
  byte hardwareConfig = COMMON_CATHODE;                // 하드웨어 설정 (공통 캐소드)
  bool updateWithDelays = false;                       // 딜레이로 업데이트 여부
  bool leadingZeros = false;                           // 선행 0 여부
  bool disableDecPoint = false;                        // 소수점 비활성화 여부

  // 7세그먼트 디스플레이 시작
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90); // 밝기 설정
}

void sendPostRequest(int count) // POST 요청을 전송하는 함수
{
  if (WiFi.status() == WL_CONNECTED) // WiFi가 연결되어 있으면
  {
    WiFiClient client;                                                                                                                            // WiFi 클라이언트 생성
    HTTPClient http;                                                                                                                              // HTTP 클라이언트 생성
    http.begin(client, serverName);                                                                                                               // HTTP 시작
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");                                                                          // 헤더 추가
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName + "&location=" + sensorLocation + "&count=" + String(count) + ""; // POST 데이터
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);                   // POST 데이터 출력
    int httpResponseCode = http.POST(httpRequestData); // POST 요청 전송
    if (httpResponseCode > 0)                          // 응답 코드가 0보다 크면
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode); // 응답 코드 출력
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode); // 오류 코드 출력
    }
    http.end(); // HTTP 종료
  }
  else
  {
    Serial.println("WiFi Disconnected"); // WiFi 연결 끊김 메시지 출력
  }
}
