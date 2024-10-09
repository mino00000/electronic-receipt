#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
char email[48];  // 이메일 주소 최대 48바이트로 설정

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  // 기본 키 설정 (6 바이트 모두 0xFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("NFC 쓰기 준비 완료. 카드를 태그하세요.");
  Serial.println("이메일을 입력하세요 (최대 48바이트):");
}

void loop() {
  // 이메일 입력 받기
  if (Serial.available() > 0) {
    int len = Serial.readBytesUntil('\n', email, sizeof(email) - 1);  // '\n'이 입력되면 이메일 입력 종료
    email[len] = '\0';  // 문자열 끝에 null 추가

    Serial.print("입력된 이메일: ");
    Serial.println(email);
    Serial.println("카드를 태그하세요.");

    // 카드 감지
    while (!mfrc522.PICC_IsNewCardPresent()) {
      delay(100);
    }
    // 카드 선택
    if (!mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    // 블록 4부터 6까지 데이터를 나누어 저장
    byte blocks[3][16];  // 3개의 블록에 각각 16바이트씩 저장
    memset(blocks, 0, sizeof(blocks));  // 모든 블록을 0으로 초기화
    strncpy((char*)blocks[0], email, 16);  // 첫 번째 블록에 첫 16바이트 저장
    if (len > 16) {
      strncpy((char*)blocks[1], email + 16, 16);  // 두 번째 블록에 그 다음 16바이트 저장
    }
    if (len > 32) {
      strncpy((char*)blocks[2], email + 32, 16);  // 세 번째 블록에 그 다음 16바이트 저장
    }

    // 인증 및 데이터 쓰기
    for (byte i = 0; i < 3; i++) {
      byte block = 4 + i;  // 블록 4, 5, 6 사용
      MFRC522::StatusCode status;

      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.print("인증 실패: ");
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
      }

      // 데이터 쓰기
      status = mfrc522.MIFARE_Write(block, blocks[i], 16);
      if (status != MFRC522::STATUS_OK) {
        Serial.print("쓰기 실패 (블록 ");
        Serial.print(block);
        Serial.print("): ");
        Serial.println(mfrc522.GetStatusCodeName(status));
      } else {
        Serial.print("데이터 쓰기 성공 (블록 ");
        Serial.print(block);
        Serial.println(")!");
      }
    }

    mfrc522.PICC_HaltA();      // 카드 중지
    mfrc522.PCD_StopCrypto1(); // 암호화 중지
    delay(1000); // 다음 동작을 위해 지연
  }
}
