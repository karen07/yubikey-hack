yubikey-hack: get_token.c
	gcc -O2 get_token.c -Wall -o yubikey-hack

clean:
	rm yubikey-hack

install:
	sudo cp yubikey-hack /usr/bin/
	sudo cp yubikey-hack.service /usr/lib/systemd/system/
	sudo systemctl daemon-reload
	sudo systemctl enable yubikey-hack
	sudo systemctl start yubikey-hack

remove:
	sudo systemctl stop yubikey-hack
	sudo systemctl disable yubikey-hack
	sudo systemctl daemon-reload
	sudo rm /usr/lib/systemd/system/yubikey-hack.service
	sudo rm /usr/bin/yubikey-hack
