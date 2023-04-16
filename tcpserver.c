#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {

    int port = atoi(argv[1]);
    char *filename1 = argv[2];
    char *filename2 = argv[3];

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

    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    
    // Chap nhan ket noi
    int client = accept(listener, (struct sockaddr *)&clientAddr, &clientAddrLen);
    printf("Client IP: %s:%d\n", inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));

    // Gui xau chao doc tu tep tin
    FILE *hello_file = fopen(filename1, "r");
    if(hello_file == NULL) {
        perror("error");
        exit(1);
    }
    char hello_buf[1024];
    size_t num_bytes = fread(hello_buf, 1, 1024, hello_file);
    fclose(hello_file);
    write(client, hello_buf, num_bytes);
 

    //Doc noi dung client gui den va ghi vao filename2
    FILE *recv_file = fopen(filename2, "wb");
    if(recv_file == NULL) {
        perror("error");
        exit(1);
    }

    char buf[1024];

    while (1) {
        int ret = read(client, buf, sizeof(buf));
        if(ret < 0) break;
        if(buf[0] != 'q' && ret != 1){
            fwrite(buf, 1, ret, recv_file); 
        } else break;
    }

    fclose(recv_file);
    printf("Đã nhận xong dữ liệu từ client.\n");

    // dong ket noi
    close(client);
    close(listener);

    return 0;
}