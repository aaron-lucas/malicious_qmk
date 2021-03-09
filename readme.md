Malicious QMK
--------------

## DISCLAIMER!!!

This project contains code which has the ability to record and transmit sensitive information without a user's consent. It exists as a **proof-of-concept only** and a user should **never** use this unless they have a full understanding of what this firmware can do.

## Summary

This is a modified version of the popular QMK keyboard firmware which contains malicious code that logs the user's keystrokes, then implements a [rubber ducky](https://www.linux.org/threads/usb-rubber-ducky.4464/) style attack to open a network connection and send the keylogged data to a remote machine by sending a series of keystrokes to the user's computer.

This is a fork of the existing [QMK repo](https://github.com/qmk/qmk_firmware) and the QMK readme is renamed as `qmk_readme.md`
