# OMPTestCase

## Dependencies

To install the required dependencies run the following command: `sudo apt install libqt5dbus5 qtbase5-dev qtchooser`

## Building and running

To build and run the project open 3 terminal windows.

### com.system.permissions

In the first terminal execute the following commands:

```shell
cd systemPermissions
qmake
make
./systemPermissions
```

The service should be running by now. You can check this by
running `gdbus call -e -d com.system.permissions -o / -m com.system.permissions.RequestPermission 0` in separate
terminal window. Output should be `()`.

### com.system.time

In the second terminal execute the following commands:

```shell
cd systemTime
qmake
make
./systemTime
```

The service should be running by now. You can check this by
running `gdbus call -e -d com.system.time -o / -m com.system.time.GetSystemTime ` in separate
terminal window. Output should be current time in millisecond `(uint64 ...,)` or an error saying the application doesn't
have the permission.

### Running the test app

In the third terminal execute the following commands:

```shell
cd clientApp
qmake
make
./clientApp
```

After executing, the program will try to get current time using `com.system.time` service. If it fails
with `UnauthorizedAccess` error, it will try to request the permission and get current time again.