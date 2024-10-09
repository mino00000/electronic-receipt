import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import serial
import tkinter as tk
from tkinter import messagebox
from datetime import datetime
import logging

# 로그 설정 (CMD에 로그 출력)
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')

# 시리얼 포트 설정 (아두이노가 연결된 포트로 변경 필요)
ser = serial.Serial('COM12', 9600)  # 아두이노와의 시리얼 통신 설정

# POS 프로그램 기본 창 생성
root = tk.Tk()
root.title("POS 시스템")
root.geometry("800x500")

# 결제 금액 입력창
amount_label = tk.Label(root, text="결제 금액:")
amount_label.pack(pady=10)
amount_entry = tk.Entry(root)
amount_entry.pack(pady=5)

# 오류 상태를 추적하는 플래그
error_shown = False

# 이메일 전송 함수 (영수증 형식으로 작성)
def send_email(receiver_email, subject, amount):
    try:
        # SMTP 서버 설정
        smtp_server = 'smtp.gmail.com'
        smtp_port = 587
        sender_email = ''  # 발신자 Gmail 이메일 주소
        sender_password = ''  # Gmail 앱 비밀번호 (16자리)

        # 현재 결제 일자 가져오기
        payment_date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # 이메일 메시지 작성 (HTML 형식의 영수증)
        msg = MIMEMultipart()
        msg['From'] = sender_email
        msg['To'] = receiver_email
        msg['Subject'] = subject

        # HTML 형식의 영수증 내용
        html_content = f"""
        <html>
        <head></head>
        <body>
            <h2>전자 영수증</h2>
            <p>안녕하세요, 아래는 결제 내역입니다.</p>
            <table border="1" cellpadding="10" cellspacing="0" width="100%">
                <tr>
                    <th>항목</th>
                    <th>내용</th>
                </tr>
                <tr>
                    <td>결제 금액</td>
                    <td>{amount}원</td>
                </tr>
                <tr>
                    <td>결제 일자</td>
                    <td>{payment_date}</td>
                </tr>
            </table>
            <p>감사합니다!</p>
            <hr />
            <p>이메일로 발송된 영수증입니다. <br> 문제나 문의사항이 있으시면 연락주세요.</p>
        </body>
        </html>
        """

        # 이메일 본문에 HTML 내용 첨부
        msg.attach(MIMEText(html_content, 'html'))

        # SMTP 서버에 연결하고 이메일 전송
        server = smtplib.SMTP(smtp_server, smtp_port)
        server.starttls()  # TLS (보안 연결) 시작
        server.login(sender_email, sender_password)  # 이메일 로그인
        text = msg.as_string()
        server.sendmail(sender_email, receiver_email, text)  # 이메일 전송
        server.quit()

        logging.info(f"이메일 전송 완료: {receiver_email}로 성공적으로 전송되었습니다.")
        messagebox.showinfo("이메일 전송 완료", f"{receiver_email}로 이메일을 성공적으로 전송했습니다.")

    except Exception as e:
        logging.error(f"이메일 전송 오류: {e}")
        messagebox.showerror("이메일 전송 오류", f"이메일 전송에 실패했습니다: {e}")

# NFC 데이터 읽기 함수 (태그 감지 후 자동 처리)
def check_nfc_tag():
    global error_shown
    if ser.in_waiting > 0:  # 시리얼 포트에 데이터가 들어오면
        email = ser.readline().decode('utf-8').strip()
        if email and "@" in email:  # 이메일이 공백이 아니고 '@'를 포함할 때 정상 처리
            logging.info(f"NFC 데이터 감지: {email}")
            messagebox.showinfo("결제 완료", f"NFC 데이터(이메일): {email}")
            receipt_window.destroy()  # 영수증 창 닫기
            error_shown = False  # 오류 상태 리셋
            
            # 이메일 전송 (영수증 형식)
            send_email(email, '영수증', amount_entry.get())
        else:
            if not error_shown:  # 이미 오류 메시지가 표시된 상태가 아니라면
                logging.warning("NFC 태그에서 올바른 이메일을 읽지 못했습니다.")
                messagebox.showerror("오류", "NFC 태그에서 올바른 이메일을 읽지 못했습니다.")
                error_shown = True  # 오류가 표시되었음을 기록
    root.after(100, check_nfc_tag)  # 100ms 후 다시 호출하여 반복 실행

# 영수증 선택 함수
def select_receipt_method():
    global receipt_window
    receipt_window = tk.Toplevel(root)
    receipt_window.title("영수증 발행")
    receipt_window.geometry("300x200")
    
    label = tk.Label(receipt_window, text="영수증 발행 방법을 선택하세요:")
    label.pack(pady=10)
    
    paper_button = tk.Button(receipt_window, text="종이 영수증", command=lambda: print_receipt("종이"))
    paper_button.pack(pady=5)
    
    electronic_button = tk.Button(receipt_window, text="전자 영수증", command=lambda: print_receipt("전자"))
    electronic_button.pack(pady=5)
    
    both_button = tk.Button(receipt_window, text="둘 다", command=lambda: print_receipt("종이 및 전자"))
    both_button.pack(pady=5)

# 영수증 발행 후 NFC 태그를 기다리는 함수
def print_receipt(method):
    logging.info(f"영수증 발행 완료: {method} 선택됨. NFC 카드를 태그하세요.")
    messagebox.showinfo("영수증 발행 완료", f"{method} 발행 완료.\nNFC 카드를 태그하세요.")
    check_nfc_tag()  # NFC 태그 대기 시작

# 결제 완료 함수
def complete_payment():
    amount = amount_entry.get()
    if amount:
        logging.info(f"결제 완료: 금액 {amount}원")
        messagebox.showinfo("결제 완료", f"결제 금액: {amount}원")
        select_receipt_method()  # 영수증 선택 창 열기
    else:
        logging.warning("결제 금액 입력 안됨.")
        messagebox.showwarning("경고", "금액을 입력하세요.")

# 결제 버튼
pay_button = tk.Button(root, text="결제 완료", command=complete_payment)
pay_button.pack(pady=20)

# 프로그램 실행
root.mainloop()
