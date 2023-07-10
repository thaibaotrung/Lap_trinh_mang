#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

int main() {
   int clientSocket;
   struct sockaddr_in serverAddr;
   char buffer[BUFFER_SIZE];

   // Tạo socket
   clientSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (clientSocket < 0) {
      perror("Không thể tạo socket");
      exit(EXIT_FAILURE);
   }

   // Cấu hình địa chỉ server
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(8888);
   serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

   // Kết nối đến server
   if (connect(clientSocket, (struct sockaddr*)&serverAddr,
               sizeof(serverAddr)) < 0) {
      perror("Không thể kết nối đến server");
      exit(EXIT_FAILURE);
   }

   // Gửi và nhận tin nhắn
   while (1) {
      printf("Client: ");
      fgets(buffer, sizeof(buffer), stdin);

      // Gửi tin nhắn cho server
      if (send(clientSocket, buffer, strlen(buffer), 0) < 0) {
         perror("Lỗi khi gửi tin nhắn");
         exit(EXIT_FAILURE);
      }

      // Nhận tin nhắn từ server
      memset(buffer, 0, sizeof(buffer));
      if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0) {
         perror("Lỗi khi nhận tin nhắn");
         exit(EXIT_FAILURE);
      }

      printf("Server: %s", buffer);
   }

   // Đóng kết nối
   close(clientSocket);

   return 0;
}
