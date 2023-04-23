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
    
    char *ipAddr = argv[1];
    int port = atoi(argv[2]);

    // Tao socket
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    // Khai bao dia chi server
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipAddr);
    addr.sin_port = htons(port);

    // Ket noi den server
    int ret = connect(client, (struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("Kết nối thất bại");
        exit(1);
    }


    // Nhap va gui du lieu
    SinhVien sv;
    printf("Nhập dữ liệu: \n");

    printf("MSSV: ");
    fgets(sv.mssv, sizeof(sv.mssv), stdin);
    sv.mssv[strcspn(sv.mssv, "\n")] = '\0';

    printf("Họ và tên: ");
    fgets(sv.hoTen, sizeof(sv.hoTen), stdin);
    sv.hoTen[strcspn(sv.hoTen, "\n")] = '\0';

    printf("Ngày sinh: ");
    fgets(sv.ngaySinh, sizeof(sv.ngaySinh), stdin);
    sv.ngaySinh[strcspn(sv.ngaySinh, "\n")] = '\0';

    printf("Điểm TB: ");
    scanf("%f", &sv.diemTb);

    send(client, &sv, sizeof(SinhVien), 0);

    printf("Đã gửi dữ liệu lên server.\n");

    close(client);
    return 0;
    
}