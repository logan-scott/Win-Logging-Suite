import smtplib, ssl, os
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText

# email setup
email = ""
password = ""

# establish SMTP connection via TLS
context = ssl.create_default_context()
server = smtplib.SMTP('smtp.gmail.com', 587) # 587 for TLS
server.starttls(context=context)
server.login(email, password)

# gather and prepare logfile
filename =  "log.txt"
f = open(filename, "r")
logfile_attachment = MIMEText(f.read())
logfile_attachment.add_header('Content-Disposition', 'attachment', filename=filename)
message = MIMEMultipart('alternative')
message.attach(logfile_attachment)

# email the logfile
server.sendmail(email, email, message.as_string()) # send and receive from same address
server.quit()
