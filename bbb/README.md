#Startup command   
Assuming the usb keyboard is recognized as  /dev/input/event1   
```
/root/rene/gadgetkbd/gadgetkbd /dev/input/event1

```

#Automatic Startup at Boot   
Create a startup file   
```
/etc/init.d/gadgetkb    
```

See the content from the file 
```
etc_init.d_gadgetkbd
```


#Deactivate the network over the usb port   


g_ether is a monolithic driver, it does not coexist with the keyboard emulator   







