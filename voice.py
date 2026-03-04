import serial
import pyttsx3
import time

engine = pyttsx3.init()
engine.setProperty('rate', 160)

try:
    ser = serial.Serial('COM9', 115200, timeout=1)
    time.sleep(2)
    print("✅ Connected to ESP8266 on COM9")
except:
    print("❌ Could not connect to COM9. Check your port!")
    exit()

engine.say("Voice alert system ready.")
engine.runAndWait()

print("🔄 Listening for sensor values and alerts...")

while True:
    try:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            print("Received:", line)

            # Check if it's a VoiceAlert line
            if line.startswith("VoiceAlert:"):
                msg = line.replace("VoiceAlert:", "").strip()
                engine.say(msg)
                engine.runAndWait()

            # Check if it's sensor values
            elif "Temp:" in line and "Humidity:" in line and "Gas Value:" in line:
                # Example: Temp: 27.5 C, Humidity: 65 %, Gas Value: 345
                try:
                    parts = line.split(',')
                    temp = parts[0].split(":")[1].strip()
                    humidity = parts[1].split(":")[1].strip()
                    gas = parts[2].split(":")[1].strip()

                    message = f"Temperature is {temp} degrees Celsius. Humidity is {humidity}. Gas value is {gas}."
                    engine.say(message)
                    engine.runAndWait()
                except:
                    print("⚠️ Could not parse sensor values.")

    except KeyboardInterrupt:
        print("\n🔴 Exiting...")
        break
    except Exception as e:
        print("⚠️ Error:", e)
