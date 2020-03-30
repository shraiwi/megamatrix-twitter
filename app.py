import tweepy
import time
import cv2
import numpy as np
from urllib.request import urlopen
import requests
from io import BytesIO
import threading

consumer_key = "your_consumer_key"          # get twitter api access for these
consumer_secret = "your_consumer_secret"

access_token = "your_access_token"
access_token_secret = "your_access_token_secret"

update_period = 10.0          # how often to query the twitter API for new posts (in seconds)

wall_url = "ip address"       # this sould be shown on startup.
wall_query = "#megamatrix"

auth = tweepy.OAuthHandler(consumer_key, consumer_secret)
auth.set_access_token(access_token, access_token_secret)

api = tweepy.API(auth)

last_url = "null url"

def refresh_wall():
    global last_url

    threading.Timer(update_period, refresh_wall).start()

    public_tweets = tweepy.Cursor(api.search, q=wall_query, count=100, since=time.strftime("%Y-%d-%m"), include_entities=True).items()
    image_urls = [tweet.extended_entities["media"][0]["media_url"] for tweet in public_tweets if "media" in tweet.entities]

    print(last_url)

    if last_url == image_urls[0]:
        return
    
    last_url = image_urls[0]
    #print(image_urls)
    image = np.asarray(bytearray(urlopen(image_urls[0]).read()), dtype=np.uint8)
    image = cv2.imdecode(image, -1)
    image = cv2.resize(image, (64, 64))
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    cv2.imshow("image", image)
    #recent_image.show()
    img_bytes = image.flatten().tobytes()
    #print(len(img_bytes))
    img_bytes
    res = requests.post(wall_url, data=img_bytes, headers={
        "Content-Type":"application/octet-stream",
    })
    #print(res)

refresh_wall()
