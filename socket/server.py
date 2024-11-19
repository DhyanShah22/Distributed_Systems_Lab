import socket 

def main():
    HOST = '127.0.0.1'
    PORT = int(input("Enter the port to bind server: "))
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()
    print(f"[SERVER] listening at {HOST} : {PORT}")

    while True:
        client, addr = server.accept()
        print(f"[REQUEST] to connect from {addr}")
        approve = input(f"Approve connection from {addr}? [y/n]").strip().lower()
        
        if approve == 'y':
            client.send("[SERVER]: Connection accepted.".encode())
            print(f"[CONNECTION] from {addr} accepted")

            while True:
                msg = client.recv(1024).decode()
                print(msg)
                if msg.lower() == 'bye':
                    client.send("Bye".encode())
                    break
                reply = msg.upper()
                client.send(reply.encode())
            
            print(f"[DISSCONNECTED]: CLIENT CLOSES")
            client.close()
        else:
            client.send("[SERVER]: Connection denied".encode())
            client.close
            print(f"[DECLINED] connection from {addr}")

if __name__ == '__main__':
    main()
