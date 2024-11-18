import socket
import threading

clients = set()

def handle_client(data, addr):
    msg = f"Client {addr}: {data.decode()}"
    print(msg)
    broadcast_message(msg, addr)

def broadcast_message(msg, sender_addr):
    for client in clients:
        if client != sender_addr:
            server.sendto(msg.encode(), client)

def main():
    HOST = '127.0.0.1'
    PORT = 5000

    global server
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind((HOST, PORT))
    print(f"Server listening on {HOST}:{PORT}")

    while True:
        try:
            data, addr = server.recvfrom(1024)
            if addr not in clients:
                clients.add(addr)
                print(f"New client connected: {addr}")
                threading.Thread(target=handle_client, args=(data, addr), daemon=True).start()
        except Exception as e:
            print(f"[ERROR] {e}")

if __name__ == '__main__':
    main()
    