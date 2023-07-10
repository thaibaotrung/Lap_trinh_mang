#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

int main() {
   int serverSocket, clientSockets[MAX_CLIENTS];
   struct sockaddr_in serverAddr, clientAddr;
   socklen_t clientAddrLen;
   char buffer[BUFFER_SIZE];
   int i;

   // Tạo socket
   serverSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (serverSocket < 0) {
      perror("Không thể tạo socket");
      exit(EXIT_FAILURE);
   }

   // Cấu hình địa chỉ server
   serverAddr.sin_family = AF_INET;
   serverAddr.sin_port = htons(8888);
   serverAddr.sin_addr.s_addr = INADDR_ANY;

   // Gắn địa chỉ vào socket
   if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) <
       0) {
      perror("Lỗi khi gắn địa chỉ vào socket");
      exit(EXIT_FAILURE);
   }

   // Lắng nghe kết nối từ client
   if (listen(serverSocket, MAX_CLIENTS) < 0) {
      perror("Lỗi khi lắng nghe");
      exit(EXIT_FAILURE);
   }

   printf("Đang chờ kết nối từ client...\n");

   // Khởi tạo các client sockets
   for (i = 0; i < MAX_CLIENTS; i++) {
      clientSockets[i] = 0;
   }

   // Chấp nhận kết nối từ client
   while (1) {
      fd_set readfds;
      int maxFd;

      FD_ZERO(&readfds);
      FD_SET(serverSocket, &readfds);
      maxFd = serverSocket;

      // Thêm client sockets vào set
      for (i = 0; i < MAX_CLIENTS; i++) {
         int clientSocket = clientSockets[i];
         if (clientSocket > 0) {
            FD_SET(clientSocket, &readfds);
         }
         if (clientSocket > maxFd) {
            maxFd = clientSocket;
         }
      }

      // Theo dõi các socket sẵn sàng để đọc
      int activity = select(maxFd + 1, &readfds, NULL, NULL, NULL);
      if ((activity < 0) && (errno != EINTR)) {
         perror("Lỗi khi sử dụng select");
         exit(EXIT_FAILURE);
      }

      // Kết nối mới từ client
      if (FD_ISSET(serverSocket, &readfds)) {
         clientAddrLen = sizeof(clientAddr);
         clientSockets[i] = accept(serverSocket, (struct sockaddr*)&clientAddr,
                                   &clientAddrLen);
         if (clientSockets[i] < 0) {
            perror("Lỗi khi chấp nhận kết nối");
            exit(EXIT_FAILURE);
         }
         printf("Kết nối từ client %s:%d\n", inet_ntoa(clientAddr.sin_addr),
                ntohs(clientAddr.sin_port));
      }

      // Xử lý tin nhắn từ client
      for (i = 0; i < MAX_CLIENTS; i++) {
         int clientSocket = clientSockets[i];
         if (FD_ISSET(clientSocket, &readfds)) {
            memset(buffer, 0, sizeof(buffer));

            // Nhận tin nhắn từ client
            int valread = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (valread == 0) {
               // Kết nối đã đóng
               getpeername(clientSocket, (struct sockaddr*)&clientAddr,
                           &clientAddrLen);
               printf("Client %s:%d đã đóng kết nối\n",
                      inet_ntoa(clientAddr.sin_addr),
                      ntohs(clientAddr.sin_port));
               close(clientSocket);
               clientSockets[i] = 0;
            } else {
               // Kiểm tra và kick người dùng ra khỏi phòng chat
               if (strstr(buffer, "/kick") != NULL) {
                  char* username =
                      strtok(buffer, " ");  // Lấy tên người dùng từ tin nhắn
                  if (username != NULL) {
                     if (strstr(username, "/kick") == NULL) {
                        int j;
                        for (j = 0; j < MAX_CLIENTS; j++) {
                           if (clientSockets[j] != clientSocket &&
                               clientSockets[j] != 0) {
                              send(clientSockets[j],
                                   "Đã kick người dùng ra khỏi phòng chat\n",
                                   strlen("Đã kick người dùng ra khỏi phòng "
                                          "chat\n"),
                                   0);
                           }
                        }
                        // Đóng kết nối với người dùng bị kick
                        getpeername(clientSocket, (struct sockaddr*)&clientAddr,
                                    &clientAddrLen);
                        printf("Người dùng %s đã bị kick khỏi phòng chat\n",
                               username);
                        close(clientSocket);
                        clientSockets[i] = 0;
                     }
                  }
               } else {
                  // Gửi tin nhắn cho các client khác
                  int j;
                  for (j = 0; j < MAX_CLIENTS; j++) {
                     if (clientSockets[j] != clientSocket &&
                         clientSockets[j] != 0) {
                        send(clientSockets[j], buffer, strlen(buffer), 0);
                     }
                  }
               }
            }
         }
      }
   }

   // Đóng kết nối
   for (i = 0; i < MAX_CLIENTS; i++) {
      if (clientSockets[i] != 0) {
         close(clientSockets[i]);
      }
   }
   close(serverSocket);

   return 0;
}
