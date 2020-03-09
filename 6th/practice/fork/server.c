#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX 1024
#define PORT 3000
#define BACKLOG 5

int main(int argc, char *argv[]){
   int sd, nsd, pid, bytes, cliaddrsize;

   struct sockaddr_in cliaddr, servaddr;
   char data[MAX];

   // int socket(int domain, int type, int protocol);
   // 소켓을 생성하여 반환
   // domain: 인터넷을 통해 통신할지, 같은 시스템 내에서 프로세스끼리 통신할지
   // - PF_INET, AF_INET: IPv4 인터넷 프로토콜 사용
   // - PF_INET6: IPv6 인터넷 프로토콜 사용
   // - PF_LOCAL, AF_UNIX 같은 시스템 내에서 프로세스끼리 통신
   // type: 데이터의 전송 형태를 지정
   // - SOCK_STREAM: TCP/IP 프로토콜 이용
   // - SOCK_DGRAM: UDP/IP 프로토콜 이용
   // protocol: 통신에 있어 특정 프로토콜 사용을 지정하기 위한 변수
   // 보통 0을 사용
   if((sd= socket(AF_INET, SOCK_STREAM, 0)) < 0){
      fprintf(stderr, "can't open socket.\n");
      exit(1);
   }

   // servaddr 구조체의 전체 크기를 0으로 채운다.(초기화)
   bzero ((char*)&servaddr, sizeof(servaddr));

   servaddr.sin_family = AF_INET;   // IPv4
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_ANY: 서버의 IP주소를 자동으로 찾아서 대입
   servaddr.sin_port = htons(PORT);

   // int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
   // 소켓에 IP주소와 포트번호를 지정. 이로서 소켓을 통신에 사용할 수 있도록 준비가 됨.
   if(bind(sd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
      fprintf(stderr, "can't bind to socket.\n");
      exit(1);
   }

   // int listen(int s, int backlog);
   // 소켓을 통해 클라이언트의 접속 요청을 기다리도록 설정.
   // s: 소켓 디스크립터
   // backlog: 대기 메시지 큐의 개수
   listen(sd, BACKLOG);

   while(1){
      cliaddrsize = sizeof(cliaddr);

      // int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
      // 클라이언트의 접속 요청을 받아들이고 클라이언트와 통신하는 전용 소켓 생성.
      // s: 소켓 디스크립터
      // *addr: 클라이언트 주소 정보를 가지고 있는 포인터
      // *addrlen: *addr 포인터가 가르키는 구조체의 크기
      if((nsd = accept(sd, (struct sockaddr *)&cliaddr, &cliaddrsize)) < 0){
         fprintf(stderr, "can't accept connection.\n");
         exit(1);
      }
      if((pid = fork()) < 0){
         fprintf(stderr,"can't fork process.\n");
         exit(1);
      }
      if(pid == 0){
         close(sd);
         while(1){
            // int recv(int s, void *buf, size_t len, int flags);
            // 소켓으로부터 데이터 수신 (return: 실제 수신한 바이트 수, -1)
            // s: 소켓 디스크립터
            // *buf: 수신할 버퍼 포인터 데이터
            // len: 버퍼의 바이트 단위 크기
            // flags: 아래 옵션 중 선택
            // - MSG_DONTWAIT: 수신을 위해 대기가 필요하다면 기다리지 않고 -1을 return하면서 바로 복귀
            // - MSG_NOSIGNAL: 상대방과 연결이 끊겼을 때, SIGPIPE 시그널을 받지 않도록 함.
            bytes = recv(nsd, data, MAX, 0);
            if(bytes == 0)
               break;
            else if(bytes < 0){
               fprintf(stderr, "can't receive data.\n");
               exit(1);
            }

            // int send(int s, const void* msg, size_t len, int flags);
            // 연결된 서버나 클라이언트로 데이터를 전송(return: 실제 전송한 바이트 수, -1)
            // s: 소켓 디스크립터
            // *msg: 전송할 데이터
            // len: 데이터의 바이트 단위 길이
            // flags: 아래 옵션 중 선택
            // - MSG_DONTWAIT: 전송 전에 대기가 필요하다면 기다리지 않고 -1을 return하면서 바로 복귀
            // - MSG_NOSIGNAL: 상대방과 연결이 끊겼을 때, SIGPIPE 시그널을 받지 않도록 함.
            if(send(nsd, data, bytes, 0) != bytes){
               fprintf(stderr, "can't send data.\n");
               exit(1);
            }
         }
         return 0;
      }
      close(nsd);
   }
}
