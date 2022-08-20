ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
GENERAT = *.o app

all: app	

app: main.o aht10.o dht11.o led.o ssd1306.o common.o at24c64.o
		$(CROSS_COMPILE)gcc $^ -o $@

%.o: %.c
		$(CROSS_COMPILE)gcc -c $< -o $@

clean:
		rm -f $(GENERAT)
