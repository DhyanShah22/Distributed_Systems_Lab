import socket
import threading

def receive_messages():
    while True:
        try:
            msg = client.recv(1024).decode()
            print(msg)
        except:
            print("[DISCONNECTED]")
            client.close()
            break
    
HOST = '127.0.0.1'
PORT = 5000

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST, PORT))

threading.Thread(target=receive_messages).start()

print("[CHAT STARTED]")
while True:
    msg = input()
    if msg.lower() == 'exit':
        break
    client.send(msg.encode())  