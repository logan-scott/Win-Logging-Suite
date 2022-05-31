# Win-Logging-Suite
This is a multi-featured keylogger to be deployed on victim machines running Windows.
This keylogger is written in C with a email module for sending the logfile via SMTP written in Python; it also makes heavy use of the win32 API.

# Features
- Keystroke logging
- Active Window logging
- Clipboard logging
- Moving and hiding executables to prevent detection
- Clear popular browsers' cache of saved passwords and data to force retyping
- Receive logfiles via email
- and more (see source code)

# Upcoming
- Update mailer module to support Google's new policy or change email servers
- Detect shutdown and send logfile
- Launch on startup
- Encrypt logfile and build decrypter
- Log devices on network
- Clean up repo
- and more

# Notes and Tips
NOTE: Gmail will no longer support the use of third-party apps to send/receive SMTP mail starting May 30th, 2022.
NOTE: Change username and password in mailer.py and compile to executable with pyInstaller
TIP: Change location of "installed" executables in keylogger.c to somewhere besides "Documents"
TIP: Rename executables to be less obvious
TIP: Hide executables with a "picture" or other file
