import cv2
import mediapipe as mp
import numpy as np
import socket
import json
import math

mp_hands = mp.solutions.hands.Hands(
    max_num_hands=2,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.7
)
mp_draw = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)

# calculate distance between two points
def distance(a,b):
    return math.sqrt ((a.x - b.x)**2 + (a.y - b.y)**2)

# Detect pinch
def Is_Pinch (hand_landmarks):
    thumb = hand_landmarks.landmark[4]
    index = hand_landmarks.landmark[8]

    return distance (thumb, index) < 0.04

# Detech fist
def Is_Fist (hand_landmarks):
    wrist = hand_landmarks.landmark[0]

    tips = [8,12,16,20] # index, middle, ring, pinky

    for tip_id in tips:
        if (distance(hand_landmarks.landmark[tip_id], wrist)) > 0.23:
            return False
    
    return True

# Get joystick vector
def get_joystick_vector (hand_x, hand_y, center, radius):
    dx = hand_x - center[0]
    dy = center[1] - hand_y
    distance = np.sqrt(dx * dx + dy * dy)

    if (distance < radius):
        # Normalize joystick values (-1 to 1)
        norm_x = dx / radius
        norm_y = dy / radius

    roundDx = round(dx/radius,2)
    roundDY = round(dy/radius,2)

    clipDX = np.clip (roundDx, -1, 1)
    clipDY = np.clip (roundDY, -1, 1)

    return (clipDX, clipDY)

# Create UDP Socket
UDP_IP = "127.0.0.1"  # Same pc
UDP_PORT = 12345

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while cap.isOpened():
    success, frame = cap.read()
    if not success:
        break

    frame = cv2.flip(frame, 1)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = mp_hands.process(rgb)

    # getting height and width of camera screen
    h, w, _ = frame.shape

    right_dx, right_dy = 0.0, 0.0

    # Right joystick (aim) calculating center and radius of right joystick
    RIGHT_CENTER = (int(w * 0.75), int(h * 0.7))
    RIGHT_RADIUS = 120

    # Draw joystick circles
    cv2.circle(frame, RIGHT_CENTER, RIGHT_RADIUS, (0, 0, 255), 2)

    # Draw center dots
    cv2.circle(frame, RIGHT_CENTER, 5, (0, 0, 255), -1)

    #jump = 0
    grab = 0

    if results.multi_hand_landmarks and results.multi_handedness:
        for hand_landmarks, handedness in zip(
            results.multi_hand_landmarks,
            results.multi_handedness
        ):
            hand_label = handedness.classification[0].label
   
            # checking for grab
            if hand_label == "Right":
                if Is_Fist (hand_landmarks):
                    grab = 1

            mp_draw.draw_landmarks(
                frame,
                hand_landmarks,
                mp.solutions.hands.HAND_CONNECTIONS
            )

            # use palm center (landmark 9 is stable)
            lm = hand_landmarks.landmark[9]
            hand_X = int (lm.x * w)
            hand_Y = int (lm.y * h)

            cx, cy = RIGHT_CENTER
            radius = RIGHT_RADIUS
            right_dx, right_dy = get_joystick_vector (hand_X, hand_Y,RIGHT_CENTER, RIGHT_RADIUS)
          
            # Visual feedback
            cv2.line(frame, (cx, cy), (hand_X, hand_Y), (255, 0, 0), 3)
            cv2.circle(frame, (hand_X, hand_Y), 10, (255, 0, 0), -1)

            printTxt = ""
            printTxt = hand_label + " " + str(right_dx) + ", " + str(right_dy) + ", " + str (0) + ", " + str(grab)

            cv2.putText(
                frame,
                printTxt,
                (10, 40 ),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (0, 255, 0),
                2
            )

    # making data for json
    data = {
        "move_x" : float (right_dx),
        "move_y" : float (right_dy),
        "move_z" : float (0),
        "grab" : int (grab),
    }

    sock.sendto(json.dumps(data).encode(), (UDP_IP, UDP_PORT))
    # print (data)

    cv2.imshow("Hand Split", frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break


cap.release()
cv2.destroyAllWindows()
