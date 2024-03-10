import cv2
import mediapipe as mp
import paho.mqtt.client as mqtt
import time

# MQTT setup
mqtt_client = mqtt.Client()
mqtt_client.connect("mqtt.eclipseprojects.io", 1883, 60)
topic = "your/topic"

# Mediapipe setup
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        continue
    frame = cv2.flip(frame, 1)

    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(frame_rgb)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Index finger landmarks
            index_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            index_dip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_DIP]
            index_pip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_PIP]
            index_mcp = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_MCP]

            # Check the state of the index finger
            if index_tip.y > index_dip.y and index_dip.y > index_pip.y:  # Fully closed
                finger_state = "1"
            elif index_tip.y > index_pip.y:  # Half open/extended
                finger_state = "2"
            else:  # Fully open
                finger_state = "3"

            # Send the state over MQTT
            mqtt_client.publish(topic, finger_state)
            time.sleep(1)  # Prevent sending too many messages

            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

        cv2.putText(frame, f'Index Finger State: {finger_state}', (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
    cv2.imshow('Hand Tracking', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
