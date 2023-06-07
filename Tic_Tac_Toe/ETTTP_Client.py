

import re
import random
import tkinter as tk
from socket import *
import _thread

from ETTTP_TicTacToe import TTT, check_msg
    


if __name__ == '__main__':

    SERVER_IP = '127.0.0.1' #서버 IP 주소
    MY_IP = '127.0.0.1' #클라이언트 IP 주소
    SERVER_PORT = 12000 #서버 포트 번호
    SIZE = 1024 #버퍼 크기
    SERVER_ADDR = (SERVER_IP, SERVER_PORT) #서버 주소

    
    with socket(AF_INET, SOCK_STREAM) as client_socket: #소켓 생성
        client_socket.connect(SERVER_ADDR)  #서버에 연결
        
        ###################################################################
        # Receive who will start first from the server
        start_msg = client_socket.recv(SIZE).decode() #서버로부터 메시지 읽기
        print(start_msg)
        split_msg = start_msg.split("\r\n") #메시지 \r\n 기준으로 쪼개기
        s_player = split_msg[2].split(":")[1].strip() #메시지 쪼개서 First-Mover를 s_player에 저장
        
        msg_info = check_msg(start_msg, MY_IP)
        if msg_info == False:
            client_socket.close()
            TTT.quit()
        
        if s_player == "ME": #First-Mover가 나라면 start=0(client)
          start = 0
        else: # 상대방이라면 start=1(server)
          start = 1
        
        ######################### Fill Out ################################
        # Send ACK 
        ack_msg = f"ACK ETTTP/1.0\r\nHost:{MY_IP}\r\nFirst-Move:{s_player}\r\n\r\n"  #ack 메시지 form 정의
        client_socket.send(str(ack_msg).encode()) #소켓을 통해서 ack 보내기
        
        ###################################################################
        
        # Start game
        root = TTT(target_socket=client_socket, src_addr=MY_IP,dst_addr=SERVER_IP)
        root.play(start_user=start)
        root.mainloop()
        client_socket.close() #연결 소켓 닫기