# import socket
# import threading

# clients = {}  

# def handle_client(client, addr):
#     client_id = f"Client {len(clients) + 1}"
#     clients[client] = client_id
#     print(f"[NEW CONNECTION] {client_id} from {addr}")

#     try:
#         while True:
#             msg = client.recv(1024).decode()
#             if msg:
#                 log_msg = f"{client_id}: {msg}"
#                 print(log_msg)
#                 broadcast(log_msg, sender=client)
#     except:
#         print(f"[DISCONNECTED] {client_id}")
#     finally:
#         clients.pop(client, None)
#         client.close()

# def broadcast(msg, sender):
#     for client in clients:
#         if client != sender:
#             client.send(msg.encode())

# def main():
#     HOST, PORT = "127.0.0.1", 5000
#     server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#     server.bind((HOST, PORT))
#     server.listen()
#     print(f"[SERVER] Listening on {HOST}:{PORT}")

#     while True:
#         client, addr = server.accept()
#         threading.Thread(target=handle_client, args=(client, addr), daemon=True).start()

# if __name__ == "__main__":
#     main()

import socket
import threading

clients = {}

def handle_client(client, addr):
    client_id = f"Client {len(clients) + 1}"
    clients[client]= client_id
    print(f"[NEW CONNECTION] {client_id} from {addr}")

    try:
        while True:
            msg = client.recv(1024).decode()
            log_msg = f"{client_id}: {msg}"
            print(log_msg)
            broadcast_message(log_msg, sender=client)
    except:
        print(f"[Disconnected] {client_id}")
    finally:
        clients.pop(client, None)
        client.close()

def broadcast_message(msg, sender):
    for client in clients:
        if(client != sender):
            client.send(msg.encode())
    
def main():
    HOST = '127.0.0.1'
    PORT = 5000
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()

    print(f"Server listening on {HOST}:{PORT}")

    while True:
        client, addr = server.accept()
        threading.Thread(target=handle_client, args=(client, addr), daemon=True).start()

if __name__ == '__main__':
    main()

