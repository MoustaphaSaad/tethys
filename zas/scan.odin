package zas

import "core:unicode"
import "core:unicode/utf8"
import "core:strings"
import "core:fmt"

_rune_is_letter :: proc(c: rune) -> bool do return unicode.is_alpha(c) || c == '_';
_rune_is_digit :: proc(c: rune) -> bool do return c >= '0' && c <= '9';
_rune_digit_value :: proc(c: rune) -> int {
	if _rune_is_digit(c) do return cast(int)c - cast(int)'0';
	else if c >= 'a' && c <= 'f' do return cast(int)c - cast(int)'a' + 10;
	else if c >= 'A' && c <= 'F' do return cast(int)c - cast(int)'A' + 10;
	else do return 16;
}

_is_same_keyword :: proc(a, b: string, case_insensitive: bool) -> bool {
	a_count := strings.rune_count(a);
	b_count := strings.rune_count(b);
	if a_count != b_count do return false;

	if case_insensitive {
		for i := 0; i < a_count; i += 1 {
			a_rune, _ := utf8.decode_rune(transmute([]u8)a[i:]);
			b_rune, _ := utf8.decode_rune(transmute([]u8)b[i:]);

			if unicode.to_lower(a_rune) != unicode.to_lower(b_rune) do return false;
		}
		return true;
	} else {
		return a == b;
	}
}

@(private)
Scanner :: struct {
	src: ^Src,
	it: int,
	c: rune,
	pos: Pos,
	line_begin: int,
}

_scanner_new :: proc(src: ^Src) -> Scanner {
	self := Scanner {
		src = src,
		it = 0,
		c = 0,
		pos = Pos {1, 0},
		line_begin = 0,
	};

	c, _ := utf8.decode_rune(self.src.content[self.it:]);
	self.c = c;
	return self;
}

_scanner_eof :: proc(self: ^Scanner) -> bool {
	return self.it >= len(self.src.content);
}

_scanner_eat :: proc(self: ^Scanner) -> bool {
	if _scanner_eof(self) do return false;

	prev_it := self.it;
	prev_c  := self.c;

	c, rune_size := utf8.decode_rune(self.src.content[self.it:]);
	self.c = c;
	self.it += rune_size;

	self.pos.col += 1;

	if prev_c == '\n' {
		self.pos.col = 1;
		self.pos.line += 1;

		append(&self.src.lines, cast(string)self.src.content[self.line_begin:self.it]);
		self.line_begin = self.it;
	}
	return true;
}

_scanner_skip_whitespaces :: proc(self: ^Scanner) {
	for unicode.is_white_space(self.c) {
		if _scanner_eat(self) == false do break;
	}
}

_scanner_id :: proc(self: ^Scanner) -> string {
	begin_it := self.it;
	for _rune_is_letter(self.c) || _rune_is_digit(self.c) || self.c == '.' {
		if _scanner_eat(self) == false do break;
	}
	return cast(string)self.src.content[begin_it:self.it];
}

_scanner_digits :: proc(self: ^Scanner, base: int) -> bool {
	found := false;
	for _rune_digit_value(self.c) < base {
		found = true;
		if _scanner_eat(self) == false do break;
	}
	return found;
}

_scanner_num :: proc(self: ^Scanner, tkn: ^Tkn) {
	begin_it := self.it;
	begin_pos := self.pos;
	tkn.kind = .Const_Integer;

	if self.c == '-' || self.c == '+' do _scanner_eat(self);

	if self.c == '0' {
		backup_it := self.it;
		backup_pos := self.pos;
		_scanner_eat(self); // for the 0

		base := 0;
		switch self.c {
		case 'b', 'B': base = 2;
		case 'o', 'O': base = 8;
		case 'd', 'D': base = 10;
		case 'x', 'X': base = 16;
		}

		if base != 0 {
			if _scanner_digits(self, base) == false {
				append(&self.src.errs, Err{
					pos = begin_pos,
					rng = {begin_it, self.it},
					msg = fmt.aprintf("illegal int literl %v", self.c),
				});
			}
			tkn.str = cast(string)self.src.content[begin_it:self.it];
			return;
		}

		// this is not a 0x number
		self.it = backup_it;
		c, _ := utf8.decode_rune(self.src.content[self.it:]);
		self.c = c;
		self.pos = backup_pos;
	}

	// since this is not a 0x number
	// it might be an integer or float so parse a decimal number anyway
	if _scanner_digits(self, 10) == false {
		append(&self.src.errs, Err{
			pos = begin_pos,
			rng = {begin_it, self.it},
			msg = fmt.aprintf("illegal int literal '%v'", self.c),
		});
	}

	//float part
	if self.c == '.' {
		tkn.kind = .Const_Float;
		_scanner_eat(self); //for the .
		//parse the after . part
		if _scanner_digits(self, 10) == false {
			append(&self.src.errs, Err{
				pos = begin_pos,
				rng = {begin_it, self.it},
				msg = fmt.aprintf("illegal float literal '%v'", self.c),
			});
		}
	}

	//scientific notation part
	if self.c == 'e' || self.c == 'E' {
		tkn.kind = .Const_Float;
		_scanner_eat(self); //for the e

		if self.c == '-' || self.c == '+'{
			_scanner_eat(self);
		}

		if _scanner_digits(self, 10) == false {
			append(&self.src.errs, Err{
				pos = begin_pos,
				rng = {begin_it, self.it},
				msg = fmt.aprintf("illegal float literal '%v'", self.c),
			});
		}
	}

	//finished the parsing of the number whether it's a float or int
	tkn.str = cast(string)self.src.content[begin_it:self.it];
}

