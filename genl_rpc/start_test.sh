cd kernelmod
make
sudo insmod genl_test.ko
cd ..
make
./genl_client hello