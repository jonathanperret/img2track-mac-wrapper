# img2track wrapper for compatibility with Apple Silicon

## Issue

[img2track](https://daviworks.com/knitting/index.html) is software to send images to Brother knitting machines. It uses an FTDI USB-to-serial cable to communicate with the machine.

The current version runs on Apple Silicon (M1, etc.) Macs, but can't detect the FTDI cable.

Tracing `img2track`'s API calls reveals that it hits a deprecation in a macOS API (the `IOUSBDevice` device class was renamed to `IOUSBHostDevice`), that prevents the library it uses to list serial ports (`PySerial`) from gathering details about the ports it does find, and since `img2track` insists on only opening a port if it is associated with an FTDI device, that prevents the search from succeeding.

Here’s the actual bug as reported in 2020 against PySerial: https://github.com/pyserial/pyserial/issues/545 — and a fix for that issue was included in version 3.5 of PySerial released in November of 2020.

## What this tool does

This repository builds a dynamic library that is meant to be injected into `img2track`'s process using the `DYLD_INSERT_LIBRARIES` environment variable. The library will intercept calls to the `IOObjectGetClass` function and return `IOUSBDevice` when the OS returns `IOUSBHostDevice`, thus fulfilling the expectations of the legacy version of `PySerial`.

## Modifying `img2track.app` to run on your Apple Silicon Mac

- Download the `img2track-mac-wrapper.zip` file from https://github.com/jonathanperret/img2track-mac-wrapper/releases/latest;
- Double-click the downloaded file to extract the `img2track` and `libiokitrewrite.dylib` files it contains;
- Make a copy of `img2track.app` to modify;
- Right-click your copy of `img2track.app` and choose to view package contents;
- Navigate to `Contents/MacOS`;
- Rename the `img2track` executable to `img2track.real`;
- Move the two files extracted from the ZIP file above to that `Contents/MacOS` directory.

Now return to the folder containing your copy of `img2track.app` and double-click it: it should open the app and the app should be able to detect and use the cable.

If it works, you can then drag the modified `img2track.app` to `/Applications` to overwrite the original.

## Building and testing

Run `make`. This will create the library in the current directory.

Make sure `img2track` is present in `/Applications/img2track.app`, then run `make run` to start `img2track` with the library injected. If debug logging is enabled in `img2track` you should see it log that it found the FTDI serial port.

To create the release ZIP file run `make package`.
