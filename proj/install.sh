if [ "$(id -u)" != "0" ]; then
   echo "ERROR: this script must be run as root!" 1>&2
   exit 1
fi
cd code
echo -e "\t(  0%) cleaning previous build files..."
make clean
echo -e "\t( 20%) invoking make..."
make
echo -e "\t( 60%) configuring arkanix as system service..."
cd ..
cp -v conf/arkanix /etc/system.conf.d/
echo -e "\t( 70%) creating game directory..."
mkdir /home/lcom/arkanix
echo -e "\t( 80%) begin copying files...\n"
cd code
strip --strip-all arkanix
cp -v arkanix /home/lcom/arkanix/
cd ..
cp -vr res /home/lcom/arkanix/
echo -e "\t(100%) creating highscores file...\n"
touch /home/lcom/arkanix/highscores.aks
echo -e "\aArkanix has been successfully installed, enjoy your new game!"