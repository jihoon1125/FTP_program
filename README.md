# FTP_program
Linux 환경에서의 서버와 클라이언트간의 파일전송 시스템 구현을 통해 FTP 통신방식의 원리를 이해한다.

---

## FTP commands

* NLST
    * -X : 현재 디렉토리에 존재하는 파일/디렉토리 출력
    * -a : 현재 디렉토리에 존재하는 모든 파일/디렉토리(숨김파일 포함) 출력
    * -l : 현재 디렉토리에 존재하는 파일/디렉토리들의 상세정보 출력
    * -al : 현재 디렉토리에 존재하는 모든 파일/디렉토리(숨김파일 포함)들의 상세정보 출력
* LIST : NLST -al 동작과 동일
* PWD : 현재 동작하는 디렉토리 출력
* CWD : 디렉토리 이동
* CDUP : 이전 디렉토리로 이동
* MKD : 디렉토리 생성
* DELE : 파일 삭제
* RMD : 디렉토리 삭제
* RNFR & RNTO : Old name에서 New name으로 이름 변경
* QUIT : Child process 종료
* PORT : 데이터 포트 오픈
* RETR : 원격 파일 가져오기
* STOR : 원격 호스트에 파일 저장하기
* TYPE : 전송 형식 수정
* USER : username 서버에 전송
* PASS : password 서버에 전송

---

## Flow chart

![image](https://user-images.githubusercontent.com/67624104/118274196-ca61cf80-b4ff-11eb-8818-fce7a3bc92eb.png)

  * Client
    1. Socket()을 통해 control, data socket 생성
    2. Connect()을 통해 server와 연결(control connection 형성)및 data socket 바인드(data connection 대기)
    3. Login, user authentication 진행
    4. Permission 통과 시 command를 입력받으면 server command로 변환 후 전송
    5. Ls, get, put command면 port command 서버에 전송
    6. Server로부터 data connection 요청 수락
    7. Server로부터 command에 부합하는 겨로가와 메시지를 지속적으로 수신

  * Server
    1. Socket()을 통해 control, data socket 생성
    2. Bind, listen, accept 과정을 거친다
    3. Client로부터 id, password를 전달받아서 authentication 절차 수행
    4. User로부터 port 명령어 수신했을 경우 data connection 요청을 client에게 시도
    5. Client로부터 전달받은 명령들을 cmd_process 함수를 통해 수행한 결과를 결과 버퍼에 저장 후 client에 전송


---

## Result example
![image](https://user-images.githubusercontent.com/67624104/118275146-034e7400-b501-11eb-9b31-d9c0fa24b457.png)


---

