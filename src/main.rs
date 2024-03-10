use std::io::{Read, Write};

extern crate tun;

fn main() {
	let mut config = tun::Configuration::default();
	config.address((10, 1, 1, 1))
	       .netmask((255, 255, 255, 255))
	       .name("rrefl0")
	       .up();

	#[cfg(target_os = "linux")]
	config.platform(|config| {
		config.packet_information(true);
	});

	let mut dev = tun::create(&config).unwrap();
	let mut buf = [0; 4096];

	loop {
		dev.read(&mut buf).unwrap();
		dev.write(&buf).unwrap();
	}
}