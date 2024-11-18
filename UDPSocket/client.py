import socket
import threading

def receive_messages():
    while True:
        try:
            msg, addr = client.recvfrom(1024)  
            print(msg.decode())
        except:
            print("[DISCONNECTED]")
            break

HOST = '127.0.0.1'
PORT = 5000

client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  

threading.Thread(target=receive_messages, daemon=True).start()

print("[CHAT STARTED]")
while True:
    msg = input()
    if msg.lower() == 'exit':
        break
    client.sendto(msg.encode(), (HOST, PORT))  
