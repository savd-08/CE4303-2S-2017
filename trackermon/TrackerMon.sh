#!/bin/bash

##########################
# Variables              #
##########################

INSTALL='sudo install --owner=root --group=root --mode=644'


##########################
# Check permissions      #
##########################

# Check for permissions errors
if [ `id -u` == 0 ]; then
    echo "[ERROR] This script should not be executed as root. Run it a a sudo-capable user."
    exit 1
fi

# Check if user can do sudo
echo "Se necesitan permisos de administrador"
if [ `sudo id -u` != 0 ]; then
    echo "This user cannot cast sudo or you typed an incorrect password (several times)."
    exit 1
else
    echo "Correctly authenticated."
fi

echo "Instaling trackermon daemon"
sleep 1
sudo cp daemon/trackermon /etc/init.d/
echo "Creating file configuration to TrackerMon.. done "
sudo chmod +x /etc/init.d/trackermon
echo "Change permission"
echo "daemon trackermon created"
make
sudo cp trackermon /usr/bin/
sudo chmod +x /usr/bin/trackermon
sudo mkdir -p /etc/trackermon/
sudo cp config.conf /etc/trackermon/
echo " you can test command: /etc/init.d/trackermon start|stop|restart|status as root "
sudo ln -s /etc/init.d/trackermon /etc/rc5.d/S30trackermon
sudo ln -s /etc/init.d/trackermon /etc/rc3.d/S30trackermon
sudo ln -s /etc/init.d/trackermon /etc/rc6.d/K05trackermon
sudo ln -s /etc/init.d/trackermon /etc/rc0.d/K05trackermon
