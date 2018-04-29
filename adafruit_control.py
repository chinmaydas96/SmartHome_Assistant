import random
import sys
import time
from Adafruit_IO import MQTTClient
ADAFRUIT_IO_KEY = 'd85e000da37f41928ec89a548957de52'
ADAFRUIT_IO_USERNAME = 'debasis1'  # See https://accounts.adafruit.com
# Define callback functions which will be called when certain events happen.


def connected(client):
    print('Connected to Adafruit IO!  Listening for DemoFeed changes...')
    # Subscribe to changes on a feed named DemoFeed.
    client.subscribe('ledBrightness')
    client.subscribe('light')
    client.subscribe('fan')
    client.subscribe('blind')


def disconnected(client):
    # Disconnected function will be called when the client disconnects.
    print('Disconnected from Adafruit IO!')
    sys.exit(1)


def message(client, feed_id, payload):
    print('Feed {0} received new value: {1}'.format(feed_id, payload))


# Create an MQTT client instance.
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
# Setup the callback functions defined above.
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message
# Connect to the Adafruit IO server.
client.connect()
client.loop_background()


def applienceONOFF(mode, status):
    print('Publishing {0} to {1}'.format(mode, status))
    client.publish(mode, status)


def appliencePWM(mode, statusValue):
    print('Publishing {0} to {1}'.format(mode, statusValue))
    # client.publish('ledBrightness', value1)
    client.publish(mode, statusValue)


# values = ['ON','OFF']
# value1 = random.randint(0, 1020)
# value2 = random.randint(0,700)
# i = random.randint(0, 1)
# value = values[i]
# ledBrightness,Fan,light,blind
