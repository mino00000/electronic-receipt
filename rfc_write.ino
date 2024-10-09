#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

MFRC522::MIFARE_Key key;
char email[40];  // 사용자의 이메일을 입력받기 위한 배열 (최대 40바이트로 설정)

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  // 키를 기본 키로 설정 (6 바이트 모두 0xFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("NFC 쓰기 준비 완료. 카드를 태그하세요.");
  Serial.println("이메일을 입력하세요 (최대 16바이트):");
}

void loop() {
  // 이메일 입력 받기
  if (Serial.available() > 0) {
    int len = Serial.readBytesUntil('\n', email, sizeof(email) - 1); // '\n'이 입력되면 이메일 입력 종료
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

    // 인증
    byte block = 4; // 데이터 쓰기를 원하는 블록 번호 (섹터 1의 첫 번째 블록)
    MFRC522::StatusCode status;

    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
      Serial.print("인증 실패: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }

    // 데이터 준비 (16바이트)
    byte dataBlock[16];
    strncpy((char*)dataBlock, email, 16); // 이메일을 16바이트로 자름
    // 이메일이 16바이트보다 짧다면 나머지 공간을 0으로 채움
    for (int i = strlen((char*)dataBlock); i < 16; i++) {
      dataBlock[i] = 0;
    }

    // 데이터 쓰기
    status = mfrc522.MIFARE_Write(block, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("쓰기 실패: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
      Serial.println("데이터 쓰기 성공!");
    }

    mfrc522.PICC_HaltA();      // 카드 중지
    mfrc522.PCD_StopCrypto1(); // 암호화 중지
    delay(1000); // 다음 동작을 위해 지연
  }
}
