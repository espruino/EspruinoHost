Espruino Host
=============

Tool to run in the background and provide a WebSocket server
that http://espruino.com/ide can use to access BLE and Serial
devices.


Setup
-----

openssl req -newkey rsa:2048 -nodes -keyout localhost.key -x509 -days 365 -out localhost.cert

Put in the same folder as the executable

### LINUX

You need to install these dependencies *before installing Qt and building* or Bluetooth will silently fall back to a dummy version

```
sudo apt-get install libbluetooth-dev bluetooth blueman bluez libusb-dev libdbus-1-dev bluez-hcidump bluez-tools
```


```
-> {"type":"version"}

<- {
    "type": "version",
    "version": "0.1"
}
```

```
-> {"type":"list"}

<- {
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
-> {"type":"connect","interface":"serial","path":"/dev/ttyACM0","baud":9600}
-> {"type":"connect","interface":"bluetooth","path":"C1:6F:4D:4A:C1:27"}

<- {"type":"connect"}
or
<- {"type":"error", "message":""}
```

```
-> {"type":"write","data":"\u0003"}
-> {"type":"write","data":"LED.set()\r\n"}

// when write complete...
<- {"type":"write",count:234}
```

```
<- {"type":"read","data":" ... "}
```

```
-> {"type":"disconnect"}
```


TODO
----

* `QSettings` to store persistent settings
* Ability to access local files *within some predefined directory* - for modules/etc
*
