import socket 
import threading

clients = {}

def handle_client(client, addr):
    client_id = f"Client: {len(clients) + 1}"
    clients[client] = client_id
    print(f"[New connection] {client_id} from {addr}")

    try:
        while True:
            msg = client.recv(1024).decode()
            if not msg:
                break
            log_msg = f"[{client_id}]: {msg}"
            print(log_msg)
            broadcast_message(log_msg, sender=client)
    except:
        print(f"DISCONNECTED {client_id}")
    finally:
        clients.pop(client, None)
        client.close()
    
def broadcast_message(message, sender):
    for client in clients:
        if client != sender:
            try:
                client.send(message.encode())
            except:
                print("[ERROR] Failed to send message to a client.")


def main():
    HOST = '127.0.0.1'
    PORT = 8000
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()

    while True:
        client, addr = server.accept()
        print(f"[CONNECTTION REQUEST] from {addr}")

        accept = input(f"Accept connection from {addr} [y/n]? ").strip().lower()
        if accept == 'y':
            client.send("[SERVER]: connection accepted".encode())
            threading.Thread(target=handle_client, args=(client, addr), daemon=True).start()
        else:
            client.send("[SERVER]: Connection declined".encode())
            client.close()
            print(f"[DECLINED] Connection from {addr}")

if __name__ == '__main__':
    main()