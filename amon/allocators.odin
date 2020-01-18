package amon

import "core:mem"
import "core:os"
import "core:sync"
import "core:fmt"
import "core:runtime"

// Fast Leak Detector
Fast_Leak :: struct {
	count, size: int
}

_GLOBAL_FAST_LEAK := Fast_Leak {
	count = 0,
	size = 0,
};

fast_leak_allocator_proc :: proc(allocator_data: rawptr, mode: mem.Allocator_Mode,
                           size, alignment: int,
                           old_memory: rawptr, old_size: int, flags: u64 = 0, loc := #caller_location) -> rawptr {
	self := cast(^Fast_Leak) allocator_data;

	switch mode {
	case .Alloc:
		ptr := os.heap_alloc(size);
		if ptr != nil {
			sync.atomic_add(&self.count, 1, .Release);
			sync.atomic_add(&self.size, size, .Release);
		}
		return ptr;

	case .Free:
		if old_memory != nil {
			sync.atomic_sub(&self.count, 1, .Release);
			sync.atomic_sub(&self.count, old_size, .Release);
		}
		os.heap_free(old_memory);
		return nil;

	case .Free_All:
		// do nothing

	case .Resize:
		if old_memory == nil {
			ptr := os.heap_alloc(size);
			if ptr != nil {
				sync.atomic_add(&self.count, 1, .Release);
				sync.atomic_add(&self.size, size, .Release);
			}
			return ptr;
		}
		ptr := os.heap_resize(old_memory, size);
		sync.atomic_add(&self.size, size - old_size, .Release);
		return ptr;
	}

	return nil;
}

fast_leak_allocator :: proc() -> mem.Allocator {
	return mem.Allocator{
		procedure = fast_leak_allocator_proc,
		data = &_GLOBAL_FAST_LEAK,
	};
}

fast_leak_detect :: proc() -> bool {
	leak_count := sync.atomic_load(&_GLOBAL_FAST_LEAK.count, .Acquire);
	leak_size  := sync.atomic_load(&_GLOBAL_FAST_LEAK.size, .Acquire);
	if leak_count > 0 {
		fmt.eprintf("leak count: %v, leak size %v\n", leak_count, leak_size);
		return false;
	}
	return true;
}


// Location Leak Detector
Loc_Leak :: struct {
	mtx: sync.Mutex,
	count, size: int,
	locs: map[rawptr]runtime.Source_Code_Location,
}

_loc_leak_make :: proc() -> Loc_Leak {
	self := Loc_Leak {
		count = 0,
		size = 0,
		locs = make(map[rawptr]runtime.Source_Code_Location),
	};
	sync.mutex_init(&self.mtx);

	return self;
}

_GLOBAL_LOC_LEAK := _loc_leak_make();

loc_leak_allocator_proc :: proc(allocator_data: rawptr, mode: mem.Allocator_Mode,
                           size, alignment: int,
                           old_memory: rawptr, old_size: int, flags: u64 = 0, loc := #caller_location) -> rawptr {
	self := cast(^Loc_Leak) allocator_data;

	switch mode {
	case .Alloc:
		ptr := os.heap_alloc(size);
		if ptr != nil {
			sync.atomic_add(&self.count, 1, .Release);
			sync.atomic_add(&self.size, size, .Release);

			sync.mutex_lock(&self.mtx);
			self.locs[ptr] = loc;
			sync.mutex_unlock(&self.mtx);
		}
		return ptr;

	case .Free:
		if old_memory != nil {
			sync.atomic_sub(&self.count, 1, .Release);
			sync.atomic_sub(&self.count, old_size, .Release);

			sync.mutex_lock(&self.mtx);
			delete_key(&self.locs, old_memory);
			sync.mutex_unlock(&self.mtx);
		}
		os.heap_free(old_memory);
		return nil;

	case .Free_All:
		// do nothing

	case .Resize:
		if old_memory == nil {
			ptr := os.heap_alloc(size);
			if ptr != nil {
				sync.atomic_add(&self.count, 1, .Release);
				sync.atomic_add(&self.size, size, .Release);

				sync.mutex_lock(&self.mtx);
				self.locs[ptr] = loc;
				sync.mutex_unlock(&self.mtx);
			}
			return ptr;
		}
		ptr := os.heap_resize(old_memory, size);
		sync.atomic_add(&self.size, size - old_size, .Release);

		// update the lock
		sync.mutex_lock(&self.mtx);
		self.locs[ptr] = loc;
		sync.mutex_unlock(&self.mtx);
		return ptr;
	}

	return nil;
}

loc_leak_allocator :: proc() -> mem.Allocator {
	return mem.Allocator{
		procedure = loc_leak_allocator_proc,
		data = &_GLOBAL_LOC_LEAK,
	};
}

loc_leak_detect :: proc() -> bool {
	leak_count := sync.atomic_load(&_GLOBAL_LOC_LEAK.count, .Acquire);
	leak_size  := sync.atomic_load(&_GLOBAL_LOC_LEAK.size, .Acquire);
	if leak_count > 0 {
		sync.mutex_lock(&_GLOBAL_LOC_LEAK.mtx);
		defer sync.mutex_unlock(&_GLOBAL_LOC_LEAK.mtx);

		for _, v in _GLOBAL_LOC_LEAK.locs {
			fmt.eprintf("file: %v, line: %v, col: %v, proc: %v\n", v.file_path, v.line, v.column, v.procedure);
		}
		fmt.eprintf("leak count: %v, leak size %v\n", leak_count, leak_size);
		return false;
	}
	return true;
}
