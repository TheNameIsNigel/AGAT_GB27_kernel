export ARCH=arm

#export CROSS_COMPILE=<Local Android Path>/android/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
export CROSS_COMPILE=/home/agat/E4GT/arm-eabi-4.4.3/bin/arm-eabi-

make u1_na_spr_defconfig
make