_scanner_comment :: proc(self: ^Scanner) -> string {
	begin_it := self.it;
	prev := self.c;
	for self.c != '\n' {
		prev = self.c;
		if _scanner_eat(self) == false do break;
	}

	end_it := self.it;
	if prev == '\r' do end_it -= 1;

	return cast(string)self.src.content[begin_it:end_it];
}

_scanner_string :: proc(self: ^Scanner, tkn: ^Tkn) {
	begin_it := self.it;
	end_it := self.it;

	prev := self.c;
	// eat all runes even those escaped by \ like \"
	for self.c != '"' || prev == '\\' {
		if _scanner_eat(self) == false {
			append(&self.src.errs, Err{
				pos = self.pos,
				msg = "unterminated string"
			});
			break;
		}
		prev = self.c;
	}

	end_it = self.it;
	_scanner_eat(self); // for the "
	tkn.str = cast(string)self.src.content[begin_it:end_it];
	tkn.rng.begin = begin_it;
	tkn.rng.end = end_it;
}

_scanner_tkn :: proc(self: ^Scanner) -> Tkn {
	_scanner_skip_whitespaces(self);

	if _scanner_eof(self) do return Tkn{};

	tkn := Tkn{
		pos = self.pos,
		rng = {self.it, self.it}
	};
	no_rng := false;
	if _rune_is_letter(self.c) {
		tkn.kind = .ID;
		tkn.str = _scanner_id(self);

		//let's loop over all the keywords and check them
		for i := cast(int)Tkn_Kind.Keyword__Begin; i < cast(int)Tkn_Kind.Keyword__End; i += 1 {
			kind := cast(Tkn_Kind)i;
			if _is_same_keyword(tkn.str, TKN_DESC[kind].mnemonic, !tkn_kind_is_ctype(kind)) {
				tkn.kind = kind;
				break;
			}
		}
	} else if _rune_is_digit(self.c) {
		_scanner_num(self, &tkn);
	} else if self.c == '-' || self.c == '+' {
		next, _ := utf8.decode_rune(self.src.content[self.it:]);
		if _rune_is_digit(next) do _scanner_num(self, &tkn);
	} else {
		c := self.c;
		begin_pos := self.pos;
		_scanner_eat(self);

		switch(c) {
		case ':':
			tkn.kind = .Colon;
			tkn.str = ":";
		case '(':
			tkn.kind = .Open_Paren;
			tkn.str = "(";
		case ')':
			tkn.kind = .Close_Paren;
			tkn.str = ")";
		case ',':
			tkn.kind = .Comma;
			tkn.str = ",";
		case ';':
			tkn.kind = .Comment;
			tkn.str = _scanner_comment(self);
		case '"':
			tkn.kind = .Const_String;
			_scanner_string(self, &tkn);
			no_rng = true;
		case:
			append(&self.src.errs, Err{
				pos = begin_pos,
				msg = fmt.aprintf("illegal character '%v'", self.c),
			});
		}
	}
	if no_rng == false do tkn.rng.end = self.it;
	return tkn;
}


src_scan :: proc(self: ^Src) -> bool {
	scanner := _scanner_new(self);
	for {
		if tkn := _scanner_tkn(&scanner); tkn.kind != .None {
			append(&self.tkns, tkn);
		} else {
			break;
		}
	}
	return len(self.errs) == 0;
}