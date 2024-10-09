프로젝트 구조
NFC 카드에 정보를 쓰는 방법
RC522 모듈 연결 및 핀맵
POS 시스템과 RC522 모듈의 연계
COM 포트 설정 주의 사항
구글 SMTP 설정 (Gmail 앱 비밀번호 사용)
프로젝트 이름: NFC 기반 POS 시스템
프로젝트 설명
이 프로젝트는 NFC 태그를 사용하여 결제 및 영수증 발행 기능을 구현하는 POS 시스템입니다. RC522 NFC 리더 모듈을 통해 사용자의 이메일 주소를 NFC 카드에 쓰고, 결제 시 해당 이메일로 전자 영수증을 발송하는 기능을 제공합니다.

1. NFC 카드에 정보를 쓰는 방법
NFC 카드에 정보를 쓰기 위해 RC522 모듈을 사용합니다. 먼저, NFC 카드에 정보를 쓰는 코드를 통해 이메일 주소나 기타 데이터를 NFC 카드에 저장해야 합니다.

예시 코드 (NFC 카드에 정보 쓰기):
cpp
코드 복사
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  // 기본 키 설정 (6 바이트 모두 0xFF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("NFC 카드에 정보를 쓰기 위해 태그하세요.");
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

  byte block = 4; // 데이터를 쓸 블록 번호
  MFRC522::StatusCode status;

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("인증 실패: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  byte dataBlock[16] = "example@mail.com";  // 이메일 주소 입력
  status = mfrc522.MIFARE_Write(block, dataBlock, 16);
  if (status == MFRC522::STATUS_OK) {
    Serial.println("NFC 카드에 정보가 성공적으로 쓰여졌습니다.");
  } else {
    Serial.println("쓰기 실패");
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
2. RC522 모듈 연결 및 핀맵
다음은 RC522 NFC 모듈을 Arduino에 연결하는 방법입니다. 각 핀의 연결을 정확하게 해야 모듈이 정상적으로 동작합니다.

RC522 핀	Arduino 핀
SDA	D10
SCK	D13
MOSI	D11
MISO	D12
GND	GND
RST	D9
3.3V	3.3V
위 핀맵을 따라 Arduino에 NFC 리더 모듈을 연결하세요. 이 핀 연결은 NFC 리더와 Arduino 간의 통신을 위해 매우 중요합니다.

3. POS 시스템과 RC522 모듈의 연계
이 프로젝트의 핵심은 POS 시스템과 RC522 NFC 리더 모듈을 연계하는 것입니다. NFC 리더 모듈을 통해 이메일 주소를 읽고, 해당 정보를 POS 시스템에 전달하여 영수증을 발송합니다.

POS 시스템 코드는 main.py에서 관리하며, 다음과 같은 주요 기능을 포함합니다:

결제 금액 입력 및 처리
NFC 태그를 통해 이메일 읽기
이메일 전송(전자 영수증 발송)
4. COM 포트 설정 주의 사항
POS 시스템과 RC522 모듈 간의 시리얼 통신을 설정할 때 COM 포트 설정이 중요합니다. Arduino가 연결된 COM 포트를 올바르게 설정해야 NFC 데이터를 읽고 POS 시스템과 통신할 수 있습니다.

Arduino IDE 또는 Device Manager에서 Arduino가 연결된 포트를 확인하세요.
소스 코드에서 아래 부분을 해당 포트로 변경하세요:
python
코드 복사
ser = serial.Serial('COM12', 9600)  # 아두이노와의 시리얼 통신 설정
5. 구글 SMTP 설정 (Gmail 앱 비밀번호 사용)
이 프로젝트는 Gmail SMTP 서버를 통해 이메일을 전송합니다. 이를 위해 Google의 앱 비밀번호를 사용해야 합니다.

앱 비밀번호 설정 방법:
Google 계정에 로그인합니다.
보안 설정으로 이동한 후 2단계 인증을 활성화합니다.
앱 비밀번호를 생성하여 프로젝트에서 사용할 비밀번호로 설정합니다.
SMTP 서버 설정:
python
코드 복사
smtp_server = 'smtp.gmail.com'
smtp_port = 587
sender_email = 'your_email@gmail.com'  # 발신자 이메일
sender_password = 'your_app_password'  # Google 앱 비밀번호
Gmail 앱 비밀번호를 사용하여 이메일 전송 시 보안 경고가 발생하지 않도록 주의하세요.

설치 및 실행 방법
프로젝트를 로컬로 클론합니다:
bash
코드 복사
git clone https://github.com/your-repo/nfc-pos-system.git
Arduino IDE에서 NFC 카드에 정보를 쓰는 코드를 실행합니다.
Python 환경에서 POS 시스템을 실행합니다:
bash
코드 복사
python main.py
결제 금액을 입력하고, NFC 태그를 통해 이메일 주소를 읽은 후, 전자 영수증을 전송합니다.
주의 사항
Arduino와 RC522 모듈의 핀 연결이 정확한지 확인하세요.
COM 포트를 올바르게 설정해야 정상적으로 NFC 데이터를 읽을 수 있습니다.
Gmail 앱 비밀번호를 설정하고, 프로젝트에서 해당 비밀번호를 사용하세요.
