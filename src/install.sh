#!/bin/bash

APACHECONF="/etc/apache2"
CGIDIRS="/data/cgi-bin;/usr/lib/cgi-bin;/var/www/cgi-bin"
WWWDIRS="/data/www;/var/www/html"
CONFIGSRC="config.ini"
CONFIGINST="authcaller.ini"
CONFIGDIR="/usr/local/etc/"

if [ -d "$APACHECONF" ]; then
  echo "OK - Apache is probably installed."
else
  echo "ERROR - Where is Apache2 installed?"
  exit 1
fi

WEBUSER=`ps -efw\
         | grep apache2\
         | grep -v '^root'\
         | grep -v grep\
         | sed 's/ .*//'\
         | sort\
         | uniq`

if [ -z "$WEBUSER" ]; then
  echo "ERROR - no Apache2 user?"
  exit 2
else
  echo "OK - Apache2 running as $WEBUSER"
fi

REALCGI=""
export IFS=";"
for CGIPATH in $CGIDIRS; do
  if [ -d "$CGIPATH" ]; then
    REALCGI="$CGIPATH"
    break
  fi
done

if [ "" == "$REALCGI" ]; then
  echo "Cannot find CGI folder in $CGIDIRS"
  exit 3
fi

REALWWW=""
export IFS=";"
for WWWPATH in $WWWDIRS; do
  if [ -d "$WWWPATH" ]; then
    REALWWW="$WWWPATH"
    break
  fi
done

if [ "" == "$REALWWW" ]; then
  echo "Cannot find WWW folder in $WWWDIRS"
  exit 4
fi

if [ -f "$CONFIGSRC" ]; then
  echo "OK - $CONFIGSRC exists"
else
  echo "ERROR - $CONFIGSRC missing"
  exit 5
fi

PINFOLDER=`cat "$CONFIGSRC"\
           | grep 'PIN_FOLDER='\
           | sed 's/.*=//'`

if [ -d "$PINFOLDER" ]; then
  echo "OK - $PINFOLDER exists"
else
  sudo mkdir "$PINFOLDER"\
  && sudo chmod 755 "$PINFOLDER"\
  && sudo chown "$WEBUSER" "$PINFOLDER"\
  || ( echo "ERROR - Failed to create $PINFOLDER" ; exit 6 )
fi

if [ -d "$CONFIGDIR" ]; then
  echo "OK - $CONFIGDIR exists"
else
  sudo mkdir "$CONFIGDIR"\
  && sudo chown root.root "$REALWWW/authcaller"\
  && sudo chmod 755 "$CONFIGDIR"\
  || ( echo "ERROR - Failed to create $CONFIGDIR" ; exit 7 )
fi

sudo install -o root -g root -m 644 "$CONFIGSRC" "$CONFIGDIR/$CONFIGINST"\
  || ( echo "ERROR - Failed to install $CONFIGDIR/$CONFIGINST" ; exit 8 )

make authcaller\
  || ( echo "ERROR - Failed to build authcaller"; exit 9 )

sudo install -o root -g root -m 755 authcaller "$REALCGI"\
  || ( echo "ERROR - Failed to install authcaller"; exit 10 )

if [ -d "$REALWWW/authcaller" ]; then
  echo "OK - $REALWWW/authcaller exists"
else
  sudo mkdir "$REALWWW/authcaller"\
  && sudo chown root.root "$REALWWW/authcaller"\
  && sudo chmod 755 "$REALWWW/authcaller"\
  || ( echo "ERROR - Failed to create $REALWWW/authcaller" ; exit 11 )
fi

sudo install -o root -g root -m 644 ui.html "$REALWWW/authcaller"\
  || ( echo "ERROR - Failed to install authcaller/ui.html"; exit 12 )

sudo install -o root -g root -m 644 ui.css "$REALWWW/authcaller"\
  || ( echo "ERROR - Failed to install authcaller/ui.css"; exit 12 )

sudo install -o root -g root -m 644 ui.js "$REALWWW/authcaller"\
  || ( echo "ERROR - Failed to install authcaller/ui.js"; exit 12 )

