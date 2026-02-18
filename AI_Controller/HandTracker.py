import cv2
import mediapipe as mp
import socket

# setup mediapipe
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode = False, max_num_hands=1, min_detection_confidence=0.7)

# setup udp socket - which will help to communicate with unreal
UDP_IP = "127.0.0.1"
UDP_PORT = 8888
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

cap = cv2.VideoCapture(0)

while cap.isOpened():
    success, image = cap.read()
    if not success: continue

    # Flip image for a mirror effect, then process
    image = cv2.cvtColor(cv2.flip(image, 1), cv2.COLOR_BGR2RGB)
    results = hands.process(image)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Get the Index Finger Tip (Landmark 8)
            index_tip = hand_landmarks.landmark[8]
            
            # COORDINATE MAPPING:
            # Convert MediaPipe (0-1) to Unreal World Units (e.g., -200 to 200)
            # We flip the Y/Z axis here so Palm Down moves the Claw Down
            ue_x = (index_tip.z * 100)  # Depth
            ue_y = (index_tip.x - 0.5) * 400 
            ue_z = (1.0 - index_tip.y) * 300 # Flipped Y to Z for height
            
            # Send data as a string "x,y,z"
            data = f"{ue_x},{ue_y},{ue_z}"
            sock.sendto(data.encode(), (UDP_IP, UDP_PORT))

    cv2.imshow('AI Robot Controller', image)
    if cv2.waitKey(5) & 0xFF == 27: break

cap.release()