import socket 
import threading

def receive_messages():
    while True:
        try:
            message = client.recv(1024).decode()
            print(message)
        except:
            print("[DISCONNECTED]")
            client.close()
            break

HOST = '127.0.0.1'
PORT = 8000

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST, PORT))

approval_msg = client.recv(1024).decode()
print(approval_msg)

if 'denied' in approval_msg.lower():
    print('Connection denied')
else:
    threading.Thread(target=receive_messages, daemon=True).start()
    print("CHAT STARTED")
    while True:
        msg = input()
        if msg.lower() == 'exit':
            break
        client.send(msg.encode())