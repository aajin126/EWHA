

import re
import random
import tkinter as tk
from socket import *
import _thread

from ETTTP_TicTacToe import TTT, check_msg

    
if __name__ == '__main__':
    
    global send_header, recv_header
    SERVER_PORT = 12000 #서버 포트 번호
    SIZE = 1024 #버퍼 크기
    server_socket = socket(AF_INET,SOCK_STREAM) 
    server_socket.bind(('',SERVER_PORT))
    server_socket.listen()
    MY_IP = '127.0.0.1' #서버 IP 주소
    
    while True: #요청 항상 대기
        client_socket, client_addr = server_socket.accept() #특정 클라이언트와 서버를 소켓을 통해 연결
        
        start = random.randrange(0,2)  # select random to start(0,1 중 택 1)
        
        ###################################################################
        # Send start move information to peer
        if start % 2 == 0 : # 0이면 server가 start가 된다.
          print("start is server")
          s_player = "ME" #First-Mover를 ME로 표시
        else: # 1이면 client가 start가 된다. 
          print("start is client")
          s_player = "YOU" #First-Mover를 YOU로 표시
      
        start_msg = f"SEND ETTTP/1.0\r\nHost:{MY_IP}\r\nFirst-Move:{s_player}\r\n\r\n"
        client_socket.send(str(start_msg).encode()) #client한테 start 전송함
    
        ######################### Fill Out ################################
        # Receive ack - if ack is correct, start game
        ack = client_socket.recv(SIZE).decode() #ack을 수신함
        print(ack)
        ack_info = check_msg(ack, MY_IP)
        if ack_info == False:
            client_socket.close()
            TTT.quit()
        ###################################################################    
        print("ACK 정상 수신")    
        root = TTT(client=False, target_socket=client_socket, src_addr=MY_IP, dst_addr=client_addr[0])
        root.play(start_user=start)
        root.mainloop()
        
        client_socket.close()
        
        break
    server_socket.close()