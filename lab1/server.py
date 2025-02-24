import socket
import time

timeout = 15

HOST = "127.0.0.1"
PORT = 65432


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen()
    s.settimeout(timeout)

    try:
        conn, addr = s.accept()
        conn.settimeout(timeout)

        with conn:
            print(f"Connected by {addr}")
            start_time = time.time()
            
            while True:
                try:
                    data = conn.recv(1024)
                    if data:
                        start_time = time.time()

                        msg = data.decode('utf-8')
                        print(f"Get message: {msg}")
                        
                        if msg == 'ping':
                            conn.sendall(b'pong')
                        elif msg == 'stop':
                            conn.sendall(b'Finish server')
                            break
                        elif msg == 'Tell me about you':
                            conn.sendall(b'You can send "ping" or "stop"')
                        else:
                            conn.sendall(bytes(f"Wrong message = {msg}. Please, send 'ping' or 'stop'", 'utf-8'))
                    else:
                        print("Client closed the connection")
                        break
                except socket.timeout:
                    print('Client timeout')
                    conn.sendall(b'Client timeout')
                    break
                    
    except socket.timeout:
        print("Server timeout: No client connected within the timeout period.")