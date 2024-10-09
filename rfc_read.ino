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

  // 인증
  byte block = 4; // 읽고자 하는 블록 번호 (섹터 1, 블록 4)
  MFRC522::StatusCode status;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("인증 실패: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // 데이터 읽기
  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("읽기 실패: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.print("읽은 데이터: ");
    for (byte i = 0; i < 16; i++) {
      Serial.write(buffer[i]);  // 데이터를 시리얼로 전송
    }
    Serial.println();
  }

  mfrc522.PICC_HaltA();      // 카드 중지
  mfrc522.PCD_StopCrypto1(); // 암호화 중지
  delay(1000); // 다음 동작 대기
}
