echo "COMPILING THE FILES "
sleep 1
make -f SFS.make 
sleep 1

echo " "
rm -rf fs1 || echo "CREATING MOUNT-POINT"
sleep 1

mkdir fs1 
echo "MOUNT POINT CREATED!!!"

sleep 1
echo "MOUNTING THE FILE SYSTEM AT MOUNT-POINT IN FOREGROUND MODE"
sleep 1
./sfs -f fs1

