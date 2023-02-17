if [ $# -lt 3 ]; then
	echo "Must provide 3 arguments"
	exit 1
fi

if [ -f ./unisexBathroom ]; then
        rm ./unisexBathroom
fi

gcc unisexBathroom.c -lpthread -o unisexBathroom

echo ""
echo "Compiled unisexBathroom.c"
echo "Running for $1 visits with $2 men and $3 women"
echo ""

./unisexBathroom $1 $2 $3