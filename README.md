Espruino Host
=============

Tool to run in the background and provide a WebSocket server
that http://espruino.com/ide can use to access BLE and Serial
devices.


Usage
-----

* Run the tool for your platform in the `builds` folder
* Go to https://espruino.github.io/EspruinoWebIDE/
* In settings (top right), `Communications`, ensure `Websocket URL` is set to the URL reported in the tool (probably `wss://localhost:31234`)
* **MacOS Note:** If you're prompted for a keychain password in the next step, you do need to enter one. Do not close/cancel the window or you will have to restart your Mac before you'll be able to get Espruino Host working! 
* Instead of `wss://localhost:31234`, go to `https://localhost:31234` in your browser, and if prompted by a security warning click 'advanced' and then 'ignore' (or follow the `Setup SSL Certificate` heading below). A page seting 'Empty Response' is fine.
* Now connect on the Web IDE page you first opened, and you're sorted!


Setup SSL Certificate
---------------------

To avoid the issues around permissions, you can pre-install the certificate:

* Go to Chrome Settings
* Scroll down & click Advanced
* Manage Certificates
  * Linux -> `IMPORT`
  * Mac OS -> `File` -> `Import Items...`
* Choose `localhost.p12` with password `espruino`


Creating your own key/certificate
---------------------------------

EspruinoHub uses a built-in certificate, but you can:


Create with:

```
openssl req -newkey rsa:2048 -nodes -keyout localhost.key -x509 -out localhost.cert
# Then edit the app.qrc file in Qt Creator and update
# the localhost.* files
```

Turn the certificate into something that can be loaded by most web browsers with:

```
openssl pkcs12 -export -clcerts -in localhost.cert -inkey localhost.key -out localhost.p12
```


Protocol
--------

* `->` means sent to EspruinoHost
* `<-` means returned from EspruinoHost


```
-> {"type":"version"}

<- {
    "type": "version",
    "version": "0.1"
}
```

### List available ports

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

### Connect to a port

```
-> {"type":"connect","interface":"serial","path":"/dev/ttyACM0","baud":9600}
-> {"type":"connect","interface":"bluetooth","path":"C1:6F:4D:4A:C1:27"}

<- {"type":"connect"}
or
<- {"type":"error", "message":""}
```


### Write data

```
-> {"type":"write","data":"\u0003"}
-> {"type":"write","data":"LED.set()\r\n"}

// when write complete...
<- {"type":"write",count:234}
```

### Data received from remote device

```
<- {"type":"read","data":" ... "}
```

### To disconnect


```
-> {"type":"disconnect"}
```

Building
--------

### LINUX

You need to install these dependencies *before installing Qt and building* or Bluetooth will silently fall back to a dummy version

```
sudo apt-get install libbluetooth-dev bluetooth blueman bluez libusb-dev libdbus-1-dev bluez-hcidump bluez-tools
```

### Windows

You'll need to build against Visual Studio 64 bit, or Bluetooth support won't work!


TODO
----

* Send 'status' messages during BLE connect to avoid the multi-second second pause during connection
* Deal with BLE connection errors properly (probably not handled right at the moment)
* `QSettings` to store persistent settings
* Ability to access local files *within some predefined directory* - for modules/etc
* Have option to download IDE locally (as a zip?), then serve the whole thing up over the WebSocket URL

