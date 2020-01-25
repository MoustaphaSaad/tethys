package zvm

import "core:strings"
import "core:runtime"
import "core:os"
import "core:mem"

Section_Type :: enum u32 { Constant, Bytecode }
Section :: struct {
	type: Section_Type,
	name: string,
	data: []byte,
}

section_constant_new :: proc(name: string, bytes: []byte) -> Section {
	data := make([]byte, len(bytes));
	runtime.copy(data, bytes);

	return Section {
		name = strings.clone(name),
		type = .Constant,
		data = data,
	};
}

section_bytecode_new :: proc(name: string, bytes: []byte) -> Section {
	data := make([]byte, len(bytes));
	runtime.copy(data, bytes);

	return Section {
		name = strings.clone(name),
		type = .Bytecode,
		data = data,
	};
}

section_free :: proc(self: ^Section) {
	delete(self.name);
	delete(self.data);
}

section_save :: proc(self: Section, f: os.Handle) {
	// write type
	x := self.type;
	os.write(f, mem.ptr_to_bytes(&self.type));

	// write name
	count := u32(len(self.name));
	os.write(f, mem.ptr_to_bytes(&count));
	os.write_string(f, self.name);

	// write data
	count = u32(len(self.data));
	os.write(f, mem.ptr_to_bytes(&count));
	os.write(f, self.data);
}

section_load :: proc(f: os.Handle) -> (self: Section) {
	// read type
	os.read(f, mem.ptr_to_bytes(&self.type));

	// read name
	count: u32 = 0;
	os.read(f, mem.ptr_to_bytes(&count));
	name := make([]byte, count);
	os.read(f, name);
	self.name = cast(string)name;

	// read data
	os.read(f, mem.ptr_to_bytes(&count));
	self.data = make([]byte, count);
	os.read(f, self.data);

	return;
}


Reloc :: struct {
	source_name: string,
	target_name: string,
	source_offset: u64
}

Pkg :: struct {
	sections: map[string]Section,
	relocs: [dynamic]Reloc,
}

pkg_new :: proc() -> Pkg {
	return Pkg {
		sections = make(map[string]Section),
		relocs = make([dynamic]Reloc),
	};
}

pkg_free :: proc(self: ^Pkg) {
	for _, section in self.sections {
		delete(section.name);
		delete(section.data);
	}
	delete(self.sections);

	for reloc in self.relocs {
		delete(reloc.source_name);
		delete(reloc.target_name);
	}
}

pkg_proc_add :: proc(self: ^Pkg, name: string, bytes: []byte) {
	assert(!(name in self.sections), "symbol name redefinition");

	section := section_bytecode_new(name, bytes);
	self.sections[section.name] = section;
}

pkg_constant_add :: proc(self: ^Pkg, name: string, bytes: []byte) {
	assert(!(name in self.sections), "symbol name redefinition");

	section := section_constant_new(name, bytes);
	self.sections[section.name] = section;
}

pkg_reloc_add :: proc(self: ^Pkg, source_name: string, source_offset: u64, target_name: string)
{
	append(&self.relocs, Reloc {
		source_name = strings.clone(source_name),
		target_name = strings.clone(target_name),
		source_offset = source_offset,
	});
}

pkg_save :: proc(self: ^Pkg, filename: string) {
	f, err := os.open(path = filename, mode = os.O_WRONLY);
	assert(err == os.ERROR_NONE);
	defer os.close(f);

	section_count := u32(len(self.sections));
	os.write(f, mem.ptr_to_bytes(&section_count));
	for _, section in self.sections do section_save(section, f);


}