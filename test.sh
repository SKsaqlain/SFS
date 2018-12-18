

echo " "
echo "MOVING TO THE FILE SYSTEM"
cd fs1
sleep 1
echo " "
echo "CREATING FOLDER ON THE FILE SYSTEM"
mkdir testfolder1
mkdir testfolder2
cd testfolder1
mkdir childfolder1
mkdir childfolder2
cd ..
echo "FOLDER CREATED"

sleep 1
echo " "
echo "CREATING A TEXT FILE"
echo "apple">file.txt
echo "TEXT FILE CREATED"
sleep 1
echo "FILE SYSTEM TREE"
echo "root"
echo "   |--testfolder1"
echo "   |            |--childfolderr1"
echo "   |            |--childfolderr2"
echo "   |--testfolder2"
echo "   |--file.txt"
sleep 1
cd ..
echo " "
echo "TO UNMOUNT THE FILE SYSTEM TYPE 'fusermount -u MOUNT-POINT'"

