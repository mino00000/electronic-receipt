#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);   // 시리얼 통신 시작
  SPI.begin();          // SPI 버스 시작
  mfrc522.PCD_Init();   // MFRC522 초기화

  // 기본 키 (6바이트 모두 0xFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("NFC 카드 태그 대기 중...");
}

void loop() {
  // 카드 감지
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // 카드 선택
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // 3개의 블록(4, 5, 6)에서 이메일을 읽어올 배열
  char email[48] = {0}; // 최대 48바이트
  byte buffer[18];      // 데이터 읽기 버퍼
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;

  // 블록 4, 5, 6 읽기
  for (byte i = 0; i < 3; i++) {
    byte block = 4 + i;  // 블록 4, 5, 6 사용

    // 인증
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("인증 실패 (블록 ");
      Serial.print(block);
      Serial.print("): ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // 데이터 읽기
    status = mfrc522.MIFARE_Read(block, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("읽기 실패 (블록 ");
      Serial.print(block);
      Serial.print("): ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    } else {
      // 읽어온 데이터를 이메일 배열에 복사
      strncat(email, (char*)buffer, 16);
    }
  }

  // 읽은 이메일 출력
  Serial.print("읽은 이메일: ");
  Serial.println(email);

  mfrc522.PICC_HaltA();      // 카드 중지
  mfrc522.PCD_StopCrypto1(); // 암호화 중지
  delay(1000); // 다음 동작 대기
}
