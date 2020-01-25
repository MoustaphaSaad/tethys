package main

import "core:fmt"

Koko :: struct {
	x: int,
}

koko_save :: proc(k: int) {
	fmt.println(&k);
}

main :: proc() {
	k: int;
	koko_save(k);
}
