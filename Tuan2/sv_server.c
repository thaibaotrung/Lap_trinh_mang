#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

typedef struct sinhVien {
    char mssv[100];
    char hoTen[100];
    char ngaySinh[100];
    float diemTb;
} SinhVien;

int main(int argc, char *argv[]) {
    
    int port = atoi(argv[1]);
    char *fileName = argv[2];


    // Tao socket
    int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listener == -1) {
        printf("socket faild!\n");
        return 1;
    }

    // Khai bao cau truc dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // Gan dia chi voi socket
    if(bind(listener, (struct sockaddr*)&addr, sizeof(addr))){
        printf("bind faild\n");
        return 1;
    }

    // Chuyen socket sang trang thai cho ket noi
    if(listen(listener, 5)) {
        printf("listen faild\n");
        return 1;
    }

    printf("Waiting for a new client ...\n\n");


    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    
    while(1) {
        // Chap nhan ket noi
        int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
        printf("Client IP: %s:%d\n", inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));
        char *ip = inet_ntoa(clientAddr.sin_addr);

    
        // Nhan du lieu va ghi vao file
        SinhVien sv;   

        time_t now = time(0);
        struct tm *local_time = localtime(&now);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

        int ret = recv(client, &sv, sizeof(sv), 0);
        if(ret < 0) break;

        printf("MSSV: %s\n", sv.mssv);
        printf("Họ và tên: %s\n", sv.hoTen);
        printf("Ngày sinh: %s\n", sv.ngaySinh);
        printf("Điểm TB: %.2f\n\n", sv.diemTb);


        FILE *recv_file = fopen(fileName, "a");

        fprintf(recv_file, "%s %s %s %s %s %.2f\n", 
            ip, time_str, sv.mssv, sv.hoTen, sv.ngaySinh, sv.diemTb);
        
        fclose(recv_file);
        close(client);
    }

    close(listener);
    return 0;

}