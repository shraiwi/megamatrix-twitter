# megamatrix-twitter
A WiFi-connected LED matrix. With a supplied server, it can access the twitter API and display tweets sent to it!

# Setup

First, clone the repo.

## The Matrix
1. In `megamatrix/megamatrix.ino`, replace the SSID and password with your local wifi's details.
2. Flash `megamatrix/megamatrix.ino` to an ESP32 after downloading all necessary libraries. Be sure to use the [ESP32 port of the SmartMatrix3 library](https://github.com/pixelmatix/SmartMatrix/).
3. Connect the ESP32 to the matrix (or matrices) according to this pinout: (be sure to also supply power to the matrix)

| HUB75 Pin | ESP32 GPIO |
| --------- | ---------- |
| R1_PIN    | 2          |
| G1_PIN    | 15         |
| B1_PIN    | 4          |
| R2_PIN    | 16         |
| G2_PIN    | 27         |
| B2_PIN    | 17         |
| A_PIN     | 5          |
| B_PIN     | 18         |
| C_PIN     | 19         |
| D_PIN     | 21         |
| E_PIN     | 12         |
| LAT_PIN   | 26         |
| OE_PIN    | 25         |
| CLK_PIN   | 22         |

4. Turn it all on. The screen should display an IP address if the WiFi is working.
## The Twitter Bot
1. Install all required libraries (numpy, opencv, tweepy, requests)
2. Apply for a twitter access token
3. Replace all required fields in `app.py` with the information required.
4. Start it and enjoy the show!
