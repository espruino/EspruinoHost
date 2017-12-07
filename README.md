


openssl req -newkey rsa:2048 -nodes -keyout localhost.key -x509 -days 365 -out localhost.cert

Put in the same folder as the executable

### LINUX

You need to install these dependencies before building or Bluetooth will silently fall back to a dummy version

```
sudo apt-get install libbluetooth-dev bluetooth blueman bluez libusb-dev libdbus-1-dev bluez-hcidump bluez-tools
```



```
{"type":"list"}

{
    "ports": [
        {
            "description": "STM32 Virtual ComPort",
            "path": "/dev/ttyACM0",
            "interface": "serial"
        },
        {
            "description": "",
            "path": "/dev/ttyS0",
            "interface": "serial"
        }
    ],
    "type": "list"
}
```

```
{"type":"connect","interface":"serial","path":"/dev/ttyACM0","baud":9600}

{"type":"connect"}
or
{"type":"error", "message":""}
```

```
{"type":"write","data":"\u0003"}
{"type":"write","data":"LED.set()\r\n"}

// when write complete...
{"type":"write",count:234}
```
