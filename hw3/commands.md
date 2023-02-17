```sh
# zip all files
zip load.zip run.sh unisexBathroom.c

# Copy file to remote
scp load.zip $USER@student-shell.sys.kth.se:~/concurrent/hw3/load.zip

# Unzip file on remote
ssh $USER@student-shell.sys.kth.se "unzip -o ~/concurrent/hw3/load.zip -d ~/concurrent/hw3"

# Enter remote
ssh $USER@student-shell.sys.kth.se
```
